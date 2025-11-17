#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Biến đếm toàn cục để đếm số lần nhận SIGINT
int count = 0;

// Hàm xử lý tín hiệu
void handler(int signo) {
    // In thông báo mỗi lần nhận Ctrl+C
    printf("SIGINT received\n");

    // Tăng biến đếm
    count++;

    // Nếu nhận 3 lần thì thoát chương trình
    if (count >= 3) {
        printf("Exit after receiving SIGINT 3 times.\n");
        _exit(0);  // dùng _exit để thoát ngay trong signal handler
    }
}

int main() {
    // Đăng ký bắt tín hiệu SIGINT
    signal(SIGINT, handler);

    // Vòng lặp vô hạn
    while (1) {
        printf("Running... Press Ctrl+C\n");
        sleep(1);  // giảm tải CPU
    }

    return 0;
}
