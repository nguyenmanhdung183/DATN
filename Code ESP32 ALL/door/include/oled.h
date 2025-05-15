#ifndef OLED_H
#define OLED_H
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

#endif