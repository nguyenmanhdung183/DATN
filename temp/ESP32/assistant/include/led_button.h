#ifndef LED_BUTTON_H
#define LED_BUTTON_H

void initLed();
void Led();


void initButton(int buttonPin);
bool getPressedButton(int buttonPin);

#endif// LED_BUTTON_H