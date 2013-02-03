#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "signatures.c"
#include "parse.h"

void extract_to_footer(FILE *img, struct signature_s *sig) {
    char buffer[16384], outfile[16];
    int read, offset, pos = ftell(img);
    FILE *out;

    sprintf(outfile, "%d%s", pos, sig->extension);
    out = fopen(outfile, "w");

    printf("%s found at position %d. Extracting to %s...\n", sig->extension, pos, outfile);
    
    while ((read = fread(buffer, 1, 16384, img)) != 0) {
        
        if ((offset = match_sequence(buffer, read, sig->footer)) == 16384) {
            fwrite(&buffer, 1, 16384 - 2048, out);
            fseek(img, -2048, SEEK_CUR);
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
    fseek(img, pos+1, SEEK_SET);
}


int match_sequence(char *buffer, int buflen, char *sequence) {
    int i, j, found;
  
    if (buflen > 16384 - 2048)
        buflen = 16384 - 2048;
    
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

    return 16384;
}


struct match_s * match_header(char *buffer, int buflen, struct signatures_s *sigs) {
    int i, j, found;
    struct signature_s *sig = sigs->first;
    struct match_s *match;

    if (buflen > 16384 - 2048)
        buflen = 16384 - 2048;

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
    char buf[16384];
    int i, read;
    struct signature_s *sig = sigs->first;
    struct match_s *match;
    
    while ((read = fread(buf, 1, 16384, img)) != 0) {

        if ((match = match_header(buf, read, sigs)) != NULL) {
            fseek(img, match->offset - read, SEEK_CUR);
            extract_to_footer(img, match->sig);
        }
        else if (read > 16384 - 2048) {
            fseek(img, -2048, SEEK_CUR);
        }
    }

    return 0;
}
