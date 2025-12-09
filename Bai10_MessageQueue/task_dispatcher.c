/* task_dispatcher.c
 * Dispatcher: gửi task vào POSIX message queue với priority do người dùng chỉ định.
 *
 * Usage: ./task_dispatcher <priority> "<task_description>"
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>     // mq_open, mq_send, mq_close
#include <errno.h>
#include <fcntl.h>      // O_* constants
#include <sys/stat.h>   // mode constants
#include <unistd.h>

const char *QUEUE_NAME = "/my_task_queue";

// Hàm in cách dùng chương trình
void print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s <priority> \"<task_description>\"\n", progname);
    fprintf(stderr, "Example: %s 30 \"CRITICAL: Restart web server\"\n", progname);
}

// Hàm mở queue (chỉ mở để ghi). Trả về descriptor hoặc (mqd_t)-1 khi lỗi.
mqd_t open_queue_for_send()
{
    // O_WRONLY: chỉ write (dispatcher)
    mqd_t d = mq_open(QUEUE_NAME, O_WRONLY);
    if (d == (mqd_t)-1) {
        perror("mq_open (dispatcher) failed");
        return (mqd_t)-1;
    }
    return d;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Parse priority (integer)
    char *endptr;
    long prio_long = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || prio_long < 0) {
        fprintf(stderr, "Priority phải là số nguyên không âm.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    unsigned int priority = (unsigned int) prio_long;

    // Lấy nội dung task (ghép tất cả phần còn lại thành 1 chuỗi nếu cần)
    // Nếu người chạy truyền chuỗi trong "", argv[2] đã đủ. Nhưng ta hỗ trợ nối nhiều arg.
    size_t total_len = 0;
    for (int i = 2; i < argc; ++i) total_len += strlen(argv[i]) + 1; // +1 cho space hoặc null
    char *msg = malloc(total_len);
    if (!msg) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    msg[0] = '\0';
    for (int i = 2; i < argc; ++i) {
        strcat(msg, argv[i]);
        if (i < argc - 1) strcat(msg, " ");
    }

    // Mở queue
    mqd_t mqdes = open_queue_for_send();
    if (mqdes == (mqd_t)-1) {
        fprintf(stderr, "Không thể mở queue '%s' để gửi. Hãy chắc worker đã tạo queue (worker chạy trước).\n", QUEUE_NAME);
        free(msg);
        return EXIT_FAILURE;
    }

    // Gửi message
    if (mq_send(mqdes, msg, strlen(msg), priority) == -1) {
        perror("mq_send failed");
        mq_close(mqdes);
        free(msg);
        return EXIT_FAILURE;
    }

    // Thông báo thành công
    printf("Sent task (Priority: %u): %s\n", priority, msg);

    // Đóng descriptor
    mq_close(mqdes);

    free(msg);
    return EXIT_SUCCESS;
}
