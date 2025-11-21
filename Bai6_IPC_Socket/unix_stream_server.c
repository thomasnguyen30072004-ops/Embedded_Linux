/* unix_stream_server.c
   Unix domain (AF_UNIX) stream server (echo).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>    // sockaddr_un
#include <errno.h>
#include <sys/stat.h>

#define SOCKET_PATH "/tmp/unix_stream.sock"
#define BACKLOG 5
#define BUF_SIZE 1024

int main(void) {
    int listen_fd = -1, conn_fd = -1;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    ssize_t n;

    // remove file socket nếu còn tồn tại
    unlink(SOCKET_PATH);

    // tạo socket AF_UNIX stream
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    // bind vào path
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // set quyền file nếu cần (ví dụ 0666)
    chmod(SOCKET_PATH, 0666);

    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("Unix stream server listening at %s\n", SOCKET_PATH);

    conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd < 0) { perror("accept"); close(listen_fd); exit(EXIT_FAILURE); }

    while ((n = read(conn_fd, buf, BUF_SIZE)) > 0) {
        printf("Received (%zd bytes): %.*s\n", n, (int)n, buf);
        if (write(conn_fd, buf, n) < 0) { perror("write"); break; }
    }
    if (n < 0) perror("read");

    close(conn_fd);
    close(listen_fd);
    unlink(SOCKET_PATH); // xóa file socket khi thoát
    return 0;
}
