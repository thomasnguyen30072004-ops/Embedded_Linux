📘 README – IPC PIPE & FIFO (Inter-Process Communication on Linux)
🔧 Giới thiệu bài học

Bài thực hành này thuộc môn Embedded Linux / Hệ điều hành Linux, mục tiêu là nắm vững cách các tiến trình giao tiếp với nhau bằng:

PIPE (Unnamed Pipe) – kênh giao tiếp cha–con

FIFO (Named Pipe) – kênh giao tiếp giữa các tiến trình độc lập

Bài này sẽ triển khai:

Mô phỏng toán tử ls -l | wc -l bằng C, sử dụng pipe(), fork(), dup2(), execlp().

Mô hình Client – Server IPC dùng FIFO:

Client gửi request đến Server

Server trả response lại Client qua FIFO riêng

Hỗ trợ nhiều client cùng lúc

Toàn bộ mã nguồn được comment line-by-line để người đọc theo dõi dễ dàng.

🧩 Phần 1 – Mô phỏng pipeline ls -l | wc -l
🎯 Mục tiêu

Tạo ra chương trình C mô phỏng:

ls -l | wc -l


Cơ chế:

Tiến trình cha tạo pipe

Tạo 2 tiến trình con:

Con 1: thực thi ls -l, ghi output vào đầu ghi của pipe

Con 2: thực thi wc -l, đọc input từ đầu đọc của pipe

Cha đóng cả hai đầu để tránh treo (wc chờ EOF)

Dùng dup2() để redirect stdin/stdout


📄 File: mypipe.c

Chức năng:

Tạo pipe

fork() tạo con 1 → chạy ls -l

fork() tạo con 2 → chạy wc -l


Dùng dup2 để chuyển hướng dữ liệu

Cha đợi 2 tiến trình kết thúc

▶️ Cách build & run
gcc mypipe.c -o mypipe
./mypipe

✔ Kết quả mong đợi

Một số nguyên = số dòng file trong directory (giống ls -l | wc -l).

🧩 Phần 2 – FIFO (Named Pipe) Client–Server

Phần này triển khai mô hình IPC giữa các tiến trình không có quan hệ cha–con.

🔥 Mô hình hoạt động
      CLIENT                          SERVER
-------------------          --------------------------
Tạo FIFO riêng (/tmp)        |
Ghi request → SERVER_FIFO -> | Đọc request từ SERVER_FIFO
Mở FIFO riêng để đọc         |
<- Đọc response từ server ---| Ghi response vào FIFO client

Thành phần dữ liệu (struct truyền qua FIFO)
typedef struct {
    pid_t pid;                 // PID client
    char client_fifo[64];      // FIFO riêng của client
    char message[128];         // Message cần gửi
} request_t;


📄 Phần 2.1 – Server: server_fifo_reply.c

Server phải:

Tạo SERVER_FIFO

Mở để đọc request từ mọi client

Khi nhận request:

Mở FIFO riêng của client

Gửi response

Luôn chạy trong vòng lặp

Dọn dẹp FIFO khi tắt

Cách build:
gcc server_fifo_reply.c -o server

Cách chạy:
./server

📄 Phần 2.2 – Client: client_fifo_reply.c

Client phải:

Lấy PID để tạo FIFO riêng

Tạo request gồm:

PID

Tên FIFO riêng

Message người dùng nhập

Mở SERVER_FIFO ở chế độ ghi

Gửi request

Mở FIFO riêng ở chế độ đọc để nhận phản hồi

In phản hồi và xóa FIFO riêng

Build:
gcc client_fifo_reply.c -o client

Run:
./client "Hello server"

🔍 Luồng hoạt động chi tiết (để báo cáo hoặc hiểu sâu)
1) Server start trước → mở SERVER_FIFO (O_RDONLY)

Block nếu chưa có process nào mở để ghi

Khi client chạy, open(O_WRONLY) → cả 2 bên thông nhau

Server read() → nhận được request

2) Client chạy → tạo FIFO riêng /tmp/client_fifo_PID

Gửi request vào SERVER_FIFO

Mở FIFO riêng của nó để đọc (block chờ server)

3) Server xử lý request

Mở FIFO riêng của client ở O_WRONLY

Gửi response

Đóng FIFO client

4) Client thoát sau khi đọc xong

unlink(FIFO riêng)

✔ Kết luận

→ Server có thể phục vụ nhiều client tuần tự
→ Không cần quan hệ cha–con
→ Named pipe cố định, nên nhiều tiến trình có thể connect

Author: Nguyen Vu Nhat Thanh
