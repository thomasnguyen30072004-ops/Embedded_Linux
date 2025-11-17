#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int seconds = 0;  // biến đếm giây

void timer_handler(int signo) {
    seconds++;  // tăng giây
    printf("Timer: %d seconds\n", seconds);

    if (seconds >= 10) {
        printf("Timer finished.\n");
        _exit(0);
    }

    alarm(1);  // đặt lại timer để tiếp tục đếm mỗi giây
}

int main() {
    // đăng ký xử lý SIGALRM
    signal(SIGALRM, timer_handler);

    alarm(1);  // kích hoạt timer lần đầu

    while (1) {
        pause();  // chờ tín hiệu, không tốn CPU
    }

    return 0;
}
