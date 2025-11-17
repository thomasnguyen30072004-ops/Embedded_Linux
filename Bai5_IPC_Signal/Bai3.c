#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void child_handler(int signo) {
    printf("Received signal from parent\n");
}

int main() {
    pid_t pid = fork(); // tạo process con

    if (pid == 0) {
        // --- PROCESS CON ---
        signal(SIGUSR1, child_handler);  // đăng ký bắt tín hiệu

        while (1) {
            pause();  // chờ tín hiệu từ cha
        }
    } 
    else {
        // --- PROCESS CHA ---
        for (int i = 0; i < 5; i++) {
            sleep(2);              // chờ 2 giây
            kill(pid, SIGUSR1);    // gửi tín hiệu đến con
            printf("Parent sent SIGUSR1 (%d/5)\n", i + 1);
        }

        sleep(1);
        printf("Parent done.\n");
    }

    return 0;
}
