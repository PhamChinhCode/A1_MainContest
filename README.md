# 🚀 A1 Main Contest – ESP32 Motorbike Testing System

Hệ thống **Điều khiển & Giám sát xe sát hạch A1** được xây dựng trên nền tảng **ESP32 + FreeRTOS**, dùng cho các bài thi sa hình xe máy.  
Dự án bao gồm:

- Giao tiếp WebServer để điều khiển và lấy dữ liệu
- Điều khiển bài thi (Contest Manager)
- Điều khiển phần cứng xe (Hardware Manager)
- Quản lý Motorbike (Encoder, Hall, đèn tín hiệu…)
- Ghi dữ liệu vào MicroSD
- OTA Update
- Queue giao tiếp giữa các task FreeRTOS

---

# 📌 1. Sơ lược chức năng

Dự án hiện thực đầy đủ một hệ thống thi sát hạch:

### ✔ Server WebSocket/HTTP phục vụ ứng dụng PC  
### ✔ Quản lý bài thi sát hạch (Contest 1 → Contest 4)  
### ✔ Tương tác thời gian thực giữa WebServerTask và ContestTask  
### ✔ Điều khiển phần cứng xe:  
- Encoder  
- Cảm biến Hall  
- Tín hiệu trái/phải  
- Động cơ, đèn, còi  
- Ghi log qua Serial  

### ✔ Tải cấu hình bài thi từ MicroSD  
### ✔ Thực hiện bài thi trong task riêng (RunTask)  
### ✔ Gửi dữ liệu song song giữa các task qua Queue FreeRTOS  
### ✔ OTA firmware update  

---

# 📦 2. Các thành phần chính

## 📁 2.1. Module chính

| Module | Chức năng |
|--------|-----------|
| HardwareManager | Quản lý phần cứng xe |
| Motorbike | Lớp đại diện cho xe thi |
| ContestManager | Thuật toán xử lý bài thi |
| NetworkManager | WebServer + giao tiếp |
| FrameConvert | Chuyển đổi frame dữ liệu |
| MicroSDTask | Ghi/đọc file trên SD |
| DateTime | Thời gian thực |
| LogUtil | Ghi log |

---

# ⚙️ 3. Kiến trúc hệ thống FreeRTOS

Hệ thống tạo 3 task:

```
WebServerTask – Core 0  
ContestTask   – Core 1  
RunTask       – Core 1  
```

---

# 🔗 4. Queue giao tiếp

Hai queue chính:

```
queueServer  
queueContest
```

---

# 📝 5. Cấu hình bài thi

Các file contest được ghi lên SD lần đầu bằng `firstSetup()`.

---

# 📁 6. Cấu trúc thư mục

```
A1_MainContest/
│── src/
│── lib/
│── include/
│── platformio.ini
```

---

# 🛠 7. Build bằng PlatformIO

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps =
    bblanchon/ArduinoJson @ ^7.0.0
```

---

# 🎯 8. Mục tiêu hệ thống

- Ổn định  
- Phản hồi nhanh  
- Chính xác bài thi  
- Dễ mở rộng  

