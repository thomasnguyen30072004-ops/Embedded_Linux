# **strutils_project** — Static & Shared Library in C

> 🚀 A complete example showing how to build, link, and use **static (.a)** and **shared (.so)** libraries in C, featuring simple string manipulation utilities.  


## 🧠 **Tổng quan**

Dự án này minh họa cách:
- Tạo và biên dịch **Static Library (`.a`)** và **Shared Library (`.so`)**
- Liên kết chúng với chương trình C (`main.c`)
- Hiểu rõ sự khác biệt giữa **static linking** và **dynamic linking** trên Linux

Thư viện mẫu `strutils` cung cấp một số hàm xử lý chuỗi cơ bản.


## 📁 **Cấu trúc dự án**

strutils_project/
- libstrutils.a        # Static library
- libstrutils.so       # Shared library
- main.c               # Example program using the library
- main_static          # Executable linked statically
- main_shared          # Executable linked dynamically
- Makefile             # Build automation file
- README.md            # Project documentation
- strutils.c           # Implementation of string utility functions
- strutils.h           # Header file with function declarations
- trutils.o            # Compiled object file
🧱 Các hàm trong thư viện strutils
Hàm	Mô tả
void str_trim(char *str)	Xóa khoảng trắng đầu và cuối chuỗi.
void str_reverse(char *str)	Đảo ngược chuỗi ký tự in-place.
int str_to_int(const char *s)	Chuyển chuỗi số sang kiểu int.
void str_copy_safe(char *dest, const char *src, int max_len)	Sao chép chuỗi an toàn, tránh tràn bộ nhớ.

⚙️ Hướng dẫn biên dịch và chạy
1️⃣ Clone repository
- git clone https://github.com/thomasnguyen30072004/strutils_project.git
- cd strutils_project
- Sau khi build xong, ta sẽ có:

libstrutils.a      # Static library
libstrutils.so     # Shared library
main_static        # Chương trình link với static lib
main_shared        # Chương trình link với shared lib

2️⃣ Biên dịch chương trình
- make clean
- make all

3️⃣ Chạy chương trình link tĩnh
- ./main_static
4️⃣ Chạy chương trình link động
- Vì main_shared sử dụng thư viện .so, nên cần thêm đường dẫn thư viện vào biến môi trường:
- export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)
- ./main_shared

🧩 Ví dụ kết quả chạy
- Trimmed: 'Hello World'
- Reversed: 'dlroW olleH'
- String to int: 12345
- Copied safely: 'Hello wor'

🧹 Các lệnh tiện ích
Dọn dẹp toàn bộ file biên dịch:
- make clean
- Chỉ build static library:
- make static
- Chỉ build shared library:
make shared

🧰 Cấu trúc Makefile (mô tả nhanh)
Makefile tự động hóa quá trình build cho cả hai loại thư viện:
Makefile
all: static shared main_static main_shared

static:
	gcc -c strutils.c -o strutils.o
	ar rcs libstrutils.a strutils.o

shared:
	gcc -fPIC -c strutils.c -o strutils.o
	gcc -shared -o libstrutils.so strutils.o

main_static:
	gcc main.c -L. -lstrutils -o main_static

main_shared:
	gcc main.c -L. -lstrutils -o main_shared


🧑‍💻 Tác giả
Nguyễn Vũ Nhật Thành
📧 Email: thomasnguyen30072004@gmail.com


