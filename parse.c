#include <stdio.h>
#include "signatures.c"

void extract_to_footer(FILE *img, struct signature_s *ftype) {
	char *outfile = (char *) malloc(16);
	char byte;
	int i, inside = 1, pos = ftell(img);
	FILE *out;

	sprintf(outfile, "%d%s", pos, ftype->extension);
	out = fopen(outfile, "w");

	printf("%s found at position %d. Extracting to %s...\n", ftype->extension, pos, outfile);
	while (inside) {
		inside = 0;
		for (i=0; i<strlen(ftype->footer); i++) {
			if (fread(&byte, 1, 1, img) == 0) {
				puts("  --EOF reached, no file footer found");
				break;
			}
			fwrite(&byte, 1, 1, out);
			if (byte != ftype->footer[i]) {
				inside = 1;
				break;
			}
		}
	}
	fseek(img, pos+1, SEEK_SET);
}

int parser_parse(FILE *img, struct signatures_s *sigs) {
	char byte;
	int i, pos, match;
	struct signature_s *sig;

	while (fread(&byte, 1, 1, img) != 0) {
		pos = ftell(img)-1;
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

			fseek(img, pos, SEEK_SET);

			if (match) {
				extract_to_footer(img, sig);
				break;
			}
			
			fread(&byte, 1, 1, img);
			sig = sig->next;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *fp;
	struct signatures_s *sigs = retrieve_signatures();
	
	fp = fopen(argv[1], "rb");
	parser_parse(fp, sigs);
	fclose(fp);
}	
