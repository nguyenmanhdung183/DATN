#include "button.h"

int numButtons;

extern std::vector<int> buttonPins;  // Chân các nút bấm
std::vector<bool> buttonStates;
std::vector<bool> lastButtonStates;
std::vector<unsigned long> lastDebounceTime;


const unsigned long debounceDelay = 20;  // Thời gian chống dội phím
extern volatile int lastPressedButton;

void buttonBegin();
int getPressedButton();
void buttonTask(void *pvParameters); //Task FreeRTOS cho xử lý nút bấm

void buttonBegin() {
  numButtons = buttonPins.size();
  buttonStates.resize(numButtons, false);
  lastButtonStates.resize(numButtons, false);
  lastDebounceTime.resize(numButtons, 0); 

    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT);
    }

    xTaskCreate(
        buttonTask,        // Hàm task
        "ButtonTask",      // Tên task
        2048,              // Kích thước stack (byte)
        NULL,              // Tham số task
        1,                 // Độ ưu tiên (1 là thấp, số cao hơn ưu tiên hơn)
        NULL               // Handle của task
    );
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
            Serial.println("Button " + String(buttonPins[i]) + " pressed");
                  rt = buttonPins[i];
          }
          buttonStates[i] = currentState;

      }

      lastButtonStates[i] = currentState;
  }

  return rt;  // Không có nút nào được nhấn
}

// Task FreeRTOS để xử lý chống dội và phát hiện nút bấm
void buttonTask(void *pvParameters) {
    while (true) {
        int pressedPin = getPressedButton();
        if (pressedPin != -1) {
            lastPressedButton = pressedPin; // Lưu chân của nút vừa nhấn
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Nhường CPU trong 10ms
    }
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