# 🧩 **Bài 4 — Linux Thread**

## 🎯 **Mục tiêu**
Viết các chương trình minh họa việc **tạo và quản lý luồng (thread)** trong Linux, sử dụng thư viện **POSIX Threads (pthread)**.  
Các bài thực hành giúp hiểu cơ chế **đồng bộ hóa, tránh race condition**, và **tối ưu truy cập tài nguyên**.

---

## 📚 **Yêu cầu thực hành**

Thực hiện 4 bài tập nhỏ trong chủ đề **Linux Thread**:

### 🧵 **Bài 1 – Tạo và chạy luồng**
- Tạo 2 luồng bằng `pthread_create()`.
- Mỗi luồng in ra ID của chính nó.
- Dùng `pthread_join()` để main chờ các luồng kết thúc.

### 🔒 **Bài 2 – Race Condition & Mutex**
- Dùng 3 luồng tăng biến toàn cục `counter` 1,000,000 lần.
- Sử dụng `pthread_mutex_t` để đảm bảo chỉ một luồng truy cập tại một thời điểm.
- So sánh kết quả khi có và không có mutex.

### 🔄 **Bài 3 – Producer – Consumer (Condition Variable)**
- Tạo 2 luồng: `Producer` sinh dữ liệu, `Consumer` đọc dữ liệu.
- Dùng `pthread_cond_t` kết hợp `pthread_mutex_t` để đồng bộ hóa.
- Tránh việc “polling” (bận chờ).

### 📖 **Bài 4 – Read–Write Lock**
- Tạo 5 Reader và 2 Writer.
- Dùng `pthread_rwlock_t` để cho phép nhiều Reader cùng đọc, nhưng chỉ một Writer được ghi.
- Quan sát cách các luồng được lập lịch và đồng bộ.

---

## 🧠 **Kiến thức áp dụng**

- Thư viện **`<pthread.h>`**.  
- Hàm tạo và quản lý luồng:
  - `pthread_create()`, `pthread_join()`, `pthread_self()`, `pthread_exit()`
- Cơ chế **đồng bộ hóa**:
  - `pthread_mutex_lock()`, `pthread_mutex_unlock()`
  - `pthread_cond_wait()`, `pthread_cond_signal()`
  - `pthread_rwlock_rdlock()`, `pthread_rwlock_wrlock()`, `pthread_rwlock_unlock()`
- Khái niệm:
  - *Critical section*, *race condition*, *blocking*, *sleeping threads*.
- Hiểu hoạt động của **scheduler** trong hệ thống Linux.

---

## 🧰 **Cấu trúc thư mục**
04_Linux_Thread/
├── thread_basic.c
├── thread_mutex.c
├── thread_condition.c
├── thread_rwlock.c
└── README.md


👨‍💻 Người thực hiện : Nguyễn Vũ Nhật Thành
