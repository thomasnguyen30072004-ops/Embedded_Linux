#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>    // sockaddr_in, ntohs
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12346
#define BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    char buf[BUF_SIZE];
    ssize_t n;

    // 1) tạo UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    // 2) bind vào port
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d\n", PORT);

    // 3) recvfrom vòng lặp
    cli_len = sizeof(cli_addr);
    n = recvfrom(sockfd, buf, BUF_SIZE-1, 0, (struct sockaddr*)&cli_addr, &cli_len);
    if (n < 0) { perror("recvfrom"); close(sockfd); exit(EXIT_FAILURE); }
    buf[n] = '\0';

    char cli_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, sizeof(cli_ip));
    printf("Received %zd bytes from %s:%d: %s\n", n, cli_ip, ntohs(cli_addr.sin_port), buf);

    // 4) trả lại ACK
    const char *ack = "ACK from UDP server";
    if (sendto(sockfd, ack, strlen(ack), 0, (struct sockaddr*)&cli_addr, cli_len) < 0) {
        perror("sendto");
    }

    close(sockfd);
    return 0;
}
