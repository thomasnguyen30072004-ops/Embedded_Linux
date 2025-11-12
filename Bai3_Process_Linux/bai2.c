#include <stdio.h>      /* printf, perror, fprintf */
#include <stdlib.h>     /* setenv, getenv, exit, _exit */
#include <unistd.h>     /* fork, execlp, getpid */
#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* wait */
#include <string.h>

int main(void) {
    pid_t pid;          /* lưu PID từ fork */
    int status;         /* dùng để wait (nếu cần) */

    /* Parent sets environment variable */
    setenv("MY_COMMAND", "ls", 1); /* đặt biến môi trường MY_COMMAND="ls", 1 = ghi đè nếu đã tồn tại */

    pid = fork();       /* tạo tiến trình con */
    if (pid < 0) { perror("fork"); return 1; } /* xử lý lỗi fork */

    if (pid == 0) {     /* -- ở tiến trình con -- */
        char *cmd = getenv("MY_COMMAND"); /* đọc biến môi trường do cha đặt */
        if (!cmd) {     /* nếu không tồn tại biến môi trường */
            fprintf(stderr, "MY_COMMAND not set\n");
            _exit(2);   /* thoát con với mã 2 */
        }
        printf("Child: will exec '%s' (PID=%d)\n", cmd, getpid());
                         /* thông báo con sẽ exec cái lệnh nào và PID của con */
        if (strcmp(cmd, "ls") == 0) {    /* nếu lệnh là "ls" */
            execlp("ls", "ls", "-l", NULL); /* thay tiến trình hiện tại bằng chương trình 'ls -l' */
            /* nếu execlp trả về, nghĩa là nó thất bại */
            perror("execlp failed");
            _exit(3);   /* thoát với mã lỗi */
        } else if (strcmp(cmd, "date") == 0) { /* ví dụ lệnh khác */
            execlp("date", "date", NULL); /* exec 'date' */
            perror("execlp failed");
            _exit(3);
        } else {
            fprintf(stderr, "Unknown command: %s\n", cmd);
            _exit(4);
        }
    } else {            /* -- ở tiến trình cha -- */
        wait(&status);   /* chờ con kết thúc (tuỳ bài có thể không cần) */
        printf("Parent: child finished\n"); /* in thông báo con đã xong */
    }
    return 0;
}
