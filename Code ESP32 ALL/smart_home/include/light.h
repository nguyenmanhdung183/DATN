#ifndef LIGHT_H
#define LIGHT_H
#include"device.h"
#include "wifi_and_firebase.h"
#include "button.h"
class Light : public Device {// 1 chân nối vcc, chân điều khiển kéo xuống low thì sáng
public:
  Light(int pin, String id, String name, String roomId, int buttonPin=-1, bool needFbUpdate=true) : Device(pin, id, name, roomId, buttonPin, needFbUpdate) {}
  void begin(){
      pinMode(getPin(), OUTPUT);
      digitalWrite(getPin(), LOW);
      setMainState(true);
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
#endif