#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <openssl/evp.h>

#include "spoon.h"
#include "signatures.c"
#include "parse.h"

void extract_to_footer(FILE *img, struct signature_s *sig, FILE *log) {
    char buffer[pagesize], outfile[32];
    int i, read, offset, count = 0;
    off_t pos = ftello(img);
    FILE *out;
    EVP_MD_CTX mdctx;
    unsigned char digest[EVP_MAX_MD_SIZE];

    // Initialize MD5 context
    EVP_DigestInit(&mdctx, EVP_md5());

    // Create destination file
    sprintf(outfile, "%jd%s", (intmax_t)pos, sig->extension);
    out = fopen(outfile, "w");

    if (!quiet) {
        printf("%s found at position %jd. Extracting to %s...\n", sig->extension + 1, (intmax_t)pos, outfile);
        fflush(stdout); 
    }
    
    while ((read = fread(buffer, 1, pagesize, img)) != 0) {
        
        // Reached max file length defined in spoon.conf
        if (count > sig->length)
            break;

        // Haven't found a footer yet
        else if ( (offset = match_sequence( buffer, read, sig->footer )) == NO_MATCH) {
            fwrite(&buffer, 1, pagesize - margin, out);
            count += pagesize - margin;
            
            EVP_DigestUpdate(&mdctx, &buffer, pagesize - margin);

            if (read > pagesize - margin)
                fseeko(img, -margin, SEEK_CUR);
        }

        // Found a footer
        else {
            fwrite(&buffer, 1, offset + strlen(sig->footer), out);
            EVP_DigestUpdate( &mdctx, &buffer, offset + strlen(sig->footer) );
            
            // account for optional comment at the end of a zipfile
            if ( strcmp( sig->extension, ".zip" ) == 0) {
                short zipCommentLen = 0;
                memcpy( &zipCommentLen, &buffer[offset + 20], 2);
                fwrite( &buffer[offset + 4], 1, 18 + zipCommentLen, out);
                EVP_DigestUpdate(&mdctx, &buffer[offset+4], 18 + zipCommentLen);

                fseeko(img, offset + 22 + zipCommentLen - read, SEEK_CUR);
            } 
            break;
        }
    }
    
    if (strcmp(sig->extension, ".zip") != 0)
        fseeko(img, pos+1, SEEK_SET);

    // Complete MD5 of extracted file
    EVP_DigestFinal_ex(&mdctx, digest, NULL);
    EVP_MD_CTX_cleanup(&mdctx);

    // Write hash and filename to files.log
    for (i=0; i<16; i++)
        fprintf(log, "%02x", digest[i]);
    fprintf(log, ",%s\n", outfile);
    
    fclose(out);
}


int match_sequence(char *buffer, int buflen, char *sequence) {
    int i, j, found;
  
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
