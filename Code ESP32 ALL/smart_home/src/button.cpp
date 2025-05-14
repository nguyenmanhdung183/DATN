#include "button.h"

int numButtons;

std::vector<int> buttonPins ={};  // Chân các nút bấm
std::vector<bool> buttonStates;
std::vector<bool> lastButtonStates;
std::vector<unsigned long> lastDebounceTime;


const unsigned long debounceDelay = 20;  // Thời gian chống dội phím

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