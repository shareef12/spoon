#include "signatures.c"

void extract_to_footer(FILE *img, struct signature_s *sig) {
    char buffer[16384], *outfile = (char *) malloc(16);
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
    
    fseek(img, pos+1, SEEK_SET);
}


int match_sequence(char *buffer, int buflen, char *sequence) {
    int i, j, found, offset;
    
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


int parser_parse(FILE *img, struct signatures_s *sigs) {
    char buf[16384];
    int i, read, offset, match = 0;
    struct signature_s *sig = sigs->first;

    while ((read = fread(buf, 1, 16384, img)) != 0) {
        while (sig != NULL) {
            if ((offset = match_sequence(buf, read, sig->header)) != 16384) {
                match = 1;
                fseek(img, offset - read, SEEK_CUR);
                extract_to_footer(img, sig);
                break;
            }
            else {
                sig = sig-> next;
            }
        }
        if (!match)
            fseek(img, -2048, SEEK_CUR);

        sig = sigs->first;
    }

    return 0;
}
