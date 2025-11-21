/* unix_dgram_server.c
   Unix domain datagram server: recvfrom, print, sendto ack.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/unix_dgram_server.sock"
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_un serv_addr, cli_addr;
    socklen_t cli_len;
    char buf[BUF_SIZE];
    ssize_t n;

    unlink(SERVER_PATH);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SERVER_PATH, sizeof(serv_addr.sun_path)-1);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Unix datagram server at %s\n", SERVER_PATH);

    cli_len = sizeof(cli_addr);
    n = recvfrom(sockfd, buf, BUF_SIZE-1, 0, (struct sockaddr*)&cli_addr, &cli_len);
    if (n < 0) { perror("recvfrom"); close(sockfd); exit(EXIT_FAILURE); }

    buf[n] = '\0';
    printf("Received: %s (from path: %s)\n", buf, cli_addr.sun_path);

    const char *ack = "ACK from unix dgram server";
    if (sendto(sockfd, ack, strlen(ack), 0, (struct sockaddr*)&cli_addr, cli_len) < 0) {
        perror("sendto");
    }

    close(sockfd);
    unlink(SERVER_PATH);
    return 0;
}
