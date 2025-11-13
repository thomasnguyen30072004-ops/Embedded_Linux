
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// --- Biến chia sẻ ---
int data;               // dữ liệu mà producer tạo, consumer đọc
int data_ready = 0;     // cờ báo hiệu "dữ liệu có sẵn chưa"

// --- Công cụ đồng bộ ---
pthread_mutex_t lock;   // Mutex đảm bảo chỉ 1 thread truy cập vùng chia sẻ
pthread_cond_t cond;    // Condition Variable để chờ / báo hiệu giữa 2 luồng


// ============================
// HÀM PRODUCER (nhà sản xuất)
// ============================
void* producer(void* arg) {
    for (int i = 0; i < 10; i++) {

        // 1️⃣ Khóa mutex để truy cập vùng chia sẻ an toàn
        pthread_mutex_lock(&lock);

        // 2️⃣ Tạo dữ liệu ngẫu nhiên
        data = rand() % 100;
        data_ready = 1;     // bật cờ cho biết “đã có dữ liệu mới”
        printf("Producer: tạo dữ liệu %d\n", data);

        // 3️⃣ Báo hiệu cho Consumer rằng có dữ liệu mới
        //  → đánh thức 1 thread đang "wait" trên cond
        pthread_cond_signal(&cond);

        // 4️⃣ Mở khóa để cho Consumer vào đọc dữ liệu
        pthread_mutex_unlock(&lock);

        // 5️⃣ Nghỉ 1s để mô phỏng thời gian sản xuất
        sleep(1);
    }

    pthread_exit(NULL);
}


// ============================
// HÀM CONSUMER (người tiêu thụ)
// ============================
void* consumer(void* arg) {
    for (int i = 0; i < 10; i++) {

        // 1️⃣ Khóa mutex trước khi kiểm tra cờ data_ready
        pthread_mutex_lock(&lock);

        // 2️⃣ Nếu chưa có dữ liệu → chờ tín hiệu từ Producer
        while (!data_ready)
            // pthread_cond_wait() sẽ:
            //  (a) tự động UNLOCK mutex → để producer có thể vào
            //  (b) đưa thread này vào trạng thái "ngủ" (blocked)
            //  (c) khi được signal → tự động LOCK mutex lại và tiếp tục chạy
            pthread_cond_wait(&cond, &lock);

        // 3️⃣ Khi được đánh thức → đọc dữ liệu
        printf("Consumer: đọc dữ liệu %d\n", data);

        // 4️⃣ Reset cờ (đánh dấu dữ liệu đã được xử lý)
        data_ready = 0;

        // 5️⃣ Mở khóa mutex để Producer có thể tiếp tục tạo dữ liệu
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}


int main() {
    pthread_t prod, cons;

    // Khởi tạo mutex và condition variable
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // Tạo 2 luồng: Producer & Consumer
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    // Đợi 2 luồng kết thúc
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // Hủy tài nguyên
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
