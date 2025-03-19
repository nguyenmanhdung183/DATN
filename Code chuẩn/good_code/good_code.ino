#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"  // Thư viện để lấy thời gian từ Internet

// Thông tin WiFi của bạn
#define WIFI_SSID "realme"
#define WIFI_PASSWORD "123456789@"
#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"

// Thông tin Firebase
#define FIREBASE_HOST "https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "TOpAxHzBbyQz0WatMvL0WUwC4a8EHPFogT0j0i1Y"  

// Thiết bị
#define DEVICE_ID "esp32w2"

#define LED 18
#define BUTTON 19

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Cấu hình NTP
const char* ntpServer = "pool.ntp.org";  
const long  gmtOffset_sec = 7 * 3600; // GMT+7 (Việt Nam)
const int   daylightOffset_sec = 0;   

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    // Kết nối WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Đang kết nối WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n✅ Đã kết nối WiFi!");

    // Đồng bộ thời gian qua NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("⏳ Đang đồng bộ thời gian...");
    delay(2000); // Chờ lấy dữ liệu thời gian từ NTP

    // Cấu hình Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Ghi dữ liệu lên Firebase
    if (Firebase.RTDB.setString(&fbdo, "/room1/device1/status", "on")) {
        Serial.println("Cập nhật thành công!");
    } else {
        Serial.println("Lỗi cập nhật: " + fbdo.errorReason());
    }
}

String ledState = "off";

void loop() {
    ledState = downloadData("/room0/device1/status");
    Button();
    digitalWrite(LED, (ledState == "on") ? HIGH : LOW);
    delay(500);
    pushNotification("Nút nhấn đã thay đổi trạng thái", DEVICE_ID);

}

// ✅ Đọc trạng thái thiết bị từ Firebase
String downloadData(String path) {
    if (Firebase.RTDB.getString(&fbdo, path)) {
        String status = fbdo.stringData();
        Serial.println("Trạng thái thiết bị: " + status);
        return status;
    }
    return "error";
}

// ✅ Upload dữ liệu lên Firebase
bool uploadData(String path, String data) {
    if (Firebase.RTDB.setString(&fbdo, path, data)) {
        Serial.println("Cập nhật thành công!");
        return true;
    } else {
        Serial.println("Lỗi cập nhật: " + fbdo.errorReason());
    }
    return false;
}

// ✅ Xử lý nút nhấn (Chống dội phím)
bool buttonState = false;
bool lastButtonState = HIGH;
void Button() {
    buttonState = digitalRead(BUTTON);
    if (buttonState == LOW && lastButtonState == HIGH) {
        delay(30); 
        if (digitalRead(BUTTON) == LOW) { 
            ledState = (ledState == "on") ? "off" : "on";
            uploadData("/room0/device1/status", ledState);
            pushNotification("Nút nhấn đã thay đổi trạng thái", DEVICE_ID);
        }
    }
    lastButtonState = buttonState;
}

// ✅ Lấy thời gian từ NTP
String getCurrentTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("❌ Lỗi lấy thời gian!");
        return "00.00.00.01.01.1970"; 
    }

    char timeStr[20];
    sprintf(timeStr, "%02d.%02d.%02d.%02d.%02d.%04d",
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 
            timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);

    return String(timeStr);
}

// ✅ Gửi thông báo lên Firebase
void pushNotification(String text, String deviceId) {
  String currentTime = getCurrentTime();  // Lấy thời gian thực
  String idtime = deviceId + "." + currentTime;

  // Tạo push key thủ công
  String pushKey = generateKey(); 

  // Construct the paths for text and time
  String pathText = String(USER_ID) + "/main/noti/" + pushKey + "/text";
  String pathTime = String(USER_ID) + "/main/noti/" + pushKey + "/time";
  String pathNewestId=String(USER_ID) +"/main/newestNoti";
  // Upload the text and time
  bool textUploaded = uploadData(pathText, text);
  bool timeUploaded = uploadData(pathTime, idtime);
  bool newestId= uploadData(pathNewestId,idtime);

  if (textUploaded && timeUploaded) {
    Serial.println("Push Notification Sent: " + pushKey);
  } else {
    Serial.println("Error sending push notification");
  }
}
String generateKey() {
    unsigned long timestamp = millis();  // Thời gian tính từ lúc ESP32 khởi động
    int randomNum = random(1000, 9999);  // Số ngẫu nhiên để tránh trùng key
    return String(timestamp) + "_" + String(randomNum);
}

