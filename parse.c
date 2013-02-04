#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "signatures.c"
#include "parse.h"

void extract_to_footer(FILE *img, struct signature_s *sig) {
    char buffer[pagesize], outfile[32];
    int read, offset, count = 0;
    off_t pos = ftello(img);
    FILE *out;

    sprintf(outfile, "%jd%s", (intmax_t)pos, sig->extension);
    out = fopen(outfile, "w");

    printf("%s found at position %jd. Extracting to %s...\n", sig->extension, (intmax_t)pos, outfile);
    
    while ((read = fread(buffer, 1, pagesize, img)) != 0) {
        
        if (count > sig->length)
            break;

        else if ((offset = match_sequence(buffer, read, sig->footer)) == NO_MATCH) {
            fwrite(&buffer, 1, pagesize - margin, out);
            count += pagesize - margin;
            fseeko(img, -margin, SEEK_CUR);
        }
        else {
            fwrite(&buffer, 1, offset + strlen(sig->footer), out);
            break;
        }
    }
    
    // Plarses for local file header, not beginning of zip
    if (strcmp(sig->extension, ".zip") == 0) {
        short zipCommentLen=0;
        memcpy(&zipCommentLen, &buffer[offset + 20], 2);
        fwrite(&buffer[offset + 4], 1, 18 + zipCommentLen, out);
    }
    
    fclose(out);
    fseeko(img, pos+1, SEEK_SET);
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


int parser_parse(FILE *img, struct signatures_s *sigs) {
    char buf[pagesize];
    int i, read;
    struct signature_s *sig = sigs->first;
    struct match_s *match;
    
    while ((read = fread(buf, 1, pagesize, img)) != 0) {
        
        if ((match = match_header(buf, read, sigs)) != NULL) {
            fseeko(img, match->offset - read, SEEK_CUR);
            extract_to_footer(img, match->sig);
        }
        else if (read > pagesize - margin) {
            fseeko(img, -margin, SEEK_CUR);
        }
    }

    return 0;
}
