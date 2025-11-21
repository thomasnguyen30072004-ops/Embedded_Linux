/* unix_dgram_client.c
   Unix domain datagram client: bind to temporary path, sendto server path, recvfrom.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/unix_dgram_server.sock"
#define CLIENT_PATH_TEMPLATE "/tmp/unix_dgram_client_%d.sock"
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_un cli_addr, serv_addr;
    char buf[BUF_SIZE];
    ssize_t n;
    const char *msg = "Hello from unix dgram client";
    char client_path[108];

    if (argc > 1) msg = argv[1];

    snprintf(client_path, sizeof(client_path), CLIENT_PATH_TEMPLATE, getpid());
    unlink(client_path);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sun_family = AF_UNIX;
    strncpy(cli_addr.sun_path, client_path, sizeof(cli_addr.sun_path)-1);

    if (bind(sockfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // prepare server addr
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strncpy(serv_addr.sun_path, SERVER_PATH, sizeof(serv_addr.sun_path)-1);

    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("sendto");
    }

    socklen_t serv_len = sizeof(serv_addr);
    n = recvfrom(sockfd, buf, BUF_SIZE-1, 0, (struct sockaddr*)&serv_addr, &serv_len);
    if (n < 0) perror("recvfrom");
    else {
        buf[n] = '\0';
        printf("Received ack: %s\n", buf);
    }

    close(sockfd);
    unlink(client_path);
    return 0;
}
