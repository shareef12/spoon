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
