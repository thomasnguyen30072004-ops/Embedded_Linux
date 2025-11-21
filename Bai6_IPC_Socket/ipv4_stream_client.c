#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // inet_pton
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd = -1;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    ssize_t n;

    const char *msg = "Hello from IPv4 stream client!\n";
    if (argc > 1) msg = argv[1]; // allow custom message via argv

    // 1) tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2) chuẩn bị địa chỉ server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 3) kết nối tới server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 4) gửi message
    if (write(sockfd, msg, strlen(msg)) < 0) {
        perror("write");
    }

    // 5) đọc echo trả lại từ server
    n = read(sockfd, buf, BUF_SIZE-1);
    if (n < 0) {
        perror("read");
    } else if (n == 0) {
        printf("Server closed connection\n");
    } else {
        buf[n] = '\0';
        printf("Received from server: %s", buf);
    }

    // 6) đóng socket
    close(sockfd);
    return 0;
}
