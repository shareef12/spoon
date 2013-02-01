#include "arg_parse.c"
#include "parse.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void main(int argc, char* argv[]) {                                                                                                                                                                        
    struct arguments* args = parse_args(argc, argv);
    struct signatures_s *sigs = retrieve_signatures();
    FILE *fp;

    printf("Running default forensics on: %s\n", args->inPath);

    fp = fopen(args->inPath, "rb");
    parser_parse(fp, sigs);
    fclose(fp);
}
