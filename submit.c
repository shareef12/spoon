#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

void submit(FILE *in, FILE *out, int sock) {
    char *hash, *fname, buffer[256], *line = NULL;
    int i, n=256;

    for (i=0; i<5; i++) {
        getline(&line, &n, in);
    }

    while (fscanf(in, "%32ms,%ms\n", &hash, &fname) != EOF) {
        send(sock, hash, strlen(hash), 0);
        send(sock, "\n", 1, 0);
        recv(sock, buffer, 255, 0);
        fprintf(out, "%s", buffer);
    }
}

int send_hashes() {
    char buffer[256];
    int sockfd;
    struct sockaddr_in dst_addr;
    struct hostent *dst;
    FILE *in, *out;

    in = fopen("files.log", "r");
    out = fopen("hashCheck.log", "w");

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;

    if ((dst = gethostbyname("hash.cymru.com")) == NULL)
        return 1;

    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(43);
    bcopy((char *)dst->h_addr, (char *)&dst_addr.sin_addr.s_addr, dst->h_length);
    memset(&(dst_addr.sin_zero), 0, 8);

    if (connect(sockfd, (struct sockaddr *)&dst_addr, sizeof(struct sockaddr)) < 0)
        return 1;

    
    send(sockfd, "begin\n", 6, 0);
    recv(sockfd, buffer, 255, 0);
    fprintf(out, "%s", buffer);

    submit(in, out, sockfd);  

    fprintf(out, "\n");
    fclose(in);
    fclose(out);
    close(sockfd);
    return 0;
}
