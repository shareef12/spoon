#include <stdio.h>
#include "signatures.c"

void extract(int pos, struct signature_s *ftype) {
	printf("%s found at position %d\n", ftype->extension, pos);
	puts("extract() not yet implemented");
}

int parser_parse(FILE *img, struct signatures_s *sigs) {
	char byte;
	int i, pos, match;
	struct signature_s *sig;

	// look at fseek, ftell, fgetpos, and fsetpos

	while (fread(&byte, 1, 1, img) != 0) {
		// loop through all signatures
		pos = ftell(img);
		sig = sigs->first;
		while (sig != NULL) {
			match = 1;
			for (i=0; i<strlen(sig->header); i++) {
				if (byte == sig->header[i]) {
					fread(&byte, 1, 1, img);
					continue;
				}
				else {
					match = 0;
					break;
				}
			}
			if (match)
				extract(pos, sig);

			sig = sig->next;
			fseek(img, pos, SEEK_SET);
		}
	}
	return 0;
}

int main() {
	FILE *fp;
	struct signatures_s *sigs = retrieve_signatures();
	
	fp = fopen("test.img", "rb");
	parser_parse(fp, sigs);
	fclose(fp);
}	
