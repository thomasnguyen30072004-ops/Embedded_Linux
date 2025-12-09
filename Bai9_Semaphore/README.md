#IPC Semaphore: Multi-Producer Multi-Consumer Restaurant Simulation

#🧩 1. Giới thiệu
-> Bài tập mô phỏng hệ thống “Nhà hàng” sử dụng POSIX Shared Memory (shm_open + mmap) và POSIX Named Semaphores (sem_open) để đồng bộ hóa nhiều tiến trình trong mô hình:
- 2 Producer (Đầu bếp)
- 3 Consumer (Khách hàng)
- Launcher: tiến trình cha chịu trách nhiệm tạo shared memory, semaphore và exec các process con.
-> Bài toán dựa trên mô hình kinh điển: Multiple Producer – Multiple Consumer Bounded Buffer
Trong bài này, mỗi loại mâm (Tray Vegan / Tray Non-Vegan) là một bounded buffer size 10.

#🧱 2. Kiến trúc chương trình
- Chương trình được chia thành 5 file thực thi độc lập:
-> launcher: Tạo shared memory, semaphore, và chạy các tiến trình khác
-> producer: Tiến trình đầu bếp (vegan hoặc nonvegan tùy argv)
-> consumer: Tiến trình khách hàng (vegan / nonvegan / hybrid tùy argv)
-> shared.h: Khai báo struct shared memory, constants, semaphore names
-> Makefile: Tự động build toàn bộ chương trình

#📦 Shared Memory chứa:
- tray_vegan[10]
- tray_nonvegan[10]
-> Mỗi ô có giá trị:
- 0 = trống
- 1 = món chay
- 2 = món không chay

#🔐 3. Semaphore sử dụng
Mỗi mâm có 3 semaphore:
- mutex_*: Binary semaphore bảo vệ truy cập critical section
- empty_*: Counting semaphore, số ô trống
- full_*: Counting semaphore, số ô đã có món

-> Tên semaphore (POSIX named):
- /devlinux_mutex_vegan
- /devlinux_empty_vegan
- /devlinux_full_vegan
- /devlinux_mutex_nonveg
- /devlinux_empty_nonveg
- /devlinux_full_nonveg

#4. Hành vi của các tiến trình
4.1 Producer – Đầu bếp
-> Mỗi đầu bếp chỉ nấu 2 món đặc trưng.

- Chef Donatello (Non-Vegan)
-> Fettuccine Chicken Alfredo
-> Garlic Sirloin Steak

- Chef Portecelli (Vegan)
-> Pistachio Pesto Pasta
-> Avocado Fruit Salad

- Hành vi:
1. Chọn ngẫu nhiên 1 trong 2 món.
2. Chờ nếu mâm đầy (sem_wait(empty)).
3. Giành quyền mutex.
4. Đặt món vào ô trống đầu tiên.
5. Trả mutex + tăng full.
6. Nghỉ 1–5 giây.

4.2 Consumer – Khách hàng
- Có 3 loại:
-> Khách 1 → cần non-vegan
-> Khách 2 → cần vegan
-> Khách 3 → hybrid (cả 2 loại mỗi lần một món)

- Hành vi:
1. Chờ nếu không có món phù hợp (sem_wait(full)).
2. Giành mutex.
3. Lấy món từ ô đầu tiên có thức ăn.
4. Trả mutex + tăng empty.
5. Nghỉ 10–15 giây.

#🚀 5. Cách build chương trình
- make

#▶️ 6. Cách chạy chương trình
- Chỉ cần chạy tiến trình cha: ./launcher
-> Launcher sẽ tự động:
1. Tạo shared memory
2. Tạo semaphores
3. Thực hiện exec tạo 5 process con:
./producer vegan
./producer nonvegan
./consumer vegan
./consumer nonvegan
./consumer hybrid

#💡7. Ghi chú quan trọng
- Nếu chạy launcher nhiều lần → cần cleanup semaphore (vì POSIX named sem không tự hủy).
- Đây là mô hình thật trong Linux — áp dụng cho Embedded Systems / OS / Process Synchronization.
- Code đã comment chi tiết line-by-line theo đúng yêu cầu bài lab.

#Author: Nguyen Vu Nhat Thanh
