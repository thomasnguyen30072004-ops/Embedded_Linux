// launcher.c
// Tạo POSIX shared memory (shm_open + ftruncate + mmap)
// Khởi tạo named semaphores (sem_open với O_CREAT).
// Fork + exec 5 tiến trình (2 producer, 3 consumer).
// In trạng thái hai mâm mỗi 10s

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>      // O_* constants
#include <sys/mman.h>   // mmap, shm_open
#include <sys/stat.h>   // mode constants
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include "shared.h"

// Hàm tiện ích: đếm số món trong tray
static int count_items(int *tray) {
    int c = 0;
    for (int i = 0; i < TRAY_SIZE; ++i) if (tray[i] != EMPTY) ++c;
    return c;
}

int main(int argc, char **argv) {
    // 1) Tạo/open shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // 2) Kích thước vùng shared memory bằng kích thước shared_data_t
    if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // 3) mmap vùng shared memory
    shared_data_t *data = mmap(NULL, sizeof(shared_data_t),
                               PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // 4) Khởi tạo tray về trạng thái rỗng
    for (int i = 0; i < TRAY_SIZE; ++i) {
        data->tray_vegan[i] = EMPTY;
        data->tray_nonvegan[i] = EMPTY;
    }

    // 5) Tạo (hoặc mở) named semaphores. Nếu đã tồn tại, unlink trước để đảm bảo trạng thái mới.
    // NOTE: unlink trước (sem_unlink) giúp tránh lỗi nếu chương trình trước crash và để sem linger.
    sem_unlink(SEM_MUTEX_VEGAN);
    sem_unlink(SEM_EMPTY_VEGAN);
    sem_unlink(SEM_FULL_VEGAN);
    sem_unlink(SEM_MUTEX_NONVEG);
    sem_unlink(SEM_EMPTY_NONVEG);
    sem_unlink(SEM_FULL_NONVEG);

    // Tạo các semaphore với giá trị khởi tạo:
    // mutex = 1 (binary), empty = TRAY_SIZE, full = 0
    sem_t *mutex_vegan = sem_open(SEM_MUTEX_VEGAN, O_CREAT | O_EXCL, 0666, 1);
    sem_t *empty_vegan = sem_open(SEM_EMPTY_VEGAN, O_CREAT | O_EXCL, 0666, TRAY_SIZE);
    sem_t *full_vegan  = sem_open(SEM_FULL_VEGAN,  O_CREAT | O_EXCL, 0666, 0);

    sem_t *mutex_nonveg = sem_open(SEM_MUTEX_NONVEG, O_CREAT | O_EXCL, 0666, 1);
    sem_t *empty_nonveg = sem_open(SEM_EMPTY_NONVEG, O_CREAT | O_EXCL, 0666, TRAY_SIZE);
    sem_t *full_nonveg  = sem_open(SEM_FULL_NONVEG,  O_CREAT | O_EXCL, 0666, 0);

    if (mutex_vegan == SEM_FAILED || empty_vegan == SEM_FAILED || full_vegan == SEM_FAILED ||
        mutex_nonveg == SEM_FAILED || empty_nonveg == SEM_FAILED || full_nonveg == SEM_FAILED) {
        perror("sem_open (creating)");
        // Nếu thất bại thì cleanup một ít
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    // 6) Exec các tiến trình con (5 chương trình). Dùng execv với argv truyền kiểu.
    // Đường dẫn mặc định giả sử tệp binary được build là ./producer và ./consumer
    pid_t p;
    // chef vegan
    p = fork();
    if (p == 0) {
        // con: exec producer vegan
        char *args[] = {"./producer", "vegan", NULL};
        execv(args[0], args);
        perror("execv producer vegan");
        _exit(EXIT_FAILURE);
    }

    // chef non-vegan
    p = fork();
    if (p == 0) {
        char *args[] = {"./producer", "nonvegan", NULL};
        execv(args[0], args);
        perror("execv producer nonvegan");
        _exit(EXIT_FAILURE);
    }

    // customer 1: wants non-vegan only
    p = fork();
    if (p == 0) {
        char *args[] = {"./consumer", "nonvegan", NULL};
        execv(args[0], args);
        perror("execv consumer nonvegan");
        _exit(EXIT_FAILURE);
    }

    // customer 2: wants vegan only
    p = fork();
    if (p == 0) {
        char *args[] = {"./consumer", "vegan", NULL};
        execv(args[0], args);
        perror("execv consumer vegan");
        _exit(EXIT_FAILURE);
    }

    // customer 3: hybrid (wants both)
    p = fork();
    if (p == 0) {
        char *args[] = {"./consumer", "hybrid", NULL};
        execv(args[0], args);
        perror("execv consumer hybrid");
        _exit(EXIT_FAILURE);
    }

    // 7) Parent: in trạng thái mâm mỗi 10s — chạy vô hạn (Ctrl+C để dừng)
    while (1) {
        sleep(10);
        int veg_count = count_items(data->tray_vegan);
        int nonveg_count = count_items(data->tray_nonvegan);
        printf("[LAUNCHER] Vegan tray: %d/%d | Non-vegan tray: %d/%d\n",
               veg_count, TRAY_SIZE, nonveg_count, TRAY_SIZE);
        fflush(stdout);
    }

    // 8) (Không tới đây) Cleanup nếu có - ví dụ khi muốn dừng sạch:
    // sem_close(...); sem_unlink(...); munmap(...); shm_unlink(...);
    return 0;
}
