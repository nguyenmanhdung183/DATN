#ifndef FAN_H
#define FAN_H
#include "device.h"
#include "wifi_and_firebase.h"
#include "button.h"

class Fan : public Device {
public:
  Fan(int pin, String id, String name, String roomId, int buttonPin =-1, bool needFbUpdate =true) : Device(pin, id, name, roomId, buttonPin, needFbUpdate) {

  }
  void begin() {  // Hàm này thay thế việc setup pinMode
    pinMode(getPin(), OUTPUT);
    digitalWrite(getPin(), LOW);
    setMainState(false);
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

#endif