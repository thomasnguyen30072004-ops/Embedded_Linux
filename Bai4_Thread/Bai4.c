
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int resource = 0;               // Biến tài nguyên chia sẻ
pthread_rwlock_t rwlock;        // Read-Write lock

// Reader: chỉ đọc dữ liệu
void* reader(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);     // Khóa đọc (cho phép nhiều reader)
        printf("Reader %d đọc resource = %d\n", id, resource);
        pthread_rwlock_unlock(&rwlock);     // Mở khóa
        usleep(100000);                     // Nghỉ 0.1s
    }
    pthread_exit(NULL);
}

// Writer: ghi dữ liệu
void* writer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_wrlock(&rwlock);     // Khóa ghi (độc quyền)
        resource++;
        printf("Writer %d ghi resource = %d\n", id, resource);
        pthread_rwlock_unlock(&rwlock);
        usleep(150000);                     // Nghỉ 0.15s
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t r[5], w[2];
    int r_id[5], w_id[2];

    pthread_rwlock_init(&rwlock, NULL);     // Khởi tạo rwlock

    // Tạo 5 Reader
    for (int i = 0; i < 5; i++) {
        r_id[i] = i + 1;
        pthread_create(&r[i], NULL, reader, &r_id[i]);
    }

    // Tạo 2 Writer
    for (int i = 0; i < 2; i++) {
        w_id[i] = i + 1;
        pthread_create(&w[i], NULL, writer, &w_id[i]);
    }

    // Đợi tất cả luồng kết thúc
    for (int i = 0; i < 5; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(w[i], NULL);

    pthread_rwlock_destroy(&rwlock);        // Hủy rwlock
    return 0;
}
