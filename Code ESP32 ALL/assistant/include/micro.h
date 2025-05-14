#ifndef MICRO_H
#define MICRO_H
#include <driver/i2s.h>
#include <Arduino.h>

#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14

#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (4 * 1024) // Giảm kích thước bộ đệm đọc
#define RECORD_TIME       (2.5) // Seconds
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)
#define HEADER_SIZE       (44)

// Biến toàn cục cho bộ đệm WAV
extern uint8_t* wav_buffer;
extern int wav_buffer_size;

void wavHeader(byte* header, int wavSize);
void i2s_adc_data_scale(uint8_t *d_buff, uint8_t* s_buff, uint32_t len);
void initWavBuffer(void);
void i2sInit(void);
void i2s_adc(void *arg);

#endif // MICRO_H