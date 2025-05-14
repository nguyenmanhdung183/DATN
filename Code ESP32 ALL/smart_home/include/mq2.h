#ifndef MQ2_H
#define MQ2_H
#include <Arduino.h>
#include "wifi_and_firebase.h"
#include "device.h"
#include "button.h"


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
    

#endif