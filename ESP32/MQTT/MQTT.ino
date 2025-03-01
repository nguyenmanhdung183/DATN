#include<WiFi.h>          // Thư viện để kết nối WiFi trên ESP8266
#include "DHTesp.h"              // Thư viện để sử dụng cảm biến DHT11 hoặc DHT22
#include <ArduinoJson.h>         // Thư viện để làm việc với dữ liệu JSON
#include <PubSubClient.h>        // Thư viện để giao tiếp qua MQTT
#include <WiFiClientSecure.h>    // Thư viện hỗ trợ kết nối WiFi an toàn (SSL/TLS)

//---- Định nghĩa chân kết nối của cảm biến DHT11 ----
#define DHTpin 2
DHTesp dht;  // Khởi tạo đối tượng DHTesp

//---- Thông tin kết nối WiFi ----
const char* ssid = "DTM E-SMART";      // Tên mạng WiFi
const char* password = "0919890938";   // Mật khẩu WiFi

//---- Thông tin kết nối MQTT ----
const char* mqtt_server = "ec405b5b55474xxxxxxxxxx.s2.eu.hivemq.cloud";  // Địa chỉ MQTT broker
const int mqtt_port = 8883;           // Cổng kết nối MQTT (8883 là cổng cho kết nối an toàn SSL)
const char* mqtt_username = "e-smart"; // Tên người dùng MQTT
const char* mqtt_password = "abc@1234"; // Mật khẩu người dùng MQTT

// Khởi tạo đối tượng WiFiClientSecure và PubSubClient để kết nối MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Các biến để gửi và nhận thông điệp MQTT
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

//---- Hàm kết nối WiFi ----
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  // Bắt đầu kết nối WiFi

  // Chờ cho đến khi kết nối WiFi thành công
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());  // Tạo một số ngẫu nhiên cho client ID
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // In ra địa chỉ IP của ESP8266 sau khi kết nối thành công
}

//---- Hàm kết nối tới MQTT broker ----
void reconnect() {
  // Kiểm tra xem kết nối có sẵn chưa, nếu không thì thử lại
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientID =  "ESPClient-";
    clientID += String(random(0xffff), HEX);  // Tạo một ID ngẫu nhiên cho client

    // Thử kết nối với MQTT broker bằng client ID và thông tin đăng nhập
    if (client.connect(clientID.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp8266/client");  // Đăng ký để nhận thông điệp từ topic này
    } else {
      // Nếu không kết nối được, in ra mã lỗi và thử lại sau 5 giây
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//---- Hàm xử lý khi nhận được thông điệp MQTT ----
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];  // Duyệt qua payload và chuyển thành chuỗi
  }
  Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
}

//---- Hàm xuất bản thông điệp MQTT ----
void publishMessage(const char* topic, String payload, boolean retained) {
  // Gửi thông điệp lên topic MQTT
  if (client.publish(topic, payload.c_str(), true)) {
    Serial.println("Message published [" + String(topic) + "]: " + payload);
  }
}

void setup() {
  Serial.begin(9600);  // Bắt đầu giao tiếp Serial với tốc độ 9600
  while (!Serial) delay(1);  // Chờ nếu Serial chưa sẵn sàng

  dht.setup(DHTpin, DHTesp::DHT11);  // Khởi tạo cảm biến DHT11

  setup_wifi();  // Kết nối WiFi
  espClient.setInsecure();  // Đặt chế độ không bảo mật (vì đang dùng SSL)
  client.setServer(mqtt_server, mqtt_port);  // Cấu hình server MQTT
  client.setCallback(callback);  // Đặt callback để xử lý khi nhận thông điệp MQTT
}

unsigned long timeUpdata = millis();  // Biến lưu thời gian cập nhật

void loop() {
  if (!client.connected()) {  // Kiểm tra xem ESP có đang kết nối MQTT không
    reconnect();  // Nếu không kết nối, thử kết nối lại
  }
  client.loop();  // Xử lý các thông điệp MQTT

  // Đọc dữ liệu từ cảm biến DHT11 mỗi 5 giây
  if (millis() - timeUpdata > 5000) {
    delay(dht.getMinimumSamplingPeriod());  // Đảm bảo thời gian chờ giữa các lần đọc
    float h = dht.getHumidity();  // Đọc độ ẩm
    float t = dht.getTemperature();  // Đọc nhiệt độ

    // Tạo đối tượng JSON để gửi dữ liệu
    DynamicJsonDocument doc(1024);
    doc["humidity"] = h;
    doc["temperature"] = t;

    char mqtt_message[128];  // Dữ liệu sẽ gửi qua MQTT
    serializeJson(doc, mqtt_message);  // Chuyển đối tượng JSON thành chuỗi

    // Gửi dữ liệu JSON lên topic "esp8266/dht11"
    publishMessage("esp8266/dht11", mqtt_message, true);

    timeUpdata = millis();  // Cập nhật thời gian gửi thông điệp
  }
}
