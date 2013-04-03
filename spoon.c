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

    if (mkdir(out, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Error creating output directory");
            exit(EXIT_FAILURE);
        }
    }

    chdir(out);
    system("rm -rf spoonResults");
    mkdir("spoonResults", 0777);
    chdir("spoonResults");
    logfile = fopen("files.log", "w");
    mkdir("extracted", 0777);
    chdir("extracted");

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

    chdir("../");
    puts("Sending hashes to hash.cymru.com...");
    send_hashes();
    printf("Extracted files located in %s/spoonResults/\n", out);
}
