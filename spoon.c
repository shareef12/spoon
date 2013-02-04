#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "spoon.h"
#include "arg_parse.c"
#include "parse.c"

void main(int argc, char* argv[]) {                                                                                                                                                                        
    struct arguments* args = parse_args(argc, argv);
    struct signatures_s *sigs = retrieve_signatures();
    FILE *fp;
    char *cmd;

    printf("Running default forensics on: %s\n", args->inPath);

    
    if ((fp = fopen(args->inPath, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (mkdir("results", 0777) == -1) {
        perror("Error creating output directory");
        exit(EXIT_FAILURE);
    }

    asprintf(&cmd, "md5sum %s", args->inPath);
    //system(cmd);

    chdir("results");
    parser_parse(fp, sigs);
    fclose(fp);
}
