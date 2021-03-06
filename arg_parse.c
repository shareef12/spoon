#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "spoon.h"
#include "arg_parse.h"

char *path = NULL;
char *out = ".";
int pagesize = 16384;
int margin = 2048;
int quiet = 0;
int pipefd = 0;

void usage_error(char *argv) {
    fprintf(stderr, "Usage: %s <-f file> [-o outputPath] [-p pagesize] [-m margin] [-q]\n",argv);
    exit(EXIT_FAILURE);
}

void parse_args(int argc, char *argv[]) {
    int opt;
    
    while ((opt = getopt(argc, argv, "f:o:p:m:g:q")) != -1) {
        switch (opt) {
            case 'f':
                path = optarg;
                break;
            case 'o':
                out = optarg;
                break;
            case 'p':
                pagesize = atoi(optarg);
                break;
            case 'm':
                margin = atoi(optarg);
                break;
            case 'g':
                pipefd = atoi(optarg);
                dup2(pipefd, 0);
                dup2(pipefd, 1);
                dup2(pipefd, 2);
                break;
            case 'q':
                quiet = 1;
                break;
            default: 
                usage_error(argv[0]);
        }
    }

    if (path == NULL)
        usage_error(argv[0]);
}
