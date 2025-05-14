#ifndef AC_H
#define AC_H
#include "device.h"
#include "wifi_and_firebase.h"
#include "button.h"

class AirConditioner : public Device {
  public:
      AirConditioner(int pin, String id, String name, String roomId, int buttonPin=-1, bool needFBUpdate=true) : Device(pin, id, name, roomId, buttonPin, needFBUpdate) {}

        void begin() {
            pinMode(getPin(), OUTPUT);
            digitalWrite(getPin(), LOW);
        }
        void turnOnAC() {
            if(!getMainState()) {
                setMainState(true);
                digitalWrite(getPin(), HIGH);
                Serial.println("❄️ " + getId() + " ON");
            }
      }
      void turnOffAC() {
            if(getMainState()) {
                setMainState(false);
                digitalWrite(getPin(), LOW);
                Serial.println("❄️ " + getId() + " OFF");
            }
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
          sendOtherStateToFirebase();
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
    void FbUpdate(String st){
    if(getMainState()&& st=="off" ){// nếu đang on
      turnOffAC();
    }
    else if(!getMainState() && st=="on"){// nếu đang off
      turnOnAC();
    }
  }
  };

#endif