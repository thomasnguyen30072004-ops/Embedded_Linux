// consumer.c
// Consumer process: tùy mode lấy món mong muốn.
// mode = "nonvegan" -> chỉ lấy nonvegan
// mode = "vegan"    -> chỉ lấy vegan
// mode = "hybrid"   -> lấy cả hai (lần lượt nonvegan rồi vegan trong loop)
// Sau khi lấy 1 món, consumer sleep 10-15s.

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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nonvegan|vegan|hybrid>\n", argv[0]);
        return 1;
    }

    int want_nonveg = 0, want_veg = 0;
    if (strcmp(argv[1], "nonvegan") == 0) want_nonveg = 1;
    else if (strcmp(argv[1], "vegan") == 0) want_veg = 1;
    else if (strcmp(argv[1], "hybrid") == 0) { want_nonveg = 1; want_veg = 1; }
    else {
        fprintf(stderr, "Invalid mode: %s\n", argv[1]);
        return 1;
    }

    // Open shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("consumer: shm_open");
        exit(EXIT_FAILURE);
    }

    shared_data_t *data = mmap(NULL, sizeof(shared_data_t),
                               PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("consumer: mmap");
        exit(EXIT_FAILURE);
    }

    // Open semaphores
    sem_t *mutex_veg   = sem_open(SEM_MUTEX_VEGAN, 0);
    sem_t *empty_veg   = sem_open(SEM_EMPTY_VEGAN, 0);
    sem_t *full_veg    = sem_open(SEM_FULL_VEGAN, 0);

    sem_t *mutex_non   = sem_open(SEM_MUTEX_NONVEG, 0);
    sem_t *empty_non   = sem_open(SEM_EMPTY_NONVEG, 0);
    sem_t *full_non    = sem_open(SEM_FULL_NONVEG, 0);

    if (mutex_veg == SEM_FAILED || empty_veg == SEM_FAILED || full_veg == SEM_FAILED ||
        mutex_non == SEM_FAILED || empty_non == SEM_FAILED || full_non == SEM_FAILED) {
        perror("consumer: sem_open");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) ^ getpid());

    // Loop vô hạn: cố lấy món theo đòi hỏi, nếu hybrid thì lấy 2 loại (mỗi loại sau khi lấy sẽ sleep 10-15s)
    while (1) {
        if (want_nonveg) {
            // Chờ có món nonvegan
            sem_wait(full_non);
            sem_wait(mutex_non);

            // Lấy món: tìm ô first non-empty và set thành EMPTY
            for (int i = 0; i < TRAY_SIZE; ++i) {
                if (data->tray_nonvegan[i] != EMPTY) {
                    data->tray_nonvegan[i] = EMPTY;
                    printf("[Customer PID %d] Lấy món NON-VEGAN ở ô %d\n", getpid(), i);
                    fflush(stdout);
                    break;
                }
            }

            sem_post(mutex_non);
            sem_post(empty_non);

            // Sau khi lấy 1 món nonvegan, phải chờ 10-15s trước khi tiếp tục
            random_sleep(10, 15);
        }

        if (want_veg) {
            // Chờ có món vegan
            sem_wait(full_veg);
            sem_wait(mutex_veg);

            for (int i = 0; i < TRAY_SIZE; ++i) {
                if (data->tray_vegan[i] != EMPTY) {
                    data->tray_vegan[i] = EMPTY;
                    printf("[Customer PID %d] Lấy món VEGAN ở ô %d\n", getpid(), i);
                    fflush(stdout);
                    break;
                }
            }

            sem_post(mutex_veg);
            sem_post(empty_veg);

            // Sau khi lấy 1 món vegan, phải chờ 10-15s trước khi tiếp tục
            random_sleep(10, 15);
        }
    }

    return 0;
}
