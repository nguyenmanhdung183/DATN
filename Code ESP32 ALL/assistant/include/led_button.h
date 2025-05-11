#ifndef LED_BUTTON_H
#define LED_BUTTON_H

void initLed();
void Led();


void initButton(int buttonPin);
bool getPressedButton(int buttonPin);


void initBuzzer(int buzzerPin);
void playBuzzer(int buzzerPin, int time);


#endif// LED_BUTTON_H