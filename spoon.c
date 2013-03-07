#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include "spoon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "arg_parse.c"
#include "parse.c"
#include "submit.c"

void main(int argc, char* argv[]) {
    struct signatures_s *sigs = retrieve_signatures();
    FILE *fp, *logfile;
    char *cmd;

    parse_args(argc, argv);

    printf("Running default forensics on: %s\n", path);

    
    if ((fp = fopen(path, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (mkdir("results", 0777) == -1) {
        perror("Error creating output directory");
        exit(EXIT_FAILURE);
    }

    chdir("results");
    logfile = fopen("files.log", "w");

    time_t tm;
    char timeMsg[64];
    time(&tm);
    strftime(timeMsg, 64, "Ran at: %H:%M:%S %d %B %Y\n", localtime(&tm));

    fprintf(logfile, "Spoon v2.0\n");
    fprintf(logfile, "%s\n", timeMsg);
    fprintf(logfile, "Extracted Files\n");
    fprintf(logfile, "===============\n");

    parser_parse(fp, sigs, logfile);
    fclose(fp);
    fclose(logfile);

    puts("Sending hashes to hash.cymru.com...");
    send_hashes();
    puts("Extracted files located in results/");
}
