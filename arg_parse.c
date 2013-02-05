#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "spoon.h"
#include "arg_parse.h"

char *path = NULL;
int pagesize = 16384;
int margin = 2048;
int search_embedded = 0;

void usage_error(char *argv) {
    fprintf(stderr, "Usage: %s [-p pagesize] [-m margin] <-f file>\n",argv);
    exit(EXIT_FAILURE);
}

void parse_args(int argc, char *argv[]) {
    int opt;
    
    while ((opt = getopt(argc, argv, "f:p:m:")) != -1) {
        switch (opt) {
            case 'f':
                path = optarg;
                break;
            case 'p':
                pagesize = atoi(optarg);
                break;
            case 'm':
                margin = atoi(optarg);
                break;
            default: 
                usage_error(argv[0]);
        }
    }

    if (path == NULL)
        usage_error(argv[0]);
}
