#include "string.h"
#include <vector>
#include <wifi_and_firebase.h>
#include "button.h"
#include "device.h"
#include "light.h"
#include "fan.h"
#include "mq2.h"
#include "ac.h"
#include "fm52.h"
#include "dht11.h"
//std::vector<int> buttonPins ={};  // Chân các nút bấm
// ko upload state trong class

#define PHONG_BEP "23714458"
#define PHONG_NGU "323238991"
#define PHONG_KHACH "374566167"


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

std::vector<Device*> Device::dvList;
Fan fanBep(27, "82322702", "Quạt 1", PHONG_BEP, 26);// 1 chan noi 5v 1 chan noi BJT cuc C 
Light lightBep(25, "903872522", "Đèn 1", PHONG_BEP, 33);// 1 chân nối vcc (25), chân điều khiển kéo xuống low thì sáng
Light lightHall(12, "null", "Đèn 2", PHONG_KHACH, -1,false);
Light lightKhach(14, "254010416", "Đèn 3", PHONG_KHACH, 34);
Light lighNgu(22, "274878463", "Đèn 5", PHONG_NGU, 4); // chân 4 là chân điều khiển, chân 22 là chân nối dây âm
//Light lightGarage(23, "null", "Đèn 4", PHONG_KHACH, -1,false);
AirConditioner airC(18, "716502344", "Điều hòa", PHONG_NGU, -1);

MQ2 mq2(-1,32, 200);// A, D, Threshold, dùng Analog cho đẹp :)))) (cảm biến gas) 2 chan nguon 5v, 1 chan tin hieu
FM52 fm52(35);// chân digital (cảm biến tiệm cận) 2 chan nguon 3.3v, 1 chasn tin hieu
DHT11 dht11(23); // chân 23 làm tín hiệu input

void setup() {
  setupWifiFirebase();
  Serial.begin(115200);
  fanBep.begin();
  lightBep.begin();
  lightHall.begin();
  lightKhach.begin();
  lighNgu.begin();
  mq2.begin();
  fm52.begin();
  dht11.begin();
  airC.begin();
  buttonBegin();
}

void loop() {

  StreamData(Device::dvList);

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

  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastTime > 2345) { 
    lastTime = currentTime;
    dht11.readSensor();
   // String st = String(dht11.getTemperature());
    airC.setTemperature(String(dht11.getTemperature()));
   // airC.sendOtherStateToFirebase();
  }

  switch(getPressedButton()){
    case 33:
      lightBep.buttonPress();
      break;
    case 26:
      fanBep.buttonPress();
      break;
    case 4:
      lighNgu.buttonPress();
      break;
    case 34:
      lightKhach.buttonPress();
      break;

    default:
      break;
  }

}
