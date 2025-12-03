// posix_shm_consumer.c
// Consumer: mở POSIX shared memory đã do producer tạo, chờ semaphore,
// đọc struct product từ vùng nhớ và in ra, sau đó cleanup và unlink.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>      // O_RDONLY
#include <sys/mman.h>   // shm_open, mmap, PROT_READ
#include <sys/stat.h>
#include <unistd.h>     // close, munmap
#include <semaphore.h>  // sem_open, sem_wait, sem_close, sem_unlink
#include <errno.h>

#define SHM_NAME "/my_shared_mem"
#define SEM_NAME "/my_shm_sem"
#define NAME_LEN 64

typedef struct {
    int id;
    char name[NAME_LEN];
    double price;
} product_t;

int main(int argc, char *argv[]) {
    // 1) Mở shared memory: ở consumer ta giả sử producer đã tạo trước rồi,
    // nên ta dùng O_RDONLY (chỉ đọc).
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        // Nếu producer chưa tạo object, shm_open sẽ fail -> báo lỗi cho người dùng.
        perror("shm_open (consumer)");
        return 1;
    }

    // 2) mmap vùng nhớ để đọc dữ liệu.
    // Ở consumer chỉ cần PROT_READ vì chỉ đọc, nhưng có thể dùng PROT_READ|PROT_WRITE nếu cần.
    void *shm_ptr = mmap(NULL, sizeof(product_t), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap (consumer)");
        close(shm_fd);
        return 1;
    }

    // 3) Mở semaphore (không O_CREAT) vì producer đã tạo nó.
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        // Nếu semaphore chưa tồn tại (producer chưa tạo), sem_open sẽ thất bại.
        perror("sem_open (consumer)");
        munmap(shm_ptr, sizeof(product_t));
        close(shm_fd);
        return 1;
    }

    // 4) Chờ producer signal: sem_wait sẽ block ở đây nếu giá trị semaphore = 0.
    printf("Consumer: waiting for producer signal...\n");
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        sem_close(sem);
        munmap(shm_ptr, sizeof(product_t));
        close(shm_fd);
        return 1;
    }

    // 5) Sau sem_wait trả về, producer đã sem_post -> dữ liệu đã sẵn sàng.
    // Copy bytes từ vùng shared memory vào struct local để dễ xử lý.
    product_t prod;
    memcpy(&prod, shm_ptr, sizeof(product_t));
    printf("Consumer: read product -> id=%d, name=\"%s\", price=%.2f\n",
           prod.id, prod.name, prod.price);

    // 6) Cleanup mapping và file descriptor
    if (munmap(shm_ptr, sizeof(product_t)) == -1) {
        perror("munmap (consumer)");
    }
    if (close(shm_fd) == -1) {
        perror("close shm_fd (consumer)");
    }

    // 7) Đóng semaphore
    if (sem_close(sem) == -1) {
        perror("sem_close (consumer)");
    }

    // 8) Unlink shared memory và semaphore để xóa named objects khỏi hệ thống kernel.
    //    Thao tác này nên làm khi bạn chắc chắn không còn tiến trình nào cần chúng.
    if (shm_unlink(SHM_NAME) == -1) {
        // Nếu lỗi thường là do object đã bị unlink rồi hoặc không tồn tại.
        // Không in perror() để tránh quá noisy, nhưng bạn có thể uncomment nếu muốn debug.
        // perror("shm_unlink");
    } else {
        printf("Consumer: shm_unlink() succeeded, removed %s\n", SHM_NAME);
    }

    if (sem_unlink(SEM_NAME) == -1) {
        // Tương tự, sem_unlink có thể fail nếu đã bị unlink trước đó.
        // perror("sem_unlink");
    } else {
        printf("Consumer: sem_unlink() succeeded, removed %s\n", SEM_NAME);
    }

    return 0;
}
