# MNV-IoT ESP32 SDK

[![Version](https://img.shields.io/badge/version-1.1.2-blue.svg)](https://github.com/kayandk/mnv-iot-sdk)
[![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)](https://espressif.com/en/products/socs/esp32)

SDK chuyên dụng cho việc kết nối các thiết bị ESP32 vào hệ sinh thái **MNV-IoT**. Hỗ trợ cơ chế xác thực bảo mật qua JWT, quản lý kết nối MQTT thông minh và tích hợp sẵn các tính năng giám sát (LWT) cũng như điều khiển thời gian thực (RPC).

## Tính năng chính

- 🔒 **Bảo mật**: Xác thực thiết bị qua HTTPS/SSL, không lưu mật khẩu MQTT trực tiếp.
- 📡 **MQTT JWT**: Tự động lấy và quản lý Token JWT để kết nối MQTT an toàn.
- 🚀 **RPC (Remote Procedure Call)**: Hỗ trợ điều khiển thiết bị phản hồi tức thì từ Dashboard.
- 💓 **Smart Heartbeat & LWT**: Theo trạng thái Online/Offline thời gian thực.
- 📊 **Telemetry**: Cấu trúc dữ liệu chuẩn hóa, tương thích hoàn toàn với MNV-IoT Backend.

## Cài đặt

### Arduino IDE
1. Tải thư mục `mnv-iot-sdk` về máy.
2. Sao chép vào thư mục `libraries` của Arduino (thường ở `Documents/Arduino/libraries`).
3. Khởi động lại Arduino IDE.

### PlatformIO
Thêm vào file `platformio.ini`:
```ini
lib_deps =
    ArduinoJson
    PubSubClient
    https://github.com/kayandk/mnv-iot-sdk
```

## Hướng dẫn sử dụng nhanh

```cpp
#include <mnv-iot-sdk/IoTClient.h>

const char* apiBaseUrl = "https://your-api.com";
const char* deviceId = "YOUR_DEVICE_ID";
const char* deviceSecret = "YOUR_DEVICE_SECRET";

mnv::IoTClient client(apiBaseUrl);

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASS");

    // Khởi tạo SDK
    client.begin(deviceId, deviceSecret);
}

void loop() {
    if (!client.isConnected()) {
        client.connect(); // Tự động Auth + Connect MQTT
    }
    client.loop();

    // Gửi dữ liệu cảm biến mỗi 10 giây
    static uint32_t last = 0;
    if (millis() - last > 10000) {
        last = millis();
        client.publishTelemetry("{\"temp\": 25.5}");
    }
}
```

## API Reference

### `IoTClient`
- `begin(deviceId, secret)`: Khởi tạo thông tin thiết bị.
- `connect()`: Thực hiện xác thực và kết nối MQTT. 
- `publishTelemetry(json)`: Gửi dữ liệu cảm biến (nhiệt độ, độ ẩm...).
- `publishAttributes(json)`: Gửi thuộc tính hệ thống (IP, RSSI, Version...).
- `onCommand(callback)`: Đăng ký nhận lệnh điều khiển hoặc yêu cầu RPC từ Dashboard.
- `publishRpcResponse(json)`: Phản hồi lại kết quả của lệnh RPC.

## Giấy phép
Bản quyền thuộc về **MNV Team**. Liên hệ [support@mnv.vn](mailto:support@mnv.vn) để biết thêm chi tiết.
