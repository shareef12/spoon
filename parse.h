#define NO_MATCH INT_MAX

struct match_s {
    int offset;
    struct signature_s *sig;
};

void extract_to_footer(FILE *img, struct signature_s *sig);

int match_sequence(char *buffer, int buflen, char *sequence);

struct match_s * match_header(char *buffer, int buflen, struct signatures_s *sigs);

int parser_parse(FILE *img, struct signatures_s *sigs);
