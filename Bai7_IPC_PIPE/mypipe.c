// mypipe.c
// Tái tạo toán tử pipeline: ls -l | wc -l
// Sử dụng pipe(), fork(), dup2(), execlp()

#include <stdio.h>      
#include <stdlib.h>     // Thư viện hỗ trợ hằng số EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h>     // Chứa pipe(), fork(), dup2(), execlp()
#include <sys/types.h>  // Các kiểu dữ liệu pid_t
#include <sys/wait.h>   // Thư viện dùng cho wait(), waitpid()
#include <errno.h>      // Hỗ trợ đọc mã lỗi errno

int main(void) {
    int fds[2];         // fds[0] = đầu đọc, fds[1] = đầu ghi của pipe
    pid_t p1, p2;       // PID cho hai tiến trình con

    if (pipe(fds) == -1) {      // Tạo pipe, trả về 0 nếu thành công
        perror("pipe");         // In lỗi nếu tạo pipe thất bại
        return EXIT_FAILURE;    // Kết thúc chương trình
    }

    // --- Tạo tiến trình con thứ nhất ---
    p1 = fork();                // fork() => tạo tiến trình con thứ nhất
    if (p1 < 0) {               // Nếu fork trả về < 0 là lỗi
        perror("fork p1");      // In lỗi
        close(fds[0]);          // Giải phóng pipe
        close(fds[1]);
        return EXIT_FAILURE;
    }

    if (p1 == 0) {              // Nhánh của tiến trình con 1
        // child 1 = chạy lệnh "ls -l" và ghi output vào pipe

        if (dup2(fds[1], STDOUT_FILENO) == -1) {   // Redirect stdout -> pipe write end
            perror("dup2 child1");     // Lỗi dup2
            _exit(EXIT_FAILURE);       // Dùng _exit tránh flush buffer
        }

        close(fds[0]);         // Đóng đầu đọc pipe (con1 không dùng)
        close(fds[1]);         // Đã dup2 rồi, đóng fd thật

        execlp("ls", "ls", "-l", (char *)NULL);    // Thực thi lệnh ls -l
        perror("execlp ls");     // Nếu execlp chạy được thì dòng này không bao giờ chạy
        _exit(EXIT_FAILURE);     // Chỉ tới đây nếu execlp lỗi
    }

    // --- Tạo tiến trình con thứ hai ---
    p2 = fork();                // fork để tạo con thứ 2
    if (p2 < 0) {               // Lỗi fork
        perror("fork p2");
        waitpid(p1, NULL, 0);   // Chờ con 1 trước để tránh zombie
        close(fds[0]);
        close(fds[1]);
        return EXIT_FAILURE;
    }

    if (p2 == 0) {              // Nhánh của tiến trình con 2
        // child 2 = chạy "wc -l", đọc từ pipe

        if (dup2(fds[0], STDIN_FILENO) == -1) {    // Redirect stdin <- pipe read end
            perror("dup2 child2");
            _exit(EXIT_FAILURE);
        }

        close(fds[0]);          // Đóng đầu đọc thật
        close(fds[1]);          // Đóng đầu ghi pipe (con2 không dùng)

        execlp("wc", "wc", "-l", (char *)NULL);    // Thực thi wc -l
        perror("execlp wc");     // Lỗi execlp
        _exit(EXIT_FAILURE);
    }

    // --- Nhánh của tiến trình cha ---
    close(fds[0]);              // Cha phải đóng cả hai đầu pipe
    close(fds[1]);              // Nếu không wc -l sẽ không nhận EOF và treo

    // Chờ cả hai con kết thúc (tránh zombie)
    int status;
    if (waitpid(p1, &status, 0) == -1) perror("waitpid p1");
    if (waitpid(p2, &status, 0) == -1) perror("waitpid p2");

    return EXIT_SUCCESS;        // Hoàn tất chương trình
}
