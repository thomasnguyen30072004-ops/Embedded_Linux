# IPC Message Queue: Priority-Based Task Dispatcher – Worker System

# 🧩 1. Giới thiệu
Bài tập mô phỏng hệ thống **Task Processing** sử dụng **POSIX Message Queue** để giao tiếp giữa các tiến trình độc lập trong Linux.

Hệ thống bao gồm:
- **Dispatcher (Client)**: gửi các tác vụ (task) vào hàng đợi.
- **Worker (Server)**: nhận và xử lý các task từ hàng đợi theo **độ ưu tiên (priority)**.

👉 Bài toán dựa trên mô hình **Client – Server IPC** với **priority scheduling**, thường gặp trong:
- Job scheduler
- Daemon xử lý tác vụ
- Embedded Linux multi-process systems

---

# 🧱 2. Kiến trúc chương trình

Chương trình được chia thành **2 file thực thi độc lập**:

- `task_worker.c`  
  → Tiến trình Worker  
  → Tạo và quản lý POSIX Message Queue  
  → Nhận và xử lý các task

- `task_dispatcher.c`  
  → Tiến trình Dispatcher (Client)  
  → Gửi task vào queue với priority tương ứng

📌 Hai tiến trình chạy **độc lập**, có thể nằm ở **hai terminal khác nhau** hoặc được spawn từ các process khác.

---

# 📦 3. POSIX Message Queue sử dụng

## 3.1 Thông tin queue
- Tên queue: /my_task_queue

- Thuộc tính queue:
- `mq_maxmsg = 10`  → tối đa 10 message trong queue
- `mq_msgsize = 1024` → kích thước tối đa mỗi message

## 3.2 Cơ chế priority
- Priority là **unsigned int**
- **Priority càng lớn → ưu tiên càng cao**
- `mq_receive()` luôn trả về message có priority cao nhất
- Nếu nhiều message cùng priority → FIFO

---

# 🔁 4. Hành vi của các tiến trình

## 4.1 Worker – `task_worker`

Vai trò:
- Tạo (hoặc mở) message queue
- Lắng nghe và xử lý task liên tục
- Thu hồi tài nguyên khi nhận `SIGINT (Ctrl+C)`

Hành vi chi tiết:
1. Gọi `mq_open()` với `O_CREAT | O_RDONLY`
2. Thiết lập signal handler cho `SIGINT`
3. Vào vòng lặp `while`
4. Chờ task bằng `mq_receive()` (blocking)
5. Nhận task có priority cao nhất
6. In nội dung task + priority
7. Giả lập xử lý bằng `sleep(1)`
8. Khi Ctrl+C → đóng queue và `mq_unlink()`

---

## 4.2 Dispatcher – `task_dispatcher`

Vai trò:
- Gửi task vào message queue
- Gán độ ưu tiên cho mỗi task

Cú pháp chạy:
./task_dispatcher <priority> "<task_description>"

Ví dụ:
./task_dispatcher 30 "CRITICAL: Restart web server"
./task_dispatcher 10 "Update weekly report"
./task_dispatcher 20 "Backup user database"


Hành vi chi tiết:
1. Parse priority từ command-line
2. Ghép nội dung task thành 1 chuỗi
3. Mở queue bằng `mq_open(O_WRONLY)`
4. Gửi task bằng `mq_send(message, priority)`
5. Thoát chương trình

---

# ⚠️ 5. Lưu ý quan trọng về Priority

Priority **không đảo ngược những message đã bị lấy khỏi queue**.

✅ Priority chỉ có ý nghĩa khi:
- **Nhiều message tồn tại trong queue tại thời điểm worker gọi `mq_receive()`**

❌ Nếu worker đã lấy message priority thấp **trước khi** message priority cao được gửi → thứ tự hiển thị phụ thuộc vào timing, **không phải lỗi**.

---

# 🚀 6. Cách build chương trình

gcc -o task_worker task_worker.c
gcc -o task_dispatcher task_dispatcher.c

# ▶️ 7. Cách chạy chương trình

## Bước 1: Chạy worker (Terminal 1)
./task_worker


## Bước 2: Gửi task từ dispatcher (Terminal 2)
./task_dispatcher 10 "Hello Thanh"
./task_dispatcher 30 "CRITICAL: Restart web server"
./task_dispatcher 20 "Backup user database"


Worker sẽ nhận và xử lý các task.

---

# 💡 8. Kiến thức rút ra từ bài này

- Cách sử dụng POSIX Message Queue trong Linux
- Hiểu cơ chế:
  - Blocking receive
  - Priority-based IPC
- Biết cách:
  - Thiết kế IPC theo mô hình Client–Server
  - Quản lý tài nguyên IPC an toàn (`mq_close`, `mq_unlink`)
- Áp dụng cho Embedded Linux và hệ thống đa tiến trình

---

# 👨‍💻 Author: Nguyen Vu Nhat Thanh


