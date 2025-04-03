#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"
#define RoomID "374566167"
#define doorID "503832430" // password: 1234, status: on/off
#define DEVICE_ID "esp32w2"

String did ="/" + String(RoomID) + "/" + String(doorID); // Đường dẫn đến thiết bị
bool uploadData(String path, String data);

class RFID_Door {
      /*
  VCC    3.3V
  GND    GND
  SCK    18 YELLOW
  MOSI   23 WHITE
  MISO   19 GREEN
  SDA/SS 5 ORANGE
  RST    15 BLUE
  */
private:
    int SERVO;
    bool state = false; // Mặc định là đóng cửa
    MFRC522 rfid; // Đối tượng RFID
    String pw ="1234"; // Mật khẩu mặc định
    Servo myServo;
    const byte validUID[2][4] = {
        {0xC3, 0x74, 0xCD, 0x28}, // Thẻ 1
        {0x33, 0xA2, 0x3F, 0x14}  // Thẻ 2
    };
    int validUIDCount = 2; // Số lượng thẻ hợp lệ

public:
    RFID_Door(int SS,int RS, int SERVO) : rfid(SS, RS) { // Khởi tạo RFID với chân SS
        this->SERVO = SERVO;
    }

    void init() {
        SPI.begin(); // Khởi tạo SPI (ESP32 tự động xử lý các chân)
        rfid.PCD_Init(); // Khởi tạo RFID
        myServo.attach(SERVO); // Gán chân điều khiển servo
        myServo.write(0); // Đặt servo về vị trí đóng cửa
    }
    
    bool getState() {
        return state;
    }

    void setState(bool newState) {
        state = newState;
    }

    void turnServo(int angle) {
        myServo.write(angle);
    }

    void openDoor() {
        Serial.println("✅ Mở cửa...");
        turnServo(0); // Mở cửa
        state = true;
    }

    void closeDoor() {
        Serial.println("🔒 Đóng cửa...");
        turnServo(112); // Đóng cửa
        state = false;
    }

    bool isValidUID() {
        // Kiểm tra nếu UID hợp lệ
        for (int i = 0; i < validUIDCount; i++) {
            if (memcmp(rfid.uid.uidByte, validUID[i], 4) == 0) {
                return true;
            }
        }
        return false;
    }

    void printUID() {
        Serial.print("UID: ");
        for (byte i = 0; i < rfid.uid.size; i++) {
            Serial.print(rfid.uid.uidByte[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    bool checkCard() {
        bool rt = false;  // Declare rt variable
        //Serial.println("⏳ Đang chờ thẻ...");
        if (!rfid.PICC_IsNewCardPresent()) return rt;  // Kiểm tra thẻ mới
        if (!rfid.PICC_ReadCardSerial()) return rt;   // Đọc serial của thẻ

        //printUID(); // In UID ra Serial Monitor
        Serial.print("checking...");
        if (isValidUID()) {
            Serial.println("✅ Thẻ hợp lệ");
            rt = true;
        } else {
            Serial.println("⛔ Thẻ không hợp lệ!");
        }

        rfid.PICC_HaltA();  // Dừng giao tiếp với thẻ
        rfid.PCD_StopCrypto1(); // Dừng mã hóa dữ liệu
        return rt;
    }
    bool checkPassword(String input) {
        if (input == pw) {
            Serial.println("✅ Mật khẩu hợp lệ");
            return true;
        } else {
            Serial.println("⛔ Mật khẩu không hợp lệ!");
            return false;
        }
    }
    void updatePassword(String newPassword) {
        if(newPassword != pw) {
            pw = newPassword; // Cập nhật mật khẩu mới
            Serial.println("✅ Mật khẩu đã được cập nhật: " + pw);
        }
    }
    void updateToFB(){
        String path = did + "/status"; // Đường dẫn đến trạng thái cửa
        if (state) {
            uploadData(path, "on"); // Cập nhật trạng thái là "on"
        } else {
            uploadData(path, "off"); // Cập nhật trạng thái là "off"
        }
    }

};
class OLED {
    private:
        // Thông số màn hình OLED
        static constexpr int SCREEN_WIDTH = 128;
        static constexpr int SCREEN_HEIGHT = 64;
        static constexpr int OLED_RESET = -1;
        static constexpr int SCREEN_ADDRESS = 0x3C;
    
        // Chân I2C của ESP32
        static constexpr int SDA = 21;
        static constexpr int SCL = 22;
        unsigned long previousMillis = 0; 
        unsigned long currentMillis = 0;
        const long interval = 2000;
        Adafruit_SSD1306 display;
    
    public:
        OLED() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}
    
        void init() {
            Wire.begin(SDA, SCL); // Khởi tạo I2C
    
            if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
                Serial.println(F("OLED không tìm thấy! Kiểm tra kết nối."));
                while (true); // Dừng chương trình
            }
    
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(10, 25);
            display.print("Hello!");
            display.display();
            showHome();
        }
        void goToHomeScreen(){
            currentMillis = millis();
            if(currentMillis - previousMillis >= interval){
                previousMillis = currentMillis;
                showHome();
            }
        }
        void updatePrevious(){
            previousMillis = millis();
        }
    
        void clearDisplay() {
            display.clearDisplay();
            display.display();
        }
    
        void showText(const String& text, int x, int y) {
            display.clearDisplay(); // Xóa màn hình trước khi in chữ mới
            display.setCursor(x, y);
            display.print(text);
            display.display();
        }
    
        void showPassword(String pw){
            display.clearDisplay();
            display.setCursor(20, 15);
            display.print("Password");
            int textWidth = pw.length() * (6 * 2);
            int xStart = (128-textWidth)/2;
            display.setCursor(xStart, 40);
            display.print(pw);
            display.display();
        }
        void showWrongPassword(){
            display.clearDisplay();
            display.setCursor(35, 15);
            display.print("Wrong");
            display.setCursor(20, 35);
            display.print("Password");
            display.display();
        }
        void showCardAccepted(){
            display.clearDisplay();
            display.setCursor(40, 15);
            display.print("Card");
            display.setCursor(20, 35);
            display.print("Accepted");
            display.display();
    
        }
        void showCardRejected(){
            display.clearDisplay();
            display.setCursor(40, 15);
            display.print("Card");
            display.setCursor(20, 35);
            display.print("Rejected");
            display.display();
    
        }
        void showOK(){
            display.clearDisplay();
            display.setCursor(30, 15);
            display.print("OK!!!!");
            display.display();
    
        }
        void showHome(){
            display.clearDisplay();
            display.setCursor(10, 15);
            display.print("SmartHome");
            display.setCursor(18, 40);
            display.print("20213838");
            display.display();
        }
        void showCloseDoor(){
            display.clearDisplay();
            display.setCursor(5, 15);
            display.print("Close Door");
            display.display();
        }
        void showOpenDoor(){
            display.clearDisplay();
            display.setCursor(5, 15);
            display.print("Open Door");
            display.display();
        }

    };
class keyBoard {
        private:
          const int rows = 3;
          String str ="";
          const int cols = 4;
          //const int rowPins[3] = {34, 35, 32};  // Chân hàng (INPUT, PULLDOWN ngoài)
          const int rowPins[3] = {14,12,13};  // Chân hàng (INPUT, PULLDOWN ngoài)
          const int colPins[4] = {27, 26, 25, 33};  // Chân cột (OUTPUT)
          
          unsigned long lastDebounceTime[3][4] = {0};  // Lưu thời gian debounce
          bool lastButtonState[3][4] = {false};  // Trạng thái trước đó của phím
          bool buttonState[3][4] = {false};  // Trạng thái hiện tại
          bool keyPressed[3][4] = {false};  // Lưu trạng thái phím đã nhấn để tránh lặp lại nhấn liên tục
          unsigned long debounceDelay = 100;  // Thời gian debounce (20ms)
      
          String arr2[3][4] = {
              {"1", "4", "7", "L"},
              {"2", "5", "8", "0"},
              {"3", "6", "9", "R"}
          };
      
        public:
          keyBoard() {}
      
          void init() {
              // Cài đặt các chân hàng và cột
              for (int i = 0; i < rows; i++) {
                  pinMode(rowPins[i], INPUT_PULLDOWN);  // Dùng điện trở Pull-Down ngoài
              }
              for (int j = 0; j < cols; j++) {
                  pinMode(colPins[j], OUTPUT);
                  digitalWrite(colPins[j], LOW);  // Đặt ban đầu là LOW
              }
          }
      
          void reset() {
              // Đặt lại trạng thái phím đã nhấn
              for (int i = 0; i < rows; i++) {
                  for (int j = 0; j < cols; j++) {
                      keyPressed[i][j] = false;
                      buttonState[i][j] = false;  // Đặt lại trạng thái phím
                      lastButtonState[i][j] = false;  // Đặt lại trạng thái trước đó
                      lastDebounceTime[i][j] = 0;  // Đặt lại thời gian debounce
                  }
              }
          }
          String getPressKey() {
              String rt = "null";
              unsigned long currentTime = millis();
      
              // Quét qua các cột
              for (int i = 0; i < cols; i++) {
                  digitalWrite(colPins[i], HIGH);  // Kích hoạt cột
      
                  // Quét qua các hàng
                  for (int j = 0; j < rows; j++) {
                      bool reading = digitalRead(rowPins[j]);  // Đọc trạng thái phím
      
                      // Kiểm tra debounce
                      if (reading != lastButtonState[j][i]) {
                          lastDebounceTime[j][i] = currentTime;  // Lưu lại thời gian khi có thay đổi
                      }
      
                      if ((currentTime - lastDebounceTime[j][i]) > debounceDelay) {
                          if (reading == HIGH && !keyPressed[j][i]) {  // Phím được nhấn lần đầu
                              rt = arr2[j][i];  // Lưu phím nhấn
                              keyPressed[j][i] = true;  // Đánh dấu là đã nhấn
                          } 
                          else if (reading == LOW && keyPressed[j][i]) {  // Phím được thả ra
                              keyPressed[j][i] = false;  // Đánh dấu là phím đã được thả
                          }
                          buttonState[j][i] = reading;  // Cập nhật trạng thái hiện tại
                      }
      
                      lastButtonState[j][i] = reading;  // Cập nhật trạng thái trước đó
                  }
      
                  digitalWrite(colPins[i], LOW);  // Tắt cột
              }
      
              return rt;  // Trả về phím đã nhấn hoặc "null" nếu không có phím nhấn
          }
      
      
          void updateStr(String a){
              str += a;
      
          }
      
      
          String getStr(){
              return str;  // Trả về chuỗi str
          }   
          void deleteStr(){
              str = "";  // Xóa chuỗi str
          }
      };
#if 1 // Firebase & Wifi
      #include <WiFi.h>
      #include "time.h"  // Thư viện để lấy thời gian từ Internet
      #include <Firebase_ESP_Client.h>
      
      // Thông tin WiFi của bạn
      #define WIFI_SSID "realme"
      #define WIFI_PASSWORD "123456789@"
      
      // Thông tin Firebase
      #define FIREBASE_HOST "https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/"
      #define FIREBASE_AUTH "TOpAxHzBbyQz0WatMvL0WUwC4a8EHPFogT0j0i1Y"  
      
      FirebaseData fbdo;
      FirebaseAuth auth;
      FirebaseConfig config;
      
      // Thiết bị
      #define DEVICE_ID "esp32w2"
      
      // Cấu hình NTP
      const char* ntpServer = "pool.ntp.org";  
      const long  gmtOffset_sec = 7 * 3600; // GMT+7 (Việt Nam)
      const int   daylightOffset_sec = 0;   
      
      void setupWifiFirebase(){
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
      }
      
      String generateKey() {
          unsigned long timestamp = millis();  // Thời gian tính từ lúc ESP32 khởi động
          int randomNum = random(1000, 9999);  // Số ngẫu nhiên để tránh trùng key
          return String(timestamp) + "_" + String(randomNum);
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
      
      // ✅ Gửi thông báo lên Firebase
      void pushNotification(String text, String deviceId) {
          String currentTime = getCurrentTime();  // Lấy thời gian thực
          String idtime = deviceId + "." + currentTime;
      
          // Tạo push key thủ công
          String pushKey = generateKey(); 
      
          // Đường dẫn Firebase với USER_ID
          String pathText =  "/main/noti/" + pushKey + "/text";
          String pathTime =  "/main/noti/" + pushKey + "/time";
          String pathNewestId = "/main/newestNoti";
      
          // Upload dữ liệu
          bool textUploaded = uploadData(pathText, text);
          bool timeUploaded = uploadData(pathTime, idtime);
          bool newestId = uploadData(pathNewestId, idtime);
      
          if (textUploaded && timeUploaded && newestId) {
              Serial.println("✅ Push Notification Sent: " + pushKey);
          } else {
              Serial.println("❌ Error sending push notification");
          }
      }
#endif      


// pin ko thể dùng khi dùng wifi 34 35 36 39  
// Tạo đối tượng RFID_Door với chân SS là 5 và servo kết nối với chân 12
RFID_Door door(5,15, 2);// chân thứ 3 là servo 5V (12) /18 23 19 5 22 12 15
OLED oled; // Tạo đối tượng OLED SDA=21 SCL=22
keyBoard kb; // pin: 34 35 32 27 26 25 33
bool isTypePW = false;

void StreamDoorData(){
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis(); 
    if (currentTime - lastTime > 1000) {  // Gửi dữ liệu mỗi 10 giây
      lastTime = currentTime;
        String st = downloadData(did + "/status"); // Đọc trạng thái từ Firebase
        if (st == "on") {
           if(!door.getState()) {
                door.openDoor(); // Nếu trạng thái là "on" thì mở cửa
                oled.showOpenDoor(); // Hiển thị thông báo mở cửa
                oled.updatePrevious(); // Cập nhật thời gian trước đó
            }
        } else if (st == "off") {
           if(door.getState()) {
                door.closeDoor(); // Nếu trạng thái là "off" thì đóng cửa
                oled.showCloseDoor(); // Hiển thị thông báo đóng cửa
                oled.updatePrevious(); // Cập nhật thời gian trước đó
            }
        } else {
            Serial.println("❌ Trạng thái không hợp lệ!");
            Serial.println("Trạng thái: " + st);
        }
        String pw = downloadData(did+"/password"); // Đọc mật khẩu từ Firebase
        door.updatePassword(pw); // Cập nhật mật khẩu mới
    }
}
void allDoor(){
    StreamDoorData(); // Gửi dữ liệu trạng thái cửa
    if (door.checkCard()) {
        if (door.getState()) {
            door.closeDoor(); // Nếu cửa đang mở thì đóng lại
            door.updateToFB(); // Cập nhật trạng thái cửa lên Firebase
            oled.showCloseDoor();
            oled.updatePrevious(); // Cập nhật thời gian trước đó
            isTypePW = false; // Đặt lại trạng thái nhập mật khẩu
        } else {
            door.openDoor(); // Nếu cửa đang đóng thì mở ra
            oled.showCardAccepted(); // Hiển thị thông báo mở cửa
            oled.updatePrevious(); // Cập nhật thời gian trước đó
            door.updateToFB(); // Cập nhật trạng thái cửa lên Firebase
            isTypePW = false; // Đặt lại trạng thái nhập mật khẩu
        }
    }

    String key = kb.getPressKey(); // Lấy phím nhấn từ bàn phím
    if (key != "null") {
        isTypePW = true;
        Serial.println(key); // In phím nhấn ra Serial Monitor
        if (key == "L") {
            kb.deleteStr(); // Xóa chuỗi khi nhấn L
            oled.showPassword(kb.getStr());
        } else if (key == "R") {
            oled.showPassword(kb.getStr());
            if(door.getState() && kb.getStr() == "") {
                door.closeDoor();
                door.updateToFB(); // Cập nhật trạng thái cửa lên Firebase
                oled.showCloseDoor(); // Nếu cửa đang mở thì đóng lại
                oled.updatePrevious(); // Cập nhật thời gian trước đó
                isTypePW = false;
                
            }else{
                if(door.checkPassword(kb.getStr())) {
                    door.openDoor(); // Nếu mật khẩu đúng thì mở cửa
                    door.updateToFB(); // Cập nhật trạng thái cửa lên Firebase
                    oled.showOK();
                    oled.updatePrevious(); // Cập nhật thời gian trước đó
                    isTypePW = false;
                } else {
                    oled.showWrongPassword();
                    oled.updatePrevious(); // Cập nhật thời gian trước đó
                    isTypePW = false;
                }
            }
            kb.deleteStr(); // Xóa chuỗi sau khi kiểm tra mật khẩu
        } else {
            kb.updateStr(key);
            oled.showPassword(kb.getStr());
            isTypePW = true; // Đánh dấu là đang nhập mật khẩu
        }
        kb.reset(); // Đặt lại trạng thái bàn phím sau khi nhấn
    }

   //delay(500); // Chờ 500ms để tránh trùng lặp quá nhanh
   if(!isTypePW){
    oled.goToHomeScreen();
   }
}
void setup() {
    setupWifiFirebase(); // Kết nối WiFi và Firebase
    Serial.begin(115200);
    door.init();
    oled.init();
    kb.init();

}

void loop() {
   allDoor(); 
}
 