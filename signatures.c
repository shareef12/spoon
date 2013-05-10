#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "signatures.h"
#include "debug.c"

char * atoh(char *in) {
    // Decode a hex-encoded string
    // ex) "\\x42\\x41" => "BA"
    
    int i, tmp, len = strlen(in) / 4;
    char *out = (char *) malloc(len + 1);
    memset(out, 0, len + 1);

    while (*in != '\x00') {
        in += 2;
        tmp = 0;

        for (i=0; i<2; i++) {
            tmp *= 16;

            switch (*in) {
                case '1': tmp += 1; break;
                case '2': tmp += 2; break;
                case '3': tmp += 3; break;
                case '4': tmp += 4; break;
                case '5': tmp += 5; break;
                case '6': tmp += 6; break;
                case '7': tmp += 7; break;
                case '8': tmp += 8; break;
                case '9': tmp += 9; break;
                case 'a': tmp += 10; break;
                case 'b': tmp += 11; break;
                case 'c': tmp += 12; break;
                case 'd': tmp += 13; break;
                case 'e': tmp += 14; break;
                case 'f': tmp += 15; break;
            }

            in++;
        }

        *out = (char)tmp;
        out++;
    }
    
    out -= len;
    return out;
}

struct signature_s * make_signature(char *header, char *footer, char *extension, int length) {

    struct signature_s *sig;
    
    sig = (struct signature_s *) malloc(sizeof(struct signature_s));
    
    sig->header = header;
    sig->footer = footer;
    sig->extension = extension;
    sig->length = length;

    sig->next = NULL;
    sig->prev = NULL;
    
    return sig;
}

void append_signature(struct signatures_s *sigs, struct signature_s *new_sig) {

    if (sigs->first == NULL) {
        sigs->first = new_sig;
        sigs->last = new_sig;
    }
    else {
        new_sig->prev = sigs->last;
        sigs->last->next = new_sig;
        sigs->last = new_sig;
    }
}

struct signatures_s * retrieve_signatures() {
    // Read signatures from spoon.conf, and add them to a linked list.

    FILE *config;
    char *header, *footer, *ext;
    int len;
    struct signatures_s *sigs = (struct signatures_s *) malloc(sizeof(struct signatures_s));
    
    config = fopen("spoon.conf", "r");

    while (fscanf(config, "%ms %ms %ms %d", &ext, &header, &footer, &len) != EOF) {
        append_signature(sigs, make_signature(atoh(header), atoh(footer), ext, len));
    }

    fclose(config);    
    return sigs;
}
