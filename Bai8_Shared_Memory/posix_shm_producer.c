// posix_shm_producer.c
// Producer: tạo vùng nhớ POSIX shared memory, ghi 1 cấu trúc product vào đó,
// và post (sem_post) một semaphore để báo cho consumer rằng dữ liệu đã sẵn sàng.
#include <stdio.h>      // printf, perror
#include <stdlib.h>     // atoi, atof, exit
#include <string.h>     // strncpy, memcpy, strlen
#include <fcntl.h>      // O_CREAT, O_RDWR (các flag cho shm_open)
#include <sys/mman.h>   // shm_open, mmap, PROT_READ/PROT_WRITE, MAP_SHARED
#include <sys/stat.h>   // mode constants (ví dụ 0666)
#include <unistd.h>     // ftruncate, close
#include <semaphore.h>  // sem_open, sem_post, sem_close
#include <errno.h>      // errno

// Đặt tên cho shared memory và semaphore.
// Lưu ý: tên bắt đầu bằng '/' theo chuẩn POSIX (ví dụ "/my_shared_mem").
#define SHM_NAME "/my_shared_mem"
#define SEM_NAME "/my_shm_sem"

// Độ dài cố định cho trường tên trong cấu trúc product
#define NAME_LEN 64

// Định nghĩa cấu trúc dữ liệu sẽ lưu trong shared memory.
// Việc dùng struct giúp cả producer và consumer hiểu cùng layout bộ nhớ.
typedef struct {
    int id;                 // id sản phẩm (4 bytes thường)
    char name[NAME_LEN];    // tên sản phẩm (NAME_LEN bytes)
    double price;           // giá sản phẩm (8 bytes thường)
} product_t;

int main(int argc, char *argv[]) {
    // Khởi tạo biến product_t prod chứa dữ liệu sẽ ghi vào shared memory.
    product_t prod;

    // Nếu người dùng truyền tham số dòng lệnh thì dùng chúng.
    // Chúng ta hỗ trợ: ./producer <id> "<name>" <price>
    if (argc >= 4) {
        // argv[1] -> id (string -> int)
        prod.id = atoi(argv[1]);                       // atoi: convert string -> int

        // argv[2] -> name (copy an toàn)
        // strncpy chỉ copy tối đa NAME_LEN-1 để chừa chỗ cho '\0'
        strncpy(prod.name, argv[2], NAME_LEN - 1);

        // Đảm bảo chuỗi kết thúc bằng '\0' (để tránh buffer không terminate)
        prod.name[NAME_LEN - 1] = '\0';

        // argv[3] -> price (string -> double)
        prod.price = atof(argv[3]);                   // atof: convert string -> double
    } else {
        // Nếu không có tham số, dùng giá trị mặc định để dễ test nhanh.
        prod.id = 1001;
        strncpy(prod.name, "Default Product", NAME_LEN - 1);
        prod.name[NAME_LEN - 1] = '\0';
        prod.price = 9.99;
    }

    // 1) Tạo hoặc mở shared memory object:
    // shm_open trả về file descriptor (giống open file), dùng O_CREAT|O_RDWR để tạo nếu chưa có.
    // Quyền 0666 cho phép đọc/ghi cho owner/group/others (thích hợp môi trường dev).
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        // Nếu thất bại, in lỗi và thoát.
        perror("shm_open");
        return 1;
    }

    // 2) Đặt kích thước vùng nhớ bằng sizeof(product_t).
    // ftruncate dùng để thiết lập kích thước file descriptor tương ứng shared object.
    if (ftruncate(shm_fd, sizeof(product_t)) == -1) {
        perror("ftruncate");
        close(shm_fd);            // đóng fd trước khi thoát
        shm_unlink(SHM_NAME);     // cố gắng unlink để cleanup
        return 1;
    }

    // 3) mmap: ánh xạ vùng nhớ vào không gian địa chỉ của tiến trình.
    // - NULL: kernel tự chọn địa chỉ
    // - size: sizeof(product_t)
    // - PROT_READ | PROT_WRITE: cho phép đọc/ghi vùng nhớ
    // - MAP_SHARED: thay đổi sẽ hiển thị cho các tiến trình khác mapping cùng object
    // - shm_fd: file descriptor của object
    void *shm_ptr = mmap(NULL, sizeof(product_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    // 4) Mở (hoặc tạo) named semaphore.
    // sem_open với O_CREAT sẽ tạo semaphore nếu chưa tồn tại; initial value = 0 để consumer phải chờ.
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        // Nếu thất bại: unmap, close fd, unlink shm (cleanup), rồi thoát.
        perror("sem_open");
        munmap(shm_ptr, sizeof(product_t));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    // 5) Ghi cấu trúc prod vào vùng nhớ (memcpy dùng để copy toàn bộ bytes của struct)

    memcpy(shm_ptr, &prod, sizeof(product_t));

    // In ra console để biết program đã ghi thành công (debug/log).
    printf("Producer: wrote product -> id=%d, name=\"%s\", price=%.2f\n",
           prod.id, prod.name, prod.price);

    // 6) Thông báo cho consumer là đã có dữ liệu bằng sem_post()
    // sem_post tăng giá trị của semaphore (value++) và nếu consumer đang block sem_wait sẽ wake.
    if (sem_post(sem) == -1) {
        perror("sem_post");
    } else {
        printf("Producer: signaled consumer via semaphore %s\n", SEM_NAME);
    }

    // 7) Cleanup local resources:
    // - munmap để bỏ ánh xạ vùng nhớ khỏi tiến trình này
    // - close shm_fd để đóng file descriptor
    if (munmap(shm_ptr, sizeof(product_t)) == -1) {
        perror("munmap");
    }
    if (close(shm_fd) == -1) {
        perror("close shm_fd");
    }

    // 8) Đóng semaphore (không unlink để consumer vẫn có thể open)
    if (sem_close(sem) == -1) {
        perror("sem_close");
    }

    // LƯU Ý QUAN TRỌNG:
    // - Không gọi shm_unlink() hoặc sem_unlink() ở đây nếu bạn muốn consumer có thể mở
    //   object sau khi producer thoát. Nếu producer gọi unlink ngay lập tức thì consumer
    //   vẫn đọc được vùng nhớ đã map nếu consumer đã mở trước, nhưng tên sẽ bị xóa khỏi hệ thống.
    // - Nên để consumer chịu trách nhiệm gọi shm_unlink và sem_unlink sau khi chắc chắn đã đọc xong.
    return 0;
}
