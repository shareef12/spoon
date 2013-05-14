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

    puts("");
    puts(" ==============================");
    puts("|          SPOON v3.0          |");
    puts("|                              |");
    puts("|    CDTs Allison, Sharpsten   |");
    puts(" ==============================");
    puts("");
    fflush(stdout);

    
    if ((fp = fopen(path, "rb")) == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (chdir(out) < 0) {
        perror("Error changing to outpur directory");
        exit(EXIT_FAILURE);
    }
    system("rm -rf spoonResults");
    if (mkdir("spoonResults", 0777) < 0) {
        perror("Cannot create output directory");
        exit(EXIT_FAILURE);
    }
    chdir("spoonResults");
    
    logfile = fopen("files.log", "w");
    mkdir("extracted", 0777);
    chdir("extracted");

    time_t tm;
    char timeMsg[64];
    time(&tm);
    strftime(timeMsg, 64, "Ran at: %H:%M:%S %d %B %Y\n", localtime(&tm));

    fprintf(logfile, "Spoon v3.0\n");
    fprintf(logfile, "%s\n", timeMsg);
    fprintf(logfile, "Extracted Files\n");
    fprintf(logfile, "===============\n");
    fflush(logfile);

    parser_parse(fp, sigs, logfile);
    fclose(fp);
    fclose(logfile);

    chdir("../");
    puts("");
    puts("Sending hashes to hash.cymru.com...");
    fflush(stdout);
    send_hashes();
    printf("Extracted files located in %s/spoonResults/\n", out);
}
