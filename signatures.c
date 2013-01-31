#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "signatures.h"

struct signature_s * make_signature(char *header, char *footer, char *extension, int length) {
	struct signature_s *sig;
	
	sig = (struct signature_s *) malloc(sizeof(struct signature_s));

	sig->header = (char *) malloc(strlen(header)+1);
	strncpy(sig->header, header, strlen(header)+1);
	sig->footer = (char *) malloc(strlen(footer)+1);
	strncpy(sig->footer, footer, strlen(footer)+1);
	sig->extension = (char *) malloc(strlen(extension)+1);
	strncpy(sig->extension, extension, strlen(extension)+1);

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
	char *header, *footer, *ext, *len;
	struct signatures_s *sigs = (struct signatures_s *) malloc(sizeof(struct signatures_s));
	
	/*
	* fp = fopen("test.conf", "r");
	* fscanf(fp, "%s\t%s\t%s\t%s", header, footer, ext, len);
	* fclose(fp);
	* printf("header: %s\nfooter: %s\next: %s\nlen: %s\n", header, footer, ext, len);
	*/
	
	append_signature(sigs, make_signature("\xff\xd8\xff\xe0", "\xff\xd9", ".jpg", 4000));
	return sigs;
}
