#ifndef __WIFI_AND_FIREBASE_H__
#define __WIFI_AND_FIREBASE_H__
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"  // Thư viện để lấy thời gian từ Internet
#include <Firebase_ESP_Client.h>
#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"

#define DEVICE_ID "esp32w2"
// Thông tin WiFi của bạn
#define WIFI_SSID "realme"
#define WIFI_PASSWORD "123456789@"

// Thông tin Firebase
#define FIREBASE_HOST "https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "TOpAxHzBbyQz0WatMvL0WUwC4a8EHPFogT0j0i1Y"  

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

#define DEVICE_ID "esp32w2"

// Cấu hình NTP
extern const char* ntpServer;
const long  gmtOffset_sec = 7 * 3600; // GMT+7 (Việt Nam)
const int   daylightOffset_sec = 0;   

void setupWifiFirebase();
String generateKey();   
String getCurrentTime();
String downloadData(String path);
bool uploadData(String path, String data);
void pushNotification(String text, String deviceId);

#endif