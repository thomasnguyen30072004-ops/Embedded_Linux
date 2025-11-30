// fifo_server.c
// Server sử dụng FIFO nhận dữ liệu từ client
// FIFO được tạo tại /tmp/my_server_fifo

#include <stdio.h>      // printf, perror
#include <stdlib.h>     // EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>     // read(), write(), close()
#include <fcntl.h>      // open()
#include <sys/stat.h>   // mkfifo()
#include <string.h>     // strlen()

#define SERVER_FIFO "/tmp/my_server_fifo"   // Đường dẫn FIFO

int main(void) {
    char buf[256];      // Buffer lưu dữ liệu nhận từ client

    // Tạo FIFO, nếu đã tồn tại thì bỏ qua
    if (mkfifo(SERVER_FIFO, 0666) == -1) {
        // mkfifo trả lỗi nếu FIFO đã tồn tại
        // Không sao → tiếp tục chạy
    }

    printf("Server: đang chờ dữ liệu từ client...\n");

    // Mở FIFO ở chế độ đọc
    // Server sẽ block tại open() CHO ĐẾN KHI có client mở FIFO ghi
    int fd = open(SERVER_FIFO, O_RDONLY);
    if (fd == -1) {
        perror("open server fifo");
        return EXIT_FAILURE;
    }

    ssize_t n;

    // Vòng lặp đọc dữ liệu từ FIFO
    // read() trả 0 nghĩa là phía ghi đã đóng → EOF
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';          // Thêm null-terminate để in chuỗi
        printf("Server nhận: %s\n", buf);
    }

    close(fd);                  // Đóng FIFO sau khi đọc
    unlink(SERVER_FIFO);        // Xóa FIFO khỏi filesystem

    return EXIT_SUCCESS;
}
