#define NO_MATCH INT_MAX

struct match_s {
    int offset;
    struct signature_s *sig;
};

void extract_to_footer(FILE *img, size_t base, struct signature_s *sig, FILE *log);

int match_sequence(char *buffer, int buflen, char *sequence);

int parser_parse(FILE *img, struct signatures_s *sigs, FILE *logfile);
