/* task_worker.c
 * Worker: mở POSIX message queue, chờ và xử lý các tác vụ theo độ ưu tiên.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>     // mq_open, mq_receive, mq_close, mq_unlink
#include <errno.h>
#include <signal.h>
#include <unistd.h>     // sleep()
#include <fcntl.h>      // O_* constants
#include <sys/stat.h>   // mode constants

// Tên hàng đợi (theo tài liệu nên bắt đầu bằng '/')
const char *QUEUE_NAME = "/my_task_queue";

// Mô tả thuộc tính mặc định (thay đổi nếu cần)
#define DEFAULT_MAXMSG 10       // số message tối đa trong queue
#define DEFAULT_MSGSIZE 1024    // kích thước tối đa một message (byte)

static mqd_t mqdes = (mqd_t) -1; // descriptor của message queue (global để signal handler dùng)
static int running = 1;          // cờ vòng lặp chính

// Hàm dọn dẹp: đóng descriptor và unlink (xóa) queue
void cleanup_and_exit(int exit_code)
{
    // Nếu descriptor hợp lệ, đóng
    if (mqdes != (mqd_t)-1) {
        mq_close(mqdes); // ignore error here
        mqdes = (mqd_t)-1;
    }

    // Xóa tên queue khỏi hệ thống (nếu tồn tại)
    mq_unlink(QUEUE_NAME); // ignore error here

    exit(exit_code);
}

// Signal handler cho SIGINT (Ctrl+C). Gọi dọn dẹp và thoát
void sigint_handler(int sig)
{
    (void)sig; // tránh cảnh báo biến unused
    printf("\nReceived SIGINT, cleaning up and exiting...\n");
    running = 0; // cho vòng lặp dừng lại
    // Gọi cleanup cuối cùng tại main sau vòng lặp
}

// Hàm mở (hoặc tạo) message queue và trả về descriptor
mqd_t open_or_create_queue()
{
    struct mq_attr attr;

    // Thiết lập thuộc tính hàng đợi
    attr.mq_flags = 0;              // blocking mode
    attr.mq_maxmsg = DEFAULT_MAXMSG;
    attr.mq_msgsize = DEFAULT_MSGSIZE;
    attr.mq_curmsgs = 0;

    // O_CREAT: tạo nếu chưa có; O_RDONLY: chỉ đọc (worker)
    mqd_t d = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (d == (mqd_t)-1) {
        perror("mq_open (worker) failed");
        return (mqd_t)-1;
    }

    return d;
}

// Hàm nhận 1 message (blocking). Trả về số byte nhận được hoặc -1 khi lỗi.
// Truyền vào buffer đã cấp phát có kích thước bufsize.
// priority_out nhận giá trị priority nếu không NULL.
ssize_t receive_task(mqd_t d, char *buffer, size_t bufsize, unsigned int *priority_out)
{
    if (!buffer) {
        errno = EINVAL;
        return -1;
    }

    // mq_receive trả về số byte đọc được hoặc -1 nếu lỗi
    ssize_t n = mq_receive(d, buffer, bufsize, priority_out);
    if (n == -1) {
        // Lỗi được báo ra caller
        return -1;
    }

    // đảm bảo chuỗi '\0' nếu có thể (message không luôn null-terminated)
    if ((size_t)n < bufsize) buffer[n] = '\0';
    else buffer[bufsize-1] = '\0';

    return n;
}

int main(int argc, char *argv[])
{
    (void)argc; (void)argv; // không dùng tham số dòng lệnh

    // Cài handler SIGINT để có thể dọn dẹp trước khi thoát
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // Mở hoặc tạo queue
    mqdes = open_or_create_queue();
    if (mqdes == (mqd_t)-1) {
        fprintf(stderr, "Không thể mở hoặc tạo message queue '%s'\n", QUEUE_NAME);
        return EXIT_FAILURE;
    }

    // Lấy thuộc tính thực tế của queue (để biết msgsize)
    struct mq_attr actual_attr;
    if (mq_getattr(mqdes, &actual_attr) == -1) {
        perror("mq_getattr");
        cleanup_and_exit(EXIT_FAILURE);
    }

    // Cấp phát buffer theo kích thước tối đa cho message
    size_t bufsize = (size_t) actual_attr.mq_msgsize + 1; // +1 cho null term
    char *buffer = malloc(bufsize);
    if (!buffer) {
        perror("malloc");
        cleanup_and_exit(EXIT_FAILURE);
    }

    printf("Worker đang chạy, lắng nghe queue '%s' (msgsize=%ld maxmsg=%ld)\n",
           QUEUE_NAME, (long)actual_attr.mq_msgsize, (long)actual_attr.mq_maxmsg);
    printf("Dùng Ctrl+C để dừng và xóa message queue.\n");

    // Vòng lặp nhận message liên tục
    while (running) {
        unsigned int prio = 0;
        ssize_t n = receive_task(mqdes, buffer, bufsize, &prio);
        if (n == -1) {
            // Nếu bị interruption bởi tín hiệu, kiểm tra chạy tiếp hay không
            if (errno == EINTR) {
                // tiếp tục vòng nếu running vẫn = 1
                continue;
            }
            // Nếu queue bị đóng/unlinked từ bên ngoài, in lỗi và break
            perror("mq_receive");
            break;
        }

        // In ra thông tin task
        printf("Processing task (Priority: %u): %s\n", prio, buffer);

        // Giả lập thời gian xử lý
        sleep(1);
    }

    // Giải phóng tài nguyên
    free(buffer);

    // Đóng và xóa queue trước khi thoát
    printf("Đang đóng và xóa queue '%s'...\n", QUEUE_NAME);
    cleanup_and_exit(EXIT_SUCCESS);

    // Không bao giờ đến đây
    return 0;
}
