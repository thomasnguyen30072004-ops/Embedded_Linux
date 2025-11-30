// server_fifo_reply.c
// Server nhận request từ clients và gửi response lại thông qua FIFO riêng của từng client.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SERVER_FIFO "/tmp/server_fifo"   // FIFO chính để nhận request

// Cấu trúc request gửi từ client
typedef struct {
    pid_t pid;                 // PID client
    char client_fifo[64];      // Đường dẫn FIFO riêng của client
    char message[128];         // Nội dung message
} request_t;

int main() {
    request_t req;             // Biến chứa request từ client

    // Tạo FIFO server (nếu đã tồn tại thì bỏ qua lỗi)
    mkfifo(SERVER_FIFO, 0666);

    printf("SERVER đang chạy...\nChờ client gửi request...\n");

    // Mở FIFO server ở chế độ chỉ đọc (blocking)
    int server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd < 0) {
        perror("open SERVER_FIFO");
        exit(1);
    }

    // Server chạy liên tục
    while (1) {
        // Đọc request từ client
        ssize_t n = read(server_fd, &req, sizeof(req));
        if (n <= 0) {
            continue;   // Nếu không có dữ liệu thì đọc tiếp
        }

        printf("SERVER nhận từ Client PID %d: %s\n",
               req.pid, req.message);

        // Mở FIFO riêng của client để gửi response
        int client_fd = open(req.client_fifo, O_WRONLY);
        if (client_fd < 0) {
            perror("open client FIFO");
            continue;
        }

        // Tạo nội dung phản hồi
        char response[128];
        snprintf(response, sizeof(response),
                 "SERVER đã nhận tin nhắn: \"%s\"", req.message);

        // Gửi phản hồi
        write(client_fd, response, strlen(response) + 1);

        close(client_fd);   // Đóng FIFO client
    }

    close(server_fd);
    unlink(SERVER_FIFO);   // Xóa FIFO nếu server tắt
    return 0;
}
