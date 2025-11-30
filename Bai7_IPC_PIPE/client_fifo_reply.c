// client_fifo_reply.c
// Client gửi request đến server và nhận response qua FIFO riêng.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SERVER_FIFO "/tmp/server_fifo"

// Cấu trúc request phải giống bên server
typedef struct {
    pid_t pid;              
    char client_fifo[64];
    char message[128];
} request_t;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s \"message\"\n", argv[0]);
        exit(1);
    }

    pid_t pid = getpid();      // Lấy PID để đặt tên FIFO riêng
    char client_fifo[64];
    sprintf(client_fifo, "/tmp/client_fifo_%d", pid);

    // Tạo FIFO riêng của client
    mkfifo(client_fifo, 0666);

    // Tạo request gửi đến server
    request_t req;
    req.pid = pid;
    strcpy(req.client_fifo, client_fifo);
    strncpy(req.message, argv[1], sizeof(req.message) - 1);

    // Gửi request đến SERVER_FIFO
    int server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd < 0) {
        perror("open SERVER_FIFO");
        unlink(client_fifo);
        exit(1);
    }

    write(server_fd, &req, sizeof(req));
    close(server_fd);

    // Mở FIFO riêng của client để nhận response từ server
    int client_fd = open(client_fifo, O_RDONLY);
    if (client_fd < 0) {
        perror("open client_fifo");
        unlink(client_fifo);
        exit(1);
    }

    char response[128];
    read(client_fd, response, sizeof(response));

    printf("CLIENT PID %d nhận phản hồi từ server: %s\n", pid, response);

    close(client_fd);
    unlink(client_fifo);   // Xóa FIFO client

    return 0;
}
