

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <arpa/inet.h>      // sockaddr_in, inet_ntop, htons
#include <sys/socket.h>     // socket(), bind(), listen(), accept()
#include <netinet/in.h>     // struct sockaddr_in
#include <errno.h>

#define PORT 12345
#define BACKLOG 5
#define BUF_SIZE 1024

int main(void) {
    int listen_fd = -1, conn_fd = -1;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    char buf[BUF_SIZE];
    ssize_t n;

    // 1) tạo socket (AF_INET, stream -> TCP)
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2) cho phép tái sử dụng địa chỉ nhanh (avoid "Address already in use")
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        // not fatal, continue
    }

    // 3) chuẩn bị sockaddr_in cho server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;           // IPv4
    serv_addr.sin_port = htons(PORT);         // port (network byte order)
    serv_addr.sin_addr.s_addr = INADDR_ANY;   // bind tất cả interface

    // 4) bind socket vào địa chỉ
    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // 5) listen (chuyển sang trạng thái server lắng nghe)
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("IPv4 stream server listening on port %d\n", PORT);

    // 6) accept vòng lặp (1 client tại một time đơn giản)
    cli_len = sizeof(cli_addr);
    conn_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);
    if (conn_fd < 0) {
        perror("accept");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // In IP client
    char cli_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, sizeof(cli_ip));
    printf("Accepted connection from %s:%d\n", cli_ip, ntohs(cli_addr.sin_port));

    // 7) đọc dữ liệu từ client và echo lại
    while ((n = read(conn_fd, buf, BUF_SIZE)) > 0) {
        // ghi ra màn hình server
        printf("Received (%zd bytes): %.*s\n", n, (int)n, buf);

        // echo trở lại client
        ssize_t sent = 0;
        while (sent < n) {
            ssize_t w = write(conn_fd, buf + sent, n - sent);
            if (w < 0) {
                perror("write");
                break;
            }
            sent += w;
        }
    }

    if (n < 0) perror("read");

    // 8) đóng kết nối
    close(conn_fd);
    close(listen_fd);
    printf("Server exiting\n");
    return 0;
}
