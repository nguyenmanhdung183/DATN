#include "micro.h"
#include "wit.h"
#include "wifi_and_firebase.h"
#include "process_action.h"
#include"led_button.h"
extern uint8_t* wav_buffer; // Bộ đệm lưu dữ liệu WAV
extern bool isRecording;
const int BUTTON_PIN = 12;



void setup() {
    Serial.begin(115200);
    // Khởi tạo WiFi và Firebase
    setupWifiFirebase();
    // Khởi tạo WIT

    
    initLed();
    initButton(BUTTON_PIN);// chân 12 làm input
    i2sInit();
    initBuzzer(27); // Chân 27 làm output cho buzzer

    Led();
    Serial.println("Done Init");

    #if 0
    WIT wit;
    // Khởi tạo bộ đệm WAV và I2S
    initWavBuffer();

    initLed();
    initButton(BUTTON_PIN);// chân 12 làm input

    i2sInit();
    // Ghi âm
    i2s_adc(nullptr);
    // Gửi dữ liệu WAV từ wav_buffer
    wit.sentData(wav_buffer, wav_buffer_size);
    // Xử lý hành động dựa trên dữ liệu từ WIT và Firebase
    Action action(wit.getData(), fetchJsonFirebase());
    action.decide();

    Serial.println("Processing action...before delete wav_buffer");

     free(wav_buffer);
     wav_buffer = nullptr;
    Serial.println("Done!");

    #endif
}

void loop() {
    if(getPressedButton(BUTTON_PIN) && !isRecording ){
        delay(1000);
        // Buzzer kêu 1 lần
        playBuzzer(27, 1);
        delay(200);
  //  if(((millis()>2000 && millis()<3000) || (millis()>20000 && millis()<21000)) && !isRecording ){
        WIT wit;
        initWavBuffer();

        i2s_adc(nullptr);
        if (wav_buffer != nullptr && wav_buffer_size > 0) {
            wit.sentData(wav_buffer, wav_buffer_size);
        } else {
            Serial.println("Error: wav_buffer is empty or not initialized");
        }
        free(wav_buffer);
        wav_buffer = nullptr;


        Action action(wit.getData(), fetchJsonFirebase());
        action.decide();

        // Giải phóng bộ đệm WAV sau khi xử lý xong
        // free(wav_buffer);
        // wav_buffer = nullptr;

    }
}