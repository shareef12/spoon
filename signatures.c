#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "signatures.h"
#include "debug.c"

char * atoh(char *in) {
    int i, j, tmp, res=0, len=strlen(in)/4;
    char *out = (char *) malloc(len + 1);

    for (i=0; i<len*4; i+=4) {
        for (j=2; j<4; j++) {
            if (in[i+j] >= '0' && in[i+j] <= '9')
                tmp = in[i+j] - '0';
            else if (in[i+j] >= 'a' && in[i+j] <= 'f')
                tmp = in[i+j] - 'a' + 10;
            else if (in[i+j] >= 'A' && in[i+j] <= 'F')
                tmp = in[i+j] - 'A' + 10;
            else
                return NULL;
            
            res = res*16 + tmp;
        }
        out[i/4] = res;
        res = 0;
    }
    out[len+1] = '\x00';
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
    FILE *fp;
    char *header, *footer, *ext;
    int len;
    struct signatures_s *sigs = (struct signatures_s *) malloc(sizeof(struct signatures_s));
    
    fp = fopen("spoon.conf", "r");
    while (fscanf(fp, "%ms %ms %ms %d", &ext, &header, &footer, &len) != EOF) {
        append_signature(sigs, make_signature(atoh(header), atoh(footer), ext, len));
    }
    fclose(fp);
    
    return sigs;
}
