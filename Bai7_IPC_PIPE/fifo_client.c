// fifo_client.c
// Client gửi dữ liệu đến server thông qua FIFO /tmp/my_server_fifo

#include <stdio.h>      // printf, perror
#include <stdlib.h>     // EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>     // write(), close()
#include <fcntl.h>      // open()
#include <string.h>     // strlen()

#define SERVER_FIFO "/tmp/my_server_fifo"

int main(int argc, char *argv[]) {

    // Kiểm tra có message để gửi không
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Mở FIFO ở chế độ ghi
    // open() sẽ block cho đến khi server mở FIFO để đọc
    int fd = open(SERVER_FIFO, O_WRONLY);
    if (fd == -1) {
        perror("open server fifo");
        return EXIT_FAILURE;
    }

    // Gửi message sang server
    write(fd, argv[1], strlen(argv[1]));

    close(fd);  // Đóng FIFO sau khi ghi xong

    return EXIT_SUCCESS;
}
