#include <stdio.h>      /* printf, perror */
#include <stdlib.h>     /* exit, _exit */
#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* wait, macros WIFEXITED/WEXITSTATUS */
#include <unistd.h>     /* fork, getpid, getppid */

int main(void) {
    pid_t pid;          /* biến lưu PID trả về từ fork() */
    int status;         /* biến để chứa trạng thái exit của con */

    pid = fork();       /* tạo tiến trình con; trả về <0 lỗi, =0 ở con, >0 ở cha (PID con) */
    if (pid < 0) {      /* kiểm tra lỗi fork */
        perror("fork failed"); /* in lỗi ra stderr */
        return 1;       /* trả về mã lỗi cho hệ điều hành */
    }

    if (pid == 0) {     /* -- đoạn chạy ở tiến trình con -- */
        printf("Child: PID=%d, PPID=%d\n", getpid(), getppid());
                          /* in PID của con và PPID (PID của cha hiện tại) */
        _exit(10);      /* thoát tiến trình con với mã 10 (dùng _exit để tránh flush buffer chung) */
    } else {            /* -- đoạn chạy ở tiến trình cha -- */
        printf("Parent: PID=%d, child PID=%d\n", getpid(), pid);
                          /* in PID của cha và PID con (giá trị pid từ fork) */
        pid_t w = wait(&status); /* chờ một con kết thúc và lấy trạng thái vào 'status' */
        if (w == -1) {  /* kiểm tra lỗi wait */
            perror("wait failed");
            return 1;
        }
        if (WIFEXITED(status)) { /* kiểm tra con exit bình thường không (không bị signal) */
            printf("Child %d exited normally with status %d\n", w, WEXITSTATUS(status));
                             /* in PID con (w) và mã thoát thực sự (WEXITSTATUS) */
        } else {
            printf("Child %d did not exit normally\n", w);
                             /* trường hợp con bị kill bởi signal hoặc không exit bình thường */
        }
    }
    return 0;            /* trả về 0 cho hệ điều hành ở tiến trình cha */
}
