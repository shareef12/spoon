#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

struct arguments {
        char* inPath;
            };

void usage_error(char* argv) {
    fprintf(stderr, "Usage: %s [-f filepath]\n",argv);
    exit(EXIT_FAILURE);
}

struct arguments* parse_args(int argc, char* argv[]) {
int opt;
struct arguments args = {0};
    while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
			case 'f':
                args.inPath = optarg;
			    break;
            default: 
                usage_error(argv[0]);
		}
	}
    if(!args.inPath) usage_error(argv[0]);

    return &args;
}

void main(int argc, char* argv[]) {
    struct arguments* args = parse_args(argc, argv);
    printf("Running default forensics on: %s\n", args->inPath);
}
