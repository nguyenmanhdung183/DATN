#include <Arduino.h>
#include "string.h"
#include <vector>
#define USER_ID "TcJzM8sKGeWkiaeQJgR7e6G5qxq1"
#define DEVICE_ID "esp32w2"
std::vector<int> buttonPins ={};  // Chân các nút bấm
// ko upload state trong class

//-------------------------------------------------Tiền xử lý------------------------------------------
#if 1
void setupWifiFirebase();
String generateKey();
String getCurrentTime();
String downloadData(String path);
bool uploadData(String path, String data);
void pushNotification(String text, String deviceId);

#endif
//-------------------------------------------------End tiền xử lý------------------------------------------

//-------------------------------------------------ROOM & Device Define---------------------------------------
#if 1
#define PHONG_BEP "23714458"
#define PHONG_NGU "323238991"
#define PHONG_KHACH "374566167"

class Device {
  private:
      int pin;
      String id;
      String name;
      String path; // Đường dẫn Firebase, ko có userid
      bool mainState;  // Trạng thái bật/tắt của thiết bị
      String otherState; //nhiệt độ hoặc độ ẩm
      String controlState; // Chỉ dành cho điều hòa
      String roomId; // ID của phòng chứa thiết bị
      int buttonPin;
      bool needFbUpdate = true;// mặc định là có nhé
  
  public:
    static std::vector<Device*> dvList;

      Device(int pin, String id = "unknown", String name = "unknown", String roomId = "unknown", int buttonPin = -1, bool needFbUpdate = true) {
        this->pin = pin;
          this->id = id;
          this->name = name;
          mainState = false;
          otherState = "unknown";
          controlState = "unknown";
          this->roomId = roomId;
          this->buttonPin = buttonPin;
          path ="/"+roomId+ "/" + id;
          this->needFbUpdate = needFbUpdate;
          if(buttonPin != -1) {
            buttonPins.push_back(buttonPin);
          }
          if(needFbUpdate){
            dvList.push_back(this);
          }
      }
  
      ~Device() {}
  
      void setMainState(bool state) { mainState = state; }
      bool getMainState() { return mainState; }
      void setOtherState(String state) { otherState = state; }
      String getOtherState() { return otherState; }
      String getId() { return id; }
      String getName() { return name; }
      String getControlState() { return controlState; }
      void setControlState(String state) { controlState = state; }
      int getPin() { return pin; }
      void setPin(int pin) { this->pin = pin; }
      void setPath(String path) { this->path = path; }
      String getPath() { return path; }
      int getButtonPin() { return buttonPin; }
      void setButtonPin(int buttonPin) { this->buttonPin = buttonPin; }
      
      void setRoomId(String roomId) { this->roomId = roomId; }
      String getRoomId() { return roomId; }        
      virtual void FbUpdate(String st) = 0; 

  };
  
class Light : public Device {// 1 chân nối vcc, chân điều khiển kéo xuống low thì sáng
public:
  Light(int pin, String id, String name, String roomId, int buttonPin=-1, bool needFbUpdate=true) : Device(pin, id, name, roomId, buttonPin, needFbUpdate) {}
  void begin(){
      pinMode(getPin(), OUTPUT);
      digitalWrite(getPin(), LOW);
  }
  void turnOnLed() {
      if(!getMainState()) {
        setMainState(true);
        digitalWrite(getPin(), LOW);
        Serial.println("💡 " + getId() + " ON");
      }
  }
  void turnOffLed() {
      if(getMainState()) {
        setMainState(false);
        digitalWrite(getPin(), HIGH);
        Serial.println("💡 " + getId() + " OFF");
      }
  }
  void toggleLed() {
      if(getMainState()) {
          turnOffLed();
      } else {
          turnOnLed();
      }
  }
  void sendStateToFirebase() {
      String state = getMainState() ? "on" : "off";
      uploadData(getPath() + "/status", state);
  }

  void buttonPress(){
    toggleLed();
    sendStateToFirebase();
  }
  void FbUpdate(String st){
    if(getMainState()&& st=="off" ){// nếu đang on
      turnOffLed();
    }
    else if(!getMainState() && st=="on"){// nếu đang off
      turnOnLed();
    }
  }
};
  

class Fan : public Device {
public:
  Fan(int pin, String id, String name, String roomId, int buttonPin =-1, bool needFbUpdate =true) : Device(pin, id, name, roomId, buttonPin, needFbUpdate) {

  }
  void begin() {  // Hàm này thay thế việc setup pinMode
    pinMode(getPin(), OUTPUT);
    digitalWrite(getPin(), LOW);
}
  void turnOnFan() {
      if(!getMainState()) {
          setMainState(true);
          digitalWrite(getPin(), HIGH);
          Serial.println("🌀 " + getId() + " ON");
      }
  }
  void turnOffFan() {
      if(getMainState()) {
          setMainState(false);
          digitalWrite(getPin(), LOW);
          Serial.println("🌀 " + getId() + " OFF");
      }
  }

  void toggleFan() {
      if(getMainState()) {
          turnOffFan();
      } else {
          turnOnFan();
      }
  }
  void sendStateToFirebase() {
      String state = getMainState() ? "on" : "off";
      uploadData(getPath() + "/status", state);
  }
  void buttonPress(){
    toggleFan();
    sendStateToFirebase();
  }

  void FbUpdate(String st){
    if(getMainState()&& st=="off" ){// nếu đang on
      turnOffFan();
    }
    else if(!getMainState() && st=="on"){// nếu đang off
      turnOnFan();
    }
  }
};

class AirConditioner : public Device {
  public:
      AirConditioner(int pin, String id, String name, String roomId, int buttonPin=-1, bool needFBUpdate=true) : Device(pin, id, name, roomId, buttonPin, needFBUpdate) {}
      void turnOnAC() {
          setMainState(true);
          Serial.println("❄️ " + getId() + " ON");
      }
      void turnOffAC() {
          setMainState(false);
          Serial.println("❄️ " + getId() + " OFF");
      }
      void toggleAC() {
          if(getMainState()) {
              turnOffAC();
          } else {
              turnOnAC();
          }
      }
      void setTemperature(String temp) {
          setOtherState(temp);
          Serial.println("❄️ " + getId() + " set temperature: " + temp + "°C");
      }
      void sendStateToFirebase() {
          String state = getMainState() ? "on" : "off";
          uploadData(getPath() + "/status", state);
          uploadData(getPath() + "/temperature", getOtherState());
      }

      void sendOtherStateToFirebase() {
          uploadData(getPath() + "/temperature", getOtherState());
      }
  };


  class MQ2 {  // Gas sensor
    private:
        int APin;
        int Dpin;
        int sensorValue;
        int sensorThres = 400;
        bool needTurnOffFan = false;
        bool dectectedGas = false;
        unsigned long gasDetectedTime = 0;  // Thời điểm bắt đầu phát hiện gas
        unsigned long lastTime = 0;         // Thời điểm phát hiện gas lần cuối
        bool isSentNoti = false;
    
    public:
        MQ2(int Apin, int Dpin, int sensorThres = 400) {
            this->APin = Apin;
            this->Dpin = Dpin;
            this->sensorThres = sensorThres;
        }
    
        void begin() {
            if (Dpin != -1) {
                pinMode(Dpin, INPUT);
            } else {
                Serial.println("Dpin = -1");
            }
        }
    
        void sentNotiToFb() {
            pushNotification("Có khí gas", DEVICE_ID);
        }
    
        void detectGas() {
            unsigned long now = millis();
            int gasValue = (APin != -1) ? analogRead(APin) : 0;
            int gasState = (Dpin != -1) ? digitalRead(Dpin) : HIGH;
    
            bool gasDetected = (APin != -1 && gasValue > sensorThres) || (Dpin != -1 && gasState == LOW);
    
            if (gasDetected) { 
                if (gasDetectedTime == 0) {
                    gasDetectedTime = now;  // Bắt đầu đếm thời gian phát hiện gas
                }
    
                if (!dectectedGas && now - gasDetectedTime >= 20) {  // Đã phát hiện gas trên 1 giây
                    dectectedGas = true;
                    needTurnOffFan = false;
                    lastTime = now;  // Cập nhật lần cuối phát hiện gas
                    if (!isSentNoti) {
                        sentNotiToFb();
                        isSentNoti = true;
                    }
                }
            } else {  
                gasDetectedTime = 0;  // Reset bộ đếm thời gian phát hiện gas
                if (dectectedGas && now - lastTime >= 5000) {  // Không có gas trong 4 giây
                    dectectedGas = false;
                    needTurnOffFan = true;
                    isSentNoti = false;
                }
            }
    
        }
    
        bool getDectectedGas() {
            return dectectedGas;
        }
        bool isNeedTurnFan() {
           return needTurnOffFan;
        }
        void justTurnOff(){
          needTurnOffFan = false;
        }
    };
    

class FM52{
  private:
    int pin;
    bool state;// false là k có, true là có
  public:
    void begin(){
      pinMode(pin, INPUT);
    }
    FM52(int pin){
      this->pin = pin;
      this->state = false;
    }
/*     int isDetectedClose(){ 
      // -1 là chuyển sang k có, 
      //1 là chuyển sang có, 
     // 0 là ko thay đổi
      
      if(digitalRead(pin) == LOW && state == false){ 
        state = true;
        return 1;
      }else if(digitalRead(pin) == HIGH && state == true){// HIGH là tắt
        state = false;
        return -1;
      }else{
       // state = !digitalRead(pin);
        return 0;
      }
    } */
   bool isDetectedClose(){
     state = !digitalRead(pin);
     return !digitalRead(pin);
   }

};

   #endif 
//-------------------------------------------------ROOM & Device Define---------------------------------------

//-------------------------------------------------BUTTON------------------------------------------------------
#if 1

//const int buttonPins[] = {13};  // Chân các nút bấm
//const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
// int numButtons = buttonPins.size();

// std::vector<bool> buttonStates(numButtons, false);
// std::vector<bool> lastButtonStates(numButtons, false);
// std::vector<unsigned long> lastDebounceTime(numButtons, 0);


int numButtons;
std::vector<bool> buttonStates;
std::vector<bool> lastButtonStates;
std::vector<unsigned long> lastDebounceTime;

const unsigned long debounceDelay = 20;  // Thời gian chống dội phím
#if 1// button touch
void buttonBegin() {
  numButtons = buttonPins.size();
  buttonStates.resize(numButtons, false);
  lastButtonStates.resize(numButtons, false);
  lastDebounceTime.resize(numButtons, 0); 

    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT);
    }
}
int getPressedButton() {
  int rt=-1;
  unsigned long currentTime = millis();

  for (int i = 0; i < numButtons; i++) {
      bool currentState = digitalRead(buttonPins[i]);

      if (currentState != lastButtonStates[i]) {
          lastDebounceTime[i] = currentTime;
      }

      if ((currentTime - lastDebounceTime[i]) > debounceDelay) {
          if (currentState ==HIGH && buttonStates[i]==LOW) {
                  rt = buttonPins[i];
          }
          buttonStates[i] = currentState;

      }

      lastButtonStates[i] = currentState;
  }

  return rt;  // Không có nút nào được nhấn
}

#endif

#if 0 // button thường
void buttonBegin() {
    numButtons = buttonPins.size();
    buttonStates.resize(numButtons, false);
    lastButtonStates.resize(numButtons, false);
    lastDebounceTime.resize(numButtons, 0); 

    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

// Hàm trả về **số pin của nút vừa nhấn** (hoặc -1 nếu không có nút nào)
int getPressedButton() {
    int rt=-1;
    unsigned long currentTime = millis();

    for (int i = 0; i < numButtons; i++) {
        bool currentState = digitalRead(buttonPins[i]);

        if (currentState != lastButtonStates[i]) {
            lastDebounceTime[i] = currentTime;
        }

        if ((currentTime - lastDebounceTime[i]) > debounceDelay) {
            if (currentState ==LOW && buttonStates[i]==HIGH) {
                    rt = buttonPins[i];
            }
            buttonStates[i] = currentState;

        }

        lastButtonStates[i] = currentState;
    }

    return rt;  // Không có nút nào được nhấn
}
#endif

#endif
//-------------------------------------------------END BUTTON------------------------------------------------------


//------------------------------------------------FireBase & Wifi-----------------------------------------------------
#if 1
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
        Serial.println("Trạng thái thiết bị: " + status);
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
//------------------------------------------------END FireBase & Wifi-----------------------------------------------------


//-------------------------------------------------STREAM------------------------------------------------------
#if 1
void StreamData(std::vector<Device*> &dvList){
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis(); 
  if (currentTime - lastTime > 1000) {  // Gửi dữ liệu mỗi 10 giây
    lastTime = currentTime;
    for(int i=0;i<dvList.size();i++){
      String st = downloadData(dvList[i]->getPath() + "/status");
      dvList[i]->FbUpdate(st);
    }
  }

}
#endif
//-------------------------------------------------END STREAM------------------------------------------------------

//----------------------------------------------KITCHEN------------------------------------------------------------------

#if 1

#define A0GAS 1      // Chân cảm biến khí gas (có thể thay đổi tùy theo phần cứng của bạn)
#define D0GAS 2      // Chân trạng thái cảm biến khí (có thể thay đổi tùy theo phần cứng của bạn)
#define BUTTONFANKC 13    // Chân nút bấm (có thể thay đổi tùy theo phần cứng của bạn)
#define FAN 27     
// Hàm bật quạt
// FAN nối vào chân B của SN8050, cực E nối với GND, cực C nối với chân âm của quạt, VCC nối chân còn lại của quạt
/*
#define A0GAS 1      // Chân cảm biến khí gas (có thể thay đổi tùy theo phần cứng của bạn)
#define D0GAS 2      // Chân trạng thái cảm biến khí (có thể thay đổi tùy theo phần cứng của bạn)
#define BUTTONFANKC 13    // Chân nút bấm (có thể thay đổi tùy theo phần cứng của bạn)
#define FAN 27       // Chân điều khiển quạt (được cấu hình với OpenDrain)

*/

void turnOnFan() {
  digitalWrite(FAN, HIGH);  
  //
}

// Hàm tắt quạt
void turnOffFan() {
  digitalWrite(FAN, LOW);
}
// Hàm chuyển trạng thái quạt (bật/tắt) khi nhấn nút
void toggleFan() {
  static bool fanState = false; 
  fanState = !fanState;          
  if (fanState) {
    turnOnFan();  // Bật quạt
    Serial.println("Fan ON");
  } else {
    turnOffFan();  // Tắt quạt
    Serial.println("Fan OFF");
  }
}

void ButtonFanKC() {
  static bool buttonState = false;
  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 30;  // Thời gian debounce 30ms

  bool currentState = digitalRead(BUTTONFANKC);
  
  if (currentState != lastButtonState) {  // Phát hiện sự thay đổi trạng thái
    lastDebounceTime = millis();  // Đánh dấu thời gian thay đổi
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {  // Kiểm tra nếu vượt quá thời gian debounce
    if (currentState == LOW && buttonState == HIGH) {  // Nút bấm được nhấn
      Serial.println("Button press");
      toggleFan();  // Chuyển trạng thái quạt
    }
    buttonState = currentState;
  }

  lastButtonState = currentState;
}


// ✅ Kiểm tra khí gas và điều khiển quạt bật/tắt
void checkGas() {
  int gasValue = analogRead(A0GAS);  // Đọc giá trị từ cảm biến khí gas

  // Nếu giá trị khí vượt quá ngưỡng nhất định, bật quạt
  if (gasValue > 400) {  // Điều chỉnh ngưỡng này tùy theo việc hiệu chỉnh cảm biến khí của bạn
    if (digitalRead(FAN) == HIGH) {  // Nếu quạt đang tắt, bật quạt
      turnOnFan();
      Serial.println("Gas detected! Fan ON");
    }
  } else {
    if (digitalRead(FAN) == LOW) {  // Nếu quạt đang bật, tắt quạt
      turnOffFan();
      Serial.println("No gas detected. Fan OFF");
    }
  }
}
#endif

//----------------------------------------------END KITCHEN------------------------------------------------------------------


//------------------------------------------------LIVING ROOM -------------------------------------------------------------
#if 0


#endif
//------------------------------------------------LIVING ROOM -------------------------------------------------------------

std::vector<Device*> Device::dvList;
Fan fanBep(27, "82322702", "Quạt 1", PHONG_BEP, 26);// 1 chan noi 5v 1 chan noi BJT cuc C 
Light lightBep(25, "903872522", "Đèn 1", PHONG_BEP, 33);// 1 chân nối vcc, chân điều khiển kéo xuống low thì sáng
Light lightHall(12, "null", "Đèn 2", PHONG_KHACH, -1,false);

MQ2 mq2(-1,32, 200);// A, D, Threshold, dùng Analog cho đẹp :)))) (cảm biến gas) 2 chan nguon 5v, 1 chan tin hieu
FM52 fm52(35);// chân digital (cảm biến tiệm cận) 2 chan nguon 3.3v, 1 chasn tin hieu


void setup() {
  
 
  setupWifiFirebase();
  Serial.begin(115200);
  fanBep.begin();
  lightBep.begin();
  lightHall.begin();
  mq2.begin();
  fm52.begin();
  buttonBegin();

  

}

void loop() {

  StreamData(Device::dvList);

  


/*   if(fm52.isDetectedClose()){
    lightBep.turnOnLed();
  }
  else{
    lightBep.turnOffLed();
  } */

  if(fm52.isDetectedClose()){
    lightHall.turnOnLed();
  }
  else{
    lightHall.turnOffLed();
  }

  mq2.detectGas();
  if(mq2.getDectectedGas()){
    if(!fanBep.getMainState()){
      fanBep.turnOnFan();
      fanBep.sendStateToFirebase();
    }
  }
  if(mq2.isNeedTurnFan()){
    fanBep.turnOffFan();
    fanBep.sendStateToFirebase();
    mq2.justTurnOff();
  }
  switch(getPressedButton()){
    case 33:
      lightBep.buttonPress();
      break;
    case 26:
      fanBep.buttonPress();
      break;
    default:
      break;
  }

}
