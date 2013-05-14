#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <openssl/evp.h>
#include <unistd.h>

#include "spoon.h"
#include "signatures.c"
#include "parse.h"


// look at difference between size_t and off_t
void extract_to_footer(FILE *img, size_t base, struct signature_s *sig, FILE *log) {
    char buffer[pagesize], fname[32], digest[EVP_MAX_MD_SIZE];
    int i, read, offset, size = 0;
    FILE *new;
    EVP_MD_CTX mdctx;
    pid_t cpid;

    sprintf(fname, "%jd%s", base, sig->extension);
    new = fopen(fname, "w");
    
    cpid = fork();
    
    if (cpid != 0) {
        if (!quiet) {
            printf("%s found at position %jd. Extracting to %s...\n", sig->extension + 1, base, fname);
            fflush(stdout); 
        }

        fread(buffer, 1, 1, img);
        return;
    }
    
    fseeko(img, base, SEEK_SET);
    EVP_DigestInit(&mdctx, EVP_md5());

    while ((read = fread(buffer, 1, pagesize, img)) != 0) {
        
        // Once we reach the maximum file length, stop extracting the file
        if (size > sig->length)
            break;

        // If we haven't found a footer, look at the next page
        else if ( (offset = match_sequence( buffer, read, sig->footer )) == NO_MATCH) {
            fwrite(&buffer, 1, pagesize - margin, new);
            size += pagesize - margin;
            EVP_DigestUpdate(&mdctx, &buffer, pagesize - margin);

            if (read > pagesize - margin)
                fseeko(img, -margin, SEEK_CUR);
        }

        // Found a footer
        else {
            fwrite(&buffer, 1, offset + strlen(sig->footer), new);
            EVP_DigestUpdate( &mdctx, &buffer, offset + strlen(sig->footer) );
            
            // account for optional comment at the end of a zipfile
            if ( strcmp( sig->extension, ".zip" ) == 0) {
                short zipCommentLen = 0;
                memcpy( &zipCommentLen, &buffer[offset + 20], 2);
                fwrite( &buffer[offset + 4], 1, 18 + zipCommentLen, new);
                EVP_DigestUpdate(&mdctx, &buffer[offset+4], 18 + zipCommentLen);

                fseeko(img, offset + 22 + zipCommentLen - read, SEEK_CUR);
            } 
            break;
        }
    }
    
    // Complete MD5 of extracted file
    EVP_DigestFinal_ex(&mdctx, digest, NULL);
    EVP_MD_CTX_cleanup(&mdctx);

    // Write hash and filename to files.log
    for (i=0; i<16; i++)
        fprintf(log, "%02x", (unsigned char) digest[i]);
    fprintf(log, ",%s\n", fname);
    fflush(log);
    
    fclose(new);
    exit(0);
}


int match_sequence(char *buffer, int buflen, char *sequence) {
    int i, j, found;

    /* 
     * Only examine sequences that start in the first (pagesize - margin) bytes.
     * The margin allows us to catch sequences that may span two pages.
     * Sequences that start in the margin will be found in the next block when
     * match_sequence is called again.
     */ 
    if (buflen > pagesize - margin)
        buflen = pagesize - margin;
    
    for (i=0; i < buflen; i++) {
        found = 1;
        for (j=0; j < strlen(sequence); j++) {       
            if (buffer[i+j]  !=  sequence[j]) {
                found = 0;
                break;
            }
        }
            
        if (found) {
            return i;
        }
    }

    return NO_MATCH;
}


int parser_parse(FILE *img, struct signatures_s *sigs, FILE *logfile) {
    char *buf = (char *) malloc(pagesize);
    char *bufcpy = buf;
    size_t base = 0, basecpy = 0;
    int i, read, readcpy, offset;
    struct signature_s *sig = sigs->first;
    struct match_s *match;
    
    while (1) {
      
        readcpy = read = fread(buf, 1, pagesize, img);

        if (read == 0)
            break;

        while (sig != NULL) {
            if ((offset = match_sequence(buf, read, sig->header)) != NO_MATCH) {
                extract_to_footer(img, base + offset, sig, logfile);
                base += offset + 1;
                buf  += offset + 1;
                read -= offset + 1;
                continue;
            }
            
            base = basecpy;
            buf  = bufcpy;
            read = readcpy;
            sig  = sig->next;
        }

        basecpy = base = base + read;
        sig = sigs->first;
    }

    free(buf);
    return 0;
}
