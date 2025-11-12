#include <stdio.h>      /* printf */
#include <stdlib.h>     /* exit */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork, sleep, getpid, getppid */

int main(void) {
    pid_t pid = fork();  /* tạo tiến trình con */
    if (pid < 0) { perror("fork"); return 1; } /* kiểm tra lỗi fork */
    if (pid == 0) {      /* -- tiến trình con -- */
        for (int i = 0; i < 20; i++) { /* lặp in PPID nhiều lần để quan sát */
            printf("Child: PID=%d, PPID=%d\n", getpid(), getppid());
            sleep(2);    /* ngủ 2 giây; trong thời gian này parent sẽ exit -> con trở thành orphan */
        }
    } else {             /* -- tiến trình cha -- */
        printf("Parent: PID=%d, child PID=%d. Parent exits now.\n", getpid(), pid);
        exit(0);         /* cha exit ngay; kernel sẽ chuyển con cho init/systemd -> PPID của con đổi */
    }
    return 0;
}
