#ifndef WIFI_AND_FIREBASE_H
#define WIFI_AND_FIREBASE_H
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"  // Thư viện để lấy thời gian từ Internet
#include <Firebase_ESP_Client.h>

#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"
#define RoomID "374566167"
#define doorID "503832430" // password: 1234, status: on/off
#define DEVICE_ID "esp32w2"

// Thông tin WiFi của bạn
#define WIFI_SSID "realme"
#define WIFI_PASSWORD "123456789@"

// Thông tin Firebase
#define FIREBASE_HOST "https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "TOpAxHzBbyQz0WatMvL0WUwC4a8EHPFogT0j0i1Y"  

extern String did; // Đường dẫn đến thiết bị

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

// Cấu hình NTP
extern const char* ntpServer;
extern const long  gmtOffset_sec; // GMT+7 (Việt Nam)
extern const int   daylightOffset_sec;


void setupWifiFirebase();
String generateKey();
String getCurrentTime();
String downloadData(String path);
bool uploadData(String path, String data);
void pushNotification(String text, String deviceId);

#endif