/* unix_stream_client.c
   Unix domain stream client: connect to SOCKET_PATH, send message, receive echo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unix_stream.sock"
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    const char *msg = "Hello from unix stream client";

    if (argc > 1) msg = argv[1];

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (write(sockfd, msg, strlen(msg)) < 0) perror("write");

    ssize_t n = read(sockfd, buf, BUF_SIZE-1);
    if (n < 0) perror("read");
    else {
        buf[n] = '\0';
        printf("Received echo: %s\n", buf);
    }

    close(sockfd);
    return 0;
}
