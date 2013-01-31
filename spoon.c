#include "arg_parse.c"
#include "parse.c"
#include <stdio.h>

void main(int argc, char* argv[]) {                                                                                                                                                                        
    struct arguments* args = parse_args(argc, argv);
    printf("Running default forensics on: %s\n", args->inPath);
    FILE *fp;
    struct signatures_s *sigs = retrieve_signatures();
    fp = fopen(args->inPath, "rb");
    parser_parse(fp, sigs);
    fclose(fp);
}
