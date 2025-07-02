#ifndef MICRO_H
#define MICRO_H
#include <driver/i2s.h>
#include <Arduino.h>

#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14

#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000) // Tần số mẫu 16kHz
#define I2S_SAMPLE_BITS   (16) // Số bit mẫu 16 bit
#define I2S_READ_LEN      (4 * 1024) // đọc 4KB mỗi lần
#define RECORD_TIME       (2.5) // Seconds
#define I2S_CHANNEL_NUM   (1) // Số kênh âm thanh (1 kênh cho mono)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME) // Kích thước ghi vào flash (tính theo byte)
#define HEADER_SIZE       (44) // Kích thước header WAV (44 byte)

// Biến toàn cục cho bộ đệm WAV
extern uint8_t* wav_buffer;
extern int wav_buffer_size;

void wavHeader(byte* header, int wavSize);  // Hàm tạo header WAV
void i2s_adc_data_scale(uint8_t *d_buff, uint8_t* s_buff, uint32_t len); //hàm scale 116-> 8 bit
void initWavBuffer(void); // Hàm khởi tạo bộ đệm WAV
void i2sInit(void); // Hàm khởi tạo I2S
void i2s_adc(void *arg);// Hàm đọc dữ liệu từ ADC qua I2S

#endif // MICRO_H