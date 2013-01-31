void debug_sigs(struct signatures_s *sigs) {
    struct signature_s *sig = sigs->first;
    
    while (sig != NULL) {
        printf("%s\n", sig->extension);
        printf("header: %s\n", sig->header);
        printf("footer: %s\n", sig->footer);
        printf("length: %d\n", sig->length);
        printf("\n");
        sig = sig->next;
    }
}
