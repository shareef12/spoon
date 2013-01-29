#include <stdio.h>

// Bug: Does not catch headers that span between 1024 byte blocks
// Headers are currently hardcoded

int parser_parse(FILE *img) {
	char byte[1040];
	int i;	

	while (fread(&byte, sizeof(char), 1024, img) != 0) {
		for (i=0; i<1024; i++) {
			switch (byte[i]) {
				case '\x69':
					if (strncmp("\x69\x68\x67", &byte[i], 3) == 0)
						puts("Found 69!");
					break;
				case '\x42':
					if (strncmp("\x42\x41\x40", &byte[i], 3) == 0)
						puts("Found 42!");
					break;
				default:
					continue;
			}
		}
	}
}

int main() {
	FILE *fp;

	fp = fopen("test.img", "rb");
	parser_parse(fp);
	fclose(fp);
}	
