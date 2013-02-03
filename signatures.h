struct signature_s {
    char *header;
    char *footer;
    char *extension;
    int length;
    struct signature_s *next;
    struct signature_s *prev;
};

struct signatures_s {
    struct signature_s *first;
    struct signature_s *last;
};

char * atoh(char *in);

struct signature_s * make_signature(char *header, char *footer, char *extension, int length);

void append_signature(struct signatures_s *sigs, struct signature_s *new_sig);

struct signatures_s * retrieve_signatures();
