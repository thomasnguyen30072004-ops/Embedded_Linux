// shared.h
// Các định nghĩa chung (tray size, giá trị món, tên shm/sem) và struct shared.
// Include file này trong tất cả các file .c

#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>

#define TRAY_SIZE 10

// Giá trị món trong ô tray
#define EMPTY   0
#define VEGAN   1
#define NONVEGAN 2

// Tên POSIX shared memory object
// (bắt đầu bằng '/', không có dấu space)
#define SHM_NAME "/devlinux_ipc_shm"

// Tên semaphore (POSIX named semaphores). Dùng tiền tố, dễ đổi.
#define SEM_MUTEX_VEGAN    "/devlinux_mutex_vegan"
#define SEM_EMPTY_VEGAN    "/devlinux_empty_vegan"
#define SEM_FULL_VEGAN     "/devlinux_full_vegan"

#define SEM_MUTEX_NONVEG   "/devlinux_mutex_nonveg"
#define SEM_EMPTY_NONVEG   "/devlinux_empty_nonveg"
#define SEM_FULL_NONVEG    "/devlinux_full_nonveg"

// Struct lưu tray trong shared memory.
// Lưu ý: KHÔNG chứa sem_t không-named ở đây vì ta dùng named semaphores.
typedef struct {
    int tray_vegan[TRAY_SIZE];      // each cell: 0-empty,1-vegan,2-nonvegan
    int tray_nonvegan[TRAY_SIZE];
} shared_data_t;

#endif // SHARED_H
