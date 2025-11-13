#include <stdio.h>     // 
#include <pthread.h>    // Thư viện cung cấp các hàm liên quan tới thread (pthread_...)


// Hàm mà mỗi luồng sẽ thực thi (entry point của thread)
void* thread_func(void* arg) {
    // pthread_self() → trả về ID (pthread_t) của luồng hiện tại
    pthread_t tid = pthread_self();

    // In ra thông báo gồm ID của luồng
    printf("Thread với ID %lu đang chạy!\n", tid);

    // pthread_exit() → kết thúc luồng hiện tại một cách an toàn
    pthread_exit(NULL);
}


int main() {
    // Khai báo 2 biến kiểu pthread_t → để lưu ID của 2 luồng
    pthread_t t1, t2;

    // Tạo luồng 1:
    // pthread_create(&t1, NULL, thread_func, NULL)
    //  - &t1: địa chỉ để lưu ID của luồng mới
    //  - NULL: thuộc tính luồng (NULL = mặc định)
    //  - thread_func: hàm mà luồng sẽ chạy
    //  - NULL: tham số truyền vào hàm (ở đây không dùng)
    pthread_create(&t1, NULL, thread_func, NULL);

    // Tạo luồng 2 (tương tự)
    pthread_create(&t2, NULL, thread_func, NULL);

    // pthread_join() → luồng chính (main thread) sẽ đợi luồng con kết thúc
    // Khi t1 kết thúc, main mới tiếp tục dòng kế tiếp
    pthread_join(t1, NULL);
	printf("Ket thuc tien trinh 1 vao tien trinh 2\n");

    // Tương tự: đợi luồng t2 kết thúc
    pthread_join(t2, NULL);
	printf("Ket thuc tien trinh 2\n");
    // Khi cả 2 luồng kết thúc, in ra thông báo
    printf("Tất cả luồng đã kết thúc.\n");

    // return 0 → kết thúc chương trình chính (main thread)
    return 0;
}
