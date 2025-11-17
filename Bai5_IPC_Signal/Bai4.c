#include <stdio.h>          
#include <signal.h>        
#include <unistd.h>         
#include <string.h>         
#include <sys/select.h>     
#include <stdlib.h>         

// Hàm xử lý tín hiệu (signal handler)
void sig_handler(int signo) {                      // khai báo hàm, nhận một tham số là số hiệu tín hiệu
    if (signo == SIGINT) {                         // nếu tín hiệu là SIGINT (thường do Ctrl+C)
        printf("\nSIGINT received.\n");            // in ra thông báo đã nhận SIGINT
        fflush(stdout);                            // flush output để đảm bảo chuỗi được in ngay lập tức
    }                                              // kết thúc nhánh if SIGINT
    else if (signo == SIGTERM) {                   // nếu tín hiệu là SIGTERM (thường do kill -15)
        printf("\nSIGTERM received. Exiting...\n");// in ra thông báo đã nhận SIGTERM
        fflush(stdout);                            // flush output trước khi thoát
        exit(0);                                   // kết thúc chương trình sạch (gọi exit)
    }                                              // kết thúc nhánh else if SIGTERM
}                                                  // kết thúc hàm sig_handler

int main() {                                       // hàm chính chương trình
    char buf[256];                                 // cấp phát buffer 256 byte để lưu input từ người dùng

    signal(SIGINT, sig_handler);                   // đăng ký handler sig_handler cho tín hiệu SIGINT
    signal(SIGTERM, sig_handler);                  // đăng ký handler sig_handler cho tín hiệu SIGTERM

    printf("Program started. Type 'exit' to quit.\n"); // in thông báo bắt đầu chương trình và hướng dẫn cơ bản
    fflush(stdout);                                // flush để đảm bảo thông báo được hiện ngay

    while (1) {                                    // vòng lặp chính chạy vô hạn cho tới khi có lệnh thoát
        fd_set rfds;                               // khai báo tập file descriptor để theo dõi đọc
        FD_ZERO(&rfds);                            // khởi tạo tập rfds về rỗng
        FD_SET(STDIN_FILENO, &rfds);               // thêm STDIN (mã 0) vào tập rfds để theo dõi

        /* gọi select để chờ dữ liệu có thể đọc trên STDIN
           tham số đầu là maxfd+1 (ở đây STDIN_FILENO=0 => 0+1=1)
           timeout = NULL => chờ vô hạn (select sẽ return khi có dữ liệu hoặc bị signal interrupt) */
        int ret = select(STDIN_FILENO + 1, &rfds, NULL, NULL, NULL);

        if (ret < 0) {                             // nếu select trả về âm => có lỗi
            perror("select error");                // in lỗi chi tiết ra stderr
            continue;                              // quay lại vòng lặp, thử select lại
        }                                          // kết thúc xử lý lỗi select

        if (FD_ISSET(STDIN_FILENO, &rfds)) {       // nếu STDIN có thể đọc (người dùng vừa nhập Enter)
            if (fgets(buf, sizeof(buf), stdin) != NULL) { // đọc một dòng từ stdin vào buf, có NULL nếu EOF hoặc lỗi
                /* kiểm tra nếu người dùng nhập "exit" (4 ký tự đầu)
                   dùng strncmp để so sánh an toàn (tránh overflow) */
                if (strncmp(buf, "exit", 4) == 0) { // nếu 4 ký tự đầu là "exit"
                    printf("Exiting program by user command.\n"); // in thông báo thoát do user
                    fflush(stdout);               // flush output trước khi thoát vòng lặp
                    break;                        // thoát vòng lặp while để kết thúc chương trình
                }                                  // kết thúc nhánh if "exit"

                /* nếu không phải "exit" thì in nội dung người dùng vừa nhập
                   printf sẽ in luôn ký tự newline nếu buf có \n từ fgets */
                printf("User input: %s", buf);      // in nội dung nhập từ người dùng
                fflush(stdout);                     // flush để hiển thị ngay trên console
            }                                      // kết thúc nhánh if fgets != NULL
            else {                                 // nếu fgets trả về NULL (EOF hoặc lỗi)
                /* có thể do Ctrl+D (EOF) hoặc lỗi đọc; ở đây ta chọn tiếp tục vòng lặp.
                   Bạn có thể xử lý EOF bằng cách break nếu muốn thoát. */
                clearerr(stdin);                   // xóa trạng thái lỗi/EOF của stdin (tùy chọn)
            }                                      // kết thúc nhánh else fgets == NULL
        }                                          // kết thúc nhánh if FD_ISSET
    }                                              // kết thúc vòng while

    return 0;                                      // trả về 0 (kết thúc chương trình bình thường)
}                                                  // kết thúc hàm main

