// producer.c
// Producer process: lựa chọn ngẫu nhiên 1 trong 2 món của chef tương ứng,
// chờ empty, lock mutex, đặt món vào tray (first empty cell), post full.
// Chạy vô hạn.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include "shared.h"

static void random_sleep(int min_s, int max_s) {
    int t = (rand() % (max_s - min_s + 1)) + min_s;
    sleep(t);
}

int main(int argc, char **argv) {
    // Arg 1 bắt buộc: "vegan" hoặc "nonvegan"
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <vegan|nonvegan>\n", argv[0]);
        return 1;
    }

    int is_vegan = (strcmp(argv[1], "vegan") == 0);

    // Chọn tên món theo chef
    const char *dish1, *dish2;
    int dish_value;
    const char *chef_name;
    if (is_vegan) {
        chef_name = "Chef Portecelli";
        dish1 = "Pistachio Pesto Pasta";
        dish2 = "Avocado Fruit Salad";
        dish_value = VEGAN;
    } else {
        chef_name = "Chef Donatello";
        dish1 = "Fettuccine Chicken Alfredo";
        dish2 = "Garlic Sirloin Steak";
        dish_value = NONVEGAN;
    }

    // Mở shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("producer: shm_open");
        exit(EXIT_FAILURE);
    }

    // mmap shared memory
    shared_data_t *data = mmap(NULL, sizeof(shared_data_t),
                               PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("producer: mmap");
        exit(EXIT_FAILURE);
    }

    // Mở semaphores theo tên
    sem_t *mutex = sem_open(is_vegan ? SEM_MUTEX_VEGAN : SEM_MUTEX_NONVEG, 0);
    sem_t *empty = sem_open(is_vegan ? SEM_EMPTY_VEGAN : SEM_EMPTY_NONVEG, 0);
    sem_t *full  = sem_open(is_vegan ? SEM_FULL_VEGAN  : SEM_FULL_NONVEG, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("producer: sem_open");
        exit(EXIT_FAILURE);
    }

    // Seed random cho mỗi process để sleep ngẫu nhiên khác nhau
    srand(time(NULL) ^ getpid());

    // Chỉ in log ngắn gọn và rõ ràng
    while (1) {
        // Chọn món ngẫu nhiên giữa 2 món đặc trưng
        const char *selected = (rand() % 2 == 0) ? dish1 : dish2;

        // Chờ nếu không còn ô trống
        sem_wait(empty);

        // Vào vùng critical
        sem_wait(mutex);

        // Tìm ô trống đầu tiên và đặt món
        if (is_vegan) {
            for (int i = 0; i < TRAY_SIZE; ++i) {
                if (data->tray_vegan[i] == EMPTY) {
                    data->tray_vegan[i] = dish_value;
                    printf("[%s PID %d] Đặt %s vào ô %d (vegan)\n",
                           chef_name, getpid(), selected, i);
                    fflush(stdout);
                    break;
                }
            }
        } else {
            for (int i = 0; i < TRAY_SIZE; ++i) {
                if (data->tray_nonvegan[i] == EMPTY) {
                    data->tray_nonvegan[i] = dish_value;
                    printf("[%s PID %d] Đặt %s vào ô %d (non-vegan)\n",
                           chef_name, getpid(), selected, i);
                    fflush(stdout);
                    break;
                }
            }
        }

        // Thoát vùng critical
        sem_post(mutex);

        // Tăng full để báo consumer
        sem_post(full);

        // Nghỉ 1-5 giây trước khi nấu tiếp
        random_sleep(1, 5);
    }

    return 0;
}
