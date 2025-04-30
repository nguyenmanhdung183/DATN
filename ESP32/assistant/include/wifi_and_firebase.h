#ifndef WIFI_AND_FIREBASE_H
#define WIFI_AND_FIREBASE_H
#include <Arduino.h>
// Thông tin WiFi của bạn
#define WIFI_SSID "realme"
#define WIFI_PASSWORD "123456789@"

// Thông tin Firebase
#define FIREBASE_HOST "https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "TOpAxHzBbyQz0WatMvL0WUwC4a8EHPFogT0j0i1Y"  
// Thiết bị
#define DEVICE_ID "esp32w2"

#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"


void setupWifiFirebase();
String downloadData(String path);
bool uploadData(String path, String data);
String fetchJsonFirebase();

#endif