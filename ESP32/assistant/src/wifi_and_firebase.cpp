#include "wifi_and_firebase.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setupWifiFirebase(){
    // Kết nối WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Đang kết nối WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n Đã kết nối WiFi!");


    // Cấu hình Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}


    
// ✅ Đọc trạng thái thiết bị từ Firebase
String downloadData(String path) {
    path = String(USER_ID) + path;  // Gán USER_ID vào đường dẫn
    if (Firebase.RTDB.getString(&fbdo, path)) {
        String status = fbdo.stringData();
       // Serial.println("Trạng thái thiết bị: " + status);
        return status;
    }
    return "error";
}

// ✅ Upload dữ liệu lên Firebase
bool uploadData(String path, String data) {// path : /a/b
    path = String(USER_ID) + path;  // Gán USER_ID vào đường dẫn
    if (Firebase.RTDB.setString(&fbdo, path, data)) {
        Serial.println("Cập nhật thành công!");
        return true;
    } else {
        Serial.println("Lỗi cập nhật: " + fbdo.errorReason());
    }
    return false;
}



String fetchJsonFirebase(){

    String path = String(USER_ID);  // Gán USER_ID vào đường dẫn
    if (Firebase.RTDB.getString(&fbdo, path)) {
        String status = fbdo.stringData();
        return status;
    }

    return "error";
}

