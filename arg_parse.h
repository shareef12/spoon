/* structures */
struct arguments {
    char *inPath;
};


/* preprocessors */
void usage_error(char *argv);  
struct arguments* parse_args(int argc, char *argv[]);
