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

void extract_to_footer(FILE *img, struct signature_s *sig, FILE *log) {
    char buffer[pagesize], fname[32], digest[EVP_MAX_MD_SIZE];
    int i, read, offset, size = 0;
    off_t pos = ftello(img);
    FILE *new;
    EVP_MD_CTX mdctx;
    pid_t cpid;

    sprintf(fname, "%jd%s", (intmax_t)pos, sig->extension);
    new = fopen(fname, "w");
    
    cpid = fork();
    
    if (cpid != 0) {
        if (!quiet) {
            printf("%s found at position %jd. Extracting to %s...\n", sig->extension + 1, (intmax_t)pos, fname);
            fflush(stdout); 
        }

        fread(buffer, 1, 1, img);
        return;
    }
    
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
        fprintf(log, "%02x", digest[i]);
    fprintf(log, ",%s\n", fname);
    
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


struct match_s * match_header(char *buffer, int buflen, struct signatures_s *sigs) {
    int i, j, found;
    struct signature_s *sig = sigs->first;
    struct match_s *match;

    /* 
     *Only examine headers that start in the first (pagesize - margin) bytes.
     * The margin allows us to catch headers that may span two pages.
     * Headers that start in the margin will be found in the next block when
     * match_header is called again.
     */ 
    if (buflen > pagesize - margin)
        buflen = pagesize - margin;

    for (i=0; i < buflen; i++) {

        while (sig != NULL) {
            found = 1;
            
            for (j=0; j < strlen(sig->header); j++) {
                if (buffer[i+j] != sig->header[j]) {
                    found = 0;
                    break;
                }
            }

            if (found) {
                match = (struct match_s *) malloc(sizeof(struct match_s));
                match->offset = i;
                match->sig = sig;
                return match;
            }

            sig = sig->next;
        }

        sig = sigs->first;
    }

    return NULL;
}


int parser_parse(FILE *img, struct signatures_s *sigs, FILE *logfile) {
    char buf[pagesize];
    int i, read;
    struct signature_s *sig = sigs->first;
    struct match_s *match;
    
    while ((read = fread(buf, 1, pagesize, img)) != 0) {
        
        if ((match = match_header(buf, read, sigs)) != NULL) {
            fseeko(img, match->offset - read, SEEK_CUR);
            extract_to_footer(img, match->sig, logfile);
            free(match);
        }
        else if (read > pagesize - margin) {
            fseeko(img, -margin, SEEK_CUR);
        }
    }

    return 0;
}
