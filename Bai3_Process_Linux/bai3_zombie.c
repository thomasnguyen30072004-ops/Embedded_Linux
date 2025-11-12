#include <stdio.h>      /* printf, perror */
#include <stdlib.h>     /* exit, _exit */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork, sleep */

int main(void) {
    pid_t pid = fork();  /* tạo tiến trình con */
    if (pid < 0) { perror("fork"); return 1; } /* lỗi fork */
    if (pid == 0) {      /* -- tiến trình con -- */
        printf("Child: PID=%d exiting immediately\n", getpid());
        _exit(0);        /* con kết thúc ngay -> trở thành zombie nếu cha không wait */
    } else {             /* -- tiến trình cha -- */
        printf("Parent: PID=%d, child PID=%d. Sleeping... (do not wait)\n", getpid(), pid);
                          /* thông báo và KHÔNG gọi wait để tạo zombie */
        sleep(60);       /* ngủ lâu để bạn có thời gian chạy 'ps' và thấy tiến trình con <defunct> */
        printf("Parent awake, now exiting\n");
    }
    return 0;
}
