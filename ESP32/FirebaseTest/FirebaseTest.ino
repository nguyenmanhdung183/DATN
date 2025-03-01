https://dayhocstem.com/blog/2023/01/dieu-khien-led-qua-iot-bang-google-firebase-va-esp8266.html

#include <Arduino.h>
#include <WiFi.h>

#include "FirebaseESP8266.h"

// Thay đổi URL và key Firebase của bạn ở đây
#define FIREBASE_HOST "https://esp32-e085a-default-rtdb.asia-southeast1.firebasedatabase.app/"    
#define FIREBASE_AUTH "VbVajPQB3iMen4gTCRdqMkxj10JZ11FaDT4hABNU"

// Thông tin kết nối WiFi
#define WIFI_SSID "VIETTEL"                         
#define WIFI_PASSWORD "minhtrang2203"  

FirebaseData firebaseData; 
String fireStatus = "";       // trạng thái LED nhận từ Firebase
int led = 2;  // GPIO số 2 kết nối với LED
String path = "ESP8266 NodeMCU Board/Outputs/Digital/"; // Đường dẫn tới LED trong Firebase Database

FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() 
{
  Serial.begin(115200);
  delay(1000);    
  pinMode(led, OUTPUT);  
  digitalWrite(led, HIGH);     // làm tắt LED ngoài

  // Kết nối với WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);            
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);

  // Cấu hình Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseAuth.token.set(FIREBASE_AUTH);

  // Kết nối đến Firebase
  Firebase.begin(firebaseConfig, firebaseAuth);
  Firebase.setString(firebaseData, path+"LED", "OFF"); // gửi trạng thái LED ban đầu
}
 
void loop() 
{
  Firebase.getString(firebaseData, path+"LED");   // nhận trạng thái LED từ Firebase
  fireStatus = firebaseData.stringData();
  
  if (fireStatus == "ON") 
  {    
    Serial.println("Led Turned ON");       
    digitalWrite(led, LOW);       // bật LED ngoài
  } 
  else if (fireStatus == "OFF") 
  {     
    Serial.println("Led Turned OFF");
    digitalWrite(led, HIGH);     // tắt LED ngoài
  }
  else 
  {
    Serial.println("Command Error! Please send ON/OFF");
  }
}
