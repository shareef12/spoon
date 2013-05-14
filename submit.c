#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

void submit_hashes(FILE *in, FILE *out, int sock) {
    // Iterate through files.log and submit all hashes
    // for extracted files
    char *hash, *fname, buffer[256], *line = NULL;
    int i;
    size_t n = 256;

    memset(buffer, 0, 256);
    for (i=0; i<5; i++) {
        getline(&line, &n, in);
    }

    while (fscanf(in, "%32ms,%ms\n", &hash, &fname) != EOF) {
        printf("Sending hash: %s\n", hash);
        
        send(sock, hash, strlen(hash), 0);
        send(sock, "\n", 1, 0);
        recv(sock, buffer, 255, 0);

        fflush(out);
        if (!quiet) {        
            fprintf(out, "%s", buffer);
            fflush(stdout);
        }
    }
}

int send_hashes() {
    // Send hashes of all extracted files to
    // hash.cymru.com to check against known malware
    char buffer[256];
    int sockfd;
    struct sockaddr_in dst_addr;
    struct hostent *dst;
    FILE *in, *out;

    in = fopen("files.log", "r");
    out = fopen("hashCheck.log", "w");

    // Connect to hash.cymru.com on port 43
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;

    if ((dst = gethostbyname("hash.cymru.com")) == NULL)
        return 1;

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(43);
    bcopy( (char *)dst->h_addr,
           (char *)&dst_addr.sin_addr.s_addr,
           dst->h_length
         );
    memset( &(dst_addr.sin_zero), 0, 8);

    if (connect(sockfd, (struct sockaddr *)&dst_addr, sizeof(struct sockaddr)) < 0)
        return 1;

    // Begin sending data
    send(sockfd, "begin\n", 6, 0);
    recv(sockfd, buffer, 255, 0);
    fprintf(out, "%s", buffer);

    submit_hashes(in, out, sockfd);

    send(sockfd, "end\n", 4, 0);

    fclose(in);
    fclose(out);
    close(sockfd);
    return 0;
}
