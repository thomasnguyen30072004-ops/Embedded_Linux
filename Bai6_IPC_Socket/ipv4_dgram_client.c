
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12346
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    socklen_t serv_len;
    const char *msg = "Hello from UDP client";

    if (argc > 1) msg = argv[1];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // gửi
    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // chờ ACK
    serv_len = sizeof(serv_addr);
    ssize_t n = recvfrom(sockfd, buf, BUF_SIZE-1, 0, (struct sockaddr*)&serv_addr, &serv_len);
    if (n < 0) perror("recvfrom");
    else {
        buf[n] = '\0';
        printf("Received from server: %s\n", buf);
    }

    close(sockfd);
    return 0;
}
