#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3       // Số lượng luồng
#define LOOP_COUNT 1000000  // Số vòng lặp mỗi luồng

long long counter = 0;          // Biến toàn cục chia sẻ
pthread_mutex_t lock;           // Đối tượng mutex

// Hàm mà mỗi luồng sẽ thực thi
void* count_func(void* arg) {
    for (int i = 0; i < LOOP_COUNT; i++) {
        // Khóa mutex trước khi truy cập counter (vùng critical)
        pthread_mutex_lock(&lock);

        // Tăng counter
        counter++;

        // Mở khóa mutex sau khi xong
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Khởi tạo mutex (NULL = thuộc tính mặc định)
    pthread_mutex_init(&lock, NULL);

    // Tạo 3 luồng
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, count_func, NULL);
    }

    // Đợi 3 luồng kết thúc
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Hủy mutex sau khi dùng xong
    pthread_mutex_destroy(&lock);

    printf("Giá trị cuối cùng của counter = %lld\n", counter);
    return 0;
}
