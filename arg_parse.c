#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "arg_parse.h"

void usage_error(char *argv) {
    fprintf(stderr, "Usage: %s [-f filepath]\n",argv);
    exit(EXIT_FAILURE);
}

struct arguments* parse_args(int argc, char *argv[]) {
    int opt;
    struct arguments *args = malloc(sizeof(struct arguments));

    memset(args,0,sizeof(struct arguments));

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                args->inPath = optarg;
                break;
            default: 
                usage_error(argv[0]);
        }
    }

    if(!args->inPath) usage_error(argv[0]);

    return args;
}


