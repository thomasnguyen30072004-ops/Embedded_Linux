# 🧩 Bài 2 — Linux File System

## 🎯 Mục tiêu
Viết chương trình **`filestat`** sử dụng hàm **`lstat()`** để lấy và hiển thị thông tin chi tiết của một tệp trong hệ thống Linux.

## 📚 Yêu cầu đề bài
Chương trình `filestat` phải:
1. Nhận đường dẫn tệp qua dòng lệnh (`argv[1]`).
2. Sử dụng `lstat()` để lấy thông tin tệp.
3. In ra ít nhất các thông tin sau:
   - Đường dẫn tệp.
   - Loại tệp (Regular file, Directory, Symbolic link, ...).
   - Kích thước (bytes).
   - Thời gian chỉnh sửa cuối cùng (Last modified time).

## 🧠 Kiến thức áp dụng
- Cấu trúc dữ liệu **`struct stat`** trong thư viện `<sys/stat.h>`.
- Các macro kiểm tra loại tệp:
  ```c
  S_ISREG(mode), S_ISDIR(mode), S_ISLNK(mode)
  
**Hàm thời gian:**
localtime(), strftime(), ctime()
Quyền truy cập tệp, UID/GID, symbolic link (hàm readlink()).

**🧰 Cấu trúc thư mục**
02_Linux_File_System/
├── filestat.c
└── README.md

**⚙️ Cách biên dịch**
gcc -Wall -Wextra -o filestat filestat.c

**▶️ Cách chạy thử**
./filestat /etc/passwd
./filestat /home
./filestat mylink

**Ví dụ kết quả:**
File Path: /etc/passwd
File Type: Regular File
Size: 2471 bytes
Last Modified: 2025-10-20 09:13:45
Hard Link Count: 1
Owner: root (uid=0)
Group: root (gid=0)
Permissions: -rw-r--r--


👨‍💻 Người thực hiện: Nguyễn Vũ Nhật Thành
