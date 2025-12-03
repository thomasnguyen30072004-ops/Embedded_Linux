
======================== SHARED MEMORY ========================
Mục tiêu:
- Tạo vùng nhớ chia sẻ giữa 2 tiến trình
- Producer ghi dữ liệu (struct product_t)
- Consumer đợi tín hiệu semaphore rồi đọc dữ liệu
- Hiểu cách đồng bộ tiến trình trong Linux

## File nguồn
- `posix_shm_producer.c` – tạo shared memory, ghi dữ liệu, sem_post báo consumer  
- `posix_shm_consumer.c` – mở shared memory, sem_wait chờ producer, đọc dữ liệu  

## Cách chạy thử
Terminal 1: ./consumer
Terminal 2: ./producer 123 "Ao_thun" 49.5

##Các hàm IPC quan trọng trong code
Shared Memory
Hàm		Chức năng
shm_open()	Tạo/mở object shared memory có tên (vd: "/my_shared_mem")
ftruncate()	Set kích thước vùng nhớ (bắt buộc sau shm_open)
mmap()		Ánh xạ shared memory vào RAM tiến trình để có con trỏ đọc/ghi
munmap()	Gỡ ánh xạ khi không dùng nữa
shm_unlink()	Xóa shared memory khỏi hệ thống kernel


Semaphore (đồng bộ)
Hàm		Chức năng
sem_open()	Tạo/mở semaphore có tên (để nhiều tiến trình cùng dùng)
sem_wait()	Consumer chờ producer; block nếu semaphore = 0
sem_post()	Producer báo “đã ghi xong”, tăng semaphore
sem_close()	Đóng handle semaphore trong tiến trình
sem_unlink()	Xóa tên semaphore khỏi kernel khi dùng xong

##Luồng hoạt động tóm tắt
Producer tạo shared memory → mmap → ghi struct
Producer sem_post() để báo dữ liệu sẵn sàng
Consumer sem_wait() → đọc shared memory
Consumer unlink shared memory + semaphore (cleanup)

##Mục đích bài này
Hiểu rõ shared memory là IPC nhanh nhất trong Linux
Biết sử dụng semaphore để tránh race-condition
Nắm quy trình: tạo → ghi → đồng bộ → đọc → cleanup

#Author: Nguyen Vu Nhat Thanh - Thomas
