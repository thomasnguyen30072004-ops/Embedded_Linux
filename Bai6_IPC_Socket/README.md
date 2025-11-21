📘 IPC Socket trong Embedded Linux


1. Giới thiệu

Trong hệ thống Embedded Linux, các tiến trình (process) thường phải trao đổi dữ liệu với nhau hoặc với thiết bị ngoại vi. Linux cung cấp nhiều cơ chế IPC như:

Pipes

Message Queue

Shared Memory

Signals

Sockets (mạnh nhất và linh hoạt nhất)

Trong đó, Socket IPC đặc biệt quan trọng vì:

Cho phép giao tiếp trong cùng máy (Unix Domain Socket)

Hoặc qua mạng (IPv4 TCP/UDP)

Hỗ trợ truyền dữ liệu dạng stream (liên kết, đáng tin cậy)

Hoặc datagram (không liên kết, nhanh, không đảm bảo)**

Thích hợp cho mọi ứng dụng trong thiết bị nhúng, từ giao tiếp giữa daemon - service, tới client–server qua LAN/WiFi.

Dự án này triển khai 4 mô hình socket cơ bản và quan trọng nhất dùng trong embedded:

IPv4 Stream (TCP)

IPv4 Datagram (UDP)

Unix Domain Stream

Unix Domain Datagram

2. Mô hình đã triển khai
🟦 1. IPv4 Stream Socket (TCP)

Domain: AF_INET

Type: SOCK_STREAM

Đảm bảo thứ tự, có bắt tay, có kết nối

Dùng cho giao tiếp tin cậy giữa device ↔ server/cloud

File:

ipv4_stream_server.c

ipv4_stream_client.c

🟩 2. IPv4 Datagram Socket (UDP)

Domain: AF_INET

Type: SOCK_DGRAM

Không kết nối, không đảm bảo, tốc độ cao

Phù hợp truyền telemetry (nhiệt độ, độ ẩm, sensor data)

File:

ipv4_dgram_server.c

ipv4_dgram_client.c

🟧 3. Unix Domain Stream Socket

Domain: AF_UNIX

Type: SOCK_STREAM

Dùng trong nội bộ hệ thống Linux

Tốc độ nhanh hơn TCP vì không qua network stack

Dùng trong: systemd, DBus, daemon nội bộ thiết bị

File:

unix_stream_server.c

unix_stream_client.c

🟪 4. Unix Domain Datagram Socket

Domain: AF_UNIX

Type: SOCK_DGRAM

Nhẹ hơn stream, message-based

Cho phép nhiều client gửi về 1 server

File:

unix_dgram_server.c

unix_dgram_client.c

3. Kiến trúc tổng quan
                +---------------------------+
                |       Embedded Linux      |
                |      (User Space IPC)     |
                +---------------------------+

   IPv4 TCP (stream)              IPv4 UDP (datagram)
 ┌────────────────────┐        ┌────────────────────┐
 │ ipv4_stream_server │        │ ipv4_dgram_server  │
 └───────────▲────────┘        └──────────▲────────┘
             │ TCP                             │ UDP
 ┌───────────┴────────┐        ┌────────────┴───────┐
 │ ipv4_stream_client │        │ ipv4_dgram_client  │
 └────────────────────┘        └────────────────────┘


   Unix Stream                        Unix Datagram
 ┌────────────────────┐        ┌───────────────────────┐
 │ unix_stream_server │        │ unix_dgram_server     │
 └──────────▲─────────┘        └────────────▲──────────┘
            │ AF_UNIX Stream                      │ AF_UNIX Dgram
 ┌──────────┴────────┐        ┌────────────────────┴───────┐
 │ unix_stream_client│        │ unix_dgram_client           │
 └────────────────────┘        └────────────────────────────┘

4. Kiến thức kỹ thuật quan trọng
Socket gồm 3 thông số quan trọng
Tham số	Giá trị mẫu	Ý nghĩa
Domain	AF_INET, AF_UNIX	Không gian giao tiếp
Type	SOCK_STREAM, SOCK_DGRAM	Kiểu dữ liệu truyền
Protocol	0 (mặc định)	TCP/UDP tùy type
Các hàm hệ thống chính

socket() – tạo socket

bind() – gán địa chỉ (IP/Port hoặc file path)

listen() – server lắng nghe kết nối (stream)

accept() – nhận kết nối từ client

connect() – client kết nối đến server

send(), recv() – truyền stream

sendto(), recvfrom() – truyền datagram

close() – đóng socket

Unix Domain Socket → luôn cần
unlink("/tmp/socket_path.sock");


Nếu không, server sẽ lỗi "Address already in use".

5. Build file
5.1 Build IPv4 Stream
gcc ipv4_stream_server.c -o ipv4_stream_server
gcc ipv4_stream_client.c -o ipv4_stream_client

5.2 Build IPv4 Datagram
gcc ipv4_dgram_server.c -o ipv4_dgram_server
gcc ipv4_dgram_client.c -o ipv4_dgram_client

5.3 Build Unix Domain Stream
gcc unix_stream_server.c -o unix_stream_server
gcc unix_stream_client.c -o unix_stream_client

5.4 Build Unix Datagram
gcc unix_dgram_server.c -o unix_dgram_server
gcc unix_dgram_client.c -o unix_dgram_client

6. Cách chạy chương trình
IPv4 Stream (TCP)

Server:

./ipv4_stream_server


Client:

./ipv4_stream_client "Hello TCP"

IPv4 Datagram (UDP)

Server:

./ipv4_dgram_server


Client:

./ipv4_dgram_client "Hello UDP"

Unix Stream

Server:

./unix_stream_server


Client:

./unix_stream_client "Hello Unix Stream"

Unix Datagram

Server:

./unix_dgram_server


Client:

./unix_dgram_client "Hello Unix Dgram"

Author: Nguyễn Vũ Nhật Thành 📘
