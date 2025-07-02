#include "led_button.h"
#include <Arduino.h>

extern bool isRecording; // Biến toàn cục để kiểm soát trạng thái ghi âm


void initLed(){
    pinMode(2, OUTPUT); // Thiết lập chân LED là OUTPUT
}
void initButton(int buttonPin) {
    //pinMode(buttonPin, INPUT_PULLUP); // Cấu hình nút nhấn với điện trở kéo lên
    pinMode(buttonPin, INPUT);
}


bool getPressedButton(int buttonPin){
    bool rt = false;
    static bool lastButtonState = false;
    static bool buttonState = false;
    static unsigned long lastDebounceTime = 0;
    unsigned long currentTime = millis();
    bool currentState = digitalRead(buttonPin);

    if (currentState != lastButtonState) {
        lastDebounceTime = currentTime;
    }

    if(currentTime - lastDebounceTime > 30){
        if(currentState == HIGH && buttonState==LOW ){
            rt  = true;
        }
        buttonState = currentState;
    }
    lastButtonState = currentState;
    return rt;

}

void Led() {
    static bool state = LOW;              // Biến lưu trạng thái của LED (ON/OFF)
    static unsigned long previousMillis = 0; // Biến lưu thời gian đã trôi qua
    const unsigned long interval = 100;    // Thời gian chớp đèn (200ms)

    unsigned long currentMillis = millis(); // Thời gian hiện tại

    // Kiểm tra nếu đang thu âm
    if (isRecording) {
        // Kiểm tra xem đã đủ thời gian 200ms chưa
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;  // Cập nhật thời gian hiện tại

            // Đảo trạng thái của LED (từ ON thành OFF hoặc ngược lại)
            state = !state;

            // Điều khiển LED theo trạng thái (tắt hoặc bật)
            digitalWrite(2, state);
        }
    } else {
        digitalWrite(2, HIGH);  // Nếu không thu âm, giữ LED bật liên tục
    }
}
