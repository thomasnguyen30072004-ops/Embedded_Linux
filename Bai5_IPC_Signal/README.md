# IPC Signal in Linux 

## 1. Giới thiệu

IPC (Inter-Process Communication) bằng **Signal** là cơ chế bất đồng bộ lâu đời trong hệ điều hành Unix/Linux, cho phép kernel gửi thông báo tới tiến trình về một sự kiện xảy ra. Tín hiệu hoạt động như **software interrupt**, làm gián đoạn luồng thực thi hiện tại và chuyển điều khiển sang hàm xử lý tín hiệu (signal handler).

Tài liệu này cung cấp phân tích kỹ thuật sâu, bao gồm:

* Cơ chế hoạt động của signal bên trong kernel
* Vòng đời một signal
* Các system call quan trọng: `signal()`, `sigaction()`, `kill()`, `alarm()`, `sigprocmask()` …
* Mô tả từng loại signal thường gặp
* Giải thích từng biến & handler trong code
* Ứng dụng trong thực tế và an toàn khi lập trình signal

---

## 2. Vòng đời của một Signal (Signal Lifecycle)

Một signal trải qua ba giai đoạn chính:

### **2.1. Generation (Tạo tín hiệu)**

Signal có thể được tạo ra bởi:

* Kernel (vd: chia 0 → `SIGFPE`, truy cập sai bộ nhớ → `SIGSEGV`)
* Người dùng (vd: Ctrl+C → `SIGINT`)
* Process khác (vd: `kill(pid, SIGUSR1)`)
* Chính process tự gửi cho nó (`raise(SIGINT)`)

### **2.2. Delivery (Chuyển tới process)**

Signal được đặt vào **pending queue** của process.
Nếu signal đó nằm trong **signal mask** (bị block), nó sẽ chờ.

### **2.3. Processing (Xử lý)**

Có ba cách xử lý signal:

1. **Default action**
2. **Ignore**
3. **Catch bằng signal handler**

Một số tín hiệu *không thể bị catch hay ignore*:

* `SIGKILL`
* `SIGSTOP`

---

## 3. Bảng Signal Quan Trọng & Mặc Định Xử Lý

| Signal    | Mã số | Ý nghĩa                | Hành động mặc định  |
| --------- | ----- | ---------------------- | ------------------- |
| `SIGINT`  | 2     | Ctrl+C                 | Terminate           |
| `SIGTERM` | 15    | Yêu cầu dừng process   | Terminate           |
| `SIGKILL` | 9     | Buộc dừng ngay lập tức | Cannot catch/ignore |
| `SIGSTOP` | 19    | Dừng process           | Cannot catch/ignore |
| `SIGCHLD` | 17    | Con dừng/chết          | Ignore              |
| `SIGUSR1` | 10    | User-defined 1         | Terminate           |
| `SIGUSR2` | 12    | User-defined 2         | Terminate           |
| `SIGALRM` | 14    | `alarm()` timeout      | Terminate           |
| `SIGSEGV` | 11    | Lỗi truy cập bộ nhớ    | Core dump           |
| `SIGFPE`  | 8     | Chia 0, lỗi toán học   | Core dump           |

---

## 4. Các System Call Quan Trọng Trong IPC Signal

### **4.1. `signal(int signo, sighandler_t handler)`**

Đăng ký hàm xử lý tín hiệu.

* `signo`: mã tín hiệu (SIGINT…)
* `handler`: địa chỉ hàm xử lý, hoặc `SIG_IGN`, `SIG_DFL`

❗ **Không an toàn hoàn toàn (incomplete async-signal-safe)** → nên dùng `sigaction()` thay thế.

---

### **4.2. `sigaction(int signo, const struct sigaction *act, struct sigaction *oldact)`**

Phiên bản nâng cấp của `signal()`, hỗ trợ thêm:

* cờ (flags)
* chặn tạm thời các tín hiệu khác khi đang xử lý
* sử dụng `sa_mask`, `sa_flags`

❗ Đây là **cách khuyến nghị** để cài đặt signal handler.

---

### **4.3. `kill(pid_t pid, int signo)`**

Gửi tín hiệu tới process khác hoặc chính nó.

* `pid > 0`: gửi đến process có PID
* `pid = 0`: gửi tới toàn bộ process trong nhóm
* `pid = -1`: gửi đến tất cả process trừ init
* `pid < -1`: gửi đến group id = |pid|

Không phải để *kill* process, mà để *send signal*.

---

### **4.4. `alarm(unsigned int seconds)`**

Đặt hẹn giờ. Khi hết thời gian → kernel gửi `SIGALRM`.

Nếu gọi lại `alarm(1)` trong handler → tạo timer lặp.

---

### **4.5. Bộ hàm xử lý signal mask**

```c
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigismember(const sigset_t *set, int signo);
int sigprocmask(int how, const sigset_t *newset, sigset_t *oldset);
```

Dùng để block/unblock signal.

**Các mode của sigprocmask():**

* `SIG_BLOCK` → thêm tín hiệu vào mask
* `SIG_UNBLOCK` → gỡ tín hiệu khỏi mask
* `SIG_SETMASK` → thay mask hiện tại bằng mask mới

---

## 5. Giải Thích Chi Tiết Các Biến & Hàm Trong Code

### **5.1. Biến `signo` trong handler**

```c
void handler(int signo);
```

* `signo` chứa mã tín hiệu được kích hoạt.
* Dùng để phân biệt nhiều tín hiệu trong một handler.

### **5.2. Biến `handler` khi đăng ký signal**

```c
signal(SIGINT, handler);
```

`handler` là con trỏ hàm.
Kernel sẽ nhảy đến hàm này khi signal được xử lý.

### **5.3. Biến `pid` trong giao tiếp process**

Dùng khi gửi tín hiệu bằng `kill()`.

### **5.4. Biến đếm toàn cục (global counter)**

`SIGINT`.

* Signal handler **không được truyền tham số**
* Signal handler chạy độc lập với main thread

### **5.5. `pause()`**

Dừng process cho đến khi có signal đến.
Không tiêu tốn CPU.

---

## 6. Quy Tắc An Toàn Khi Dùng Signal Handler (RẤT QUAN TRỌNG)

Bên trong handler **chỉ được** gọi hàm thuộc nhóm **async-signal-safe**, ví dụ:

* `_exit()`
* `write()`
* `signal()`
* `kill()`

---

## 7. Mô Hình Stack Khi Xảy Ra Signal

1. Kernel ngắt main thread
2. Push ngữ cảnh hiện tại xuống stack
3. Jump vào signal handler
4. Khi handler kết thúc → kernel khôi phục stack → quay lại nơi bị ngắt

---

## 8. Tóm Tắt Các Hàm IPC Signal

### **Danh sách đầy đủ:**

* `signal()` — đăng ký handler đơn giản
* `sigaction()` — đăng ký handler nâng cao (khuyến nghị)
* `kill()` — gửi tín hiệu đến process
* `raise()` — process tự gửi tín hiệu cho chính nó
* `alarm()` — hẹn giờ gửi SIGALRM
* `pause()` — chờ tín hiệu
* `sigemptyset()` — tạo mask rỗng
* `sigfillset()` — mask chứa tất cả tín hiệu
* `sigaddset()` — thêm một tín hiệu vào mask
* `sigdelset()` — xóa tín hiệu khỏi mask
* `sigprocmask()` — block/unblock tín hiệu

---

## 9. Kết Luận

Signal là công cụ cực mạnh nhưng cần dùng đúng cách để tránh deadlock, race condition hoặc crash ứng dụng.

Nếu dùng đúng, signal có thể:

* Giao tiếp process hiệu quả
* Tạo timer nhẹ
* Điều khiển process con
* Xử lý sự kiện bất đồng bộ trong hệ thống nhúng Linux
