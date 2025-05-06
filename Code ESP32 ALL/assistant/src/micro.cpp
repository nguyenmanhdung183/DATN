#include "micro.h"
#include "led_button.h"
uint8_t* wav_buffer = nullptr; // Bộ đệm lưu dữ liệu WAV
int wav_buffer_size = 0;       // Kích thước bộ đệm WAV
bool isRecording = false; // Biến toàn cục để kiểm soát trạng thái ghi âm

// Khởi tạo bộ đệm WAV trong RAM
void initWavBuffer() {
    wav_buffer_size = FLASH_RECORD_SIZE + HEADER_SIZE; // Kích thước bao gồm header

    // In thông tin heap để debug
    Serial.printf("Free heap before allocation: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("wav_buffer_size: %d bytes\n", wav_buffer_size);

    // Cấp phát bộ nhớ cho wav_buffer
    wav_buffer = (uint8_t*) malloc(wav_buffer_size);
    if (wav_buffer == nullptr) {
        Serial.println("Memory allocation failed for wav_buffer!");
        while (true) {
            delay(1000); // Dừng chương trình nếu lỗi
        }
    }

    Serial.printf("Free heap after allocation: %u bytes\n", ESP.getFreeHeap());

    // Tạo và sao chép header WAV
    byte header[HEADER_SIZE];
    wavHeader(header, FLASH_RECORD_SIZE);
    memcpy(wav_buffer, header, HEADER_SIZE);
}

void i2sInit() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = 0,
        .dma_buf_count = 32,  // Giảm để tiết kiệm bộ nhớ
        .dma_buf_len = 512,   // Giảm để tiết kiệm bộ nhớ
        .use_apll = 1
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, nullptr);

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
    };

    i2s_set_pin(I2S_PORT, &pin_config);
}

void i2s_adc_data_scale(uint8_t *d_buff, uint8_t* s_buff, uint32_t len) {
    uint32_t j = 0;
    uint32_t dac_value = 0;
    for (int i = 0; i < len; i += 2) {
        dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | (s_buff[i + 0])));
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048; // Chuẩn hóa dữ liệu âm thanh
    }
}

void i2s_adc(void *arg) {
    int i2s_read_len = I2S_READ_LEN;
    int flash_wr_size = HEADER_SIZE; // Bắt đầu sau header
    size_t bytes_read;

    // Kiểm tra wav_buffer
    if (wav_buffer == nullptr) {
        Serial.println("wav_buffer is NULL! Cannot proceed with recording.");
        return;
    }

    // Cấp phát bộ đệm I2S
    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

    if (i2s_read_buff == nullptr || flash_write_buff == nullptr) {
        Serial.println("Memory allocation failed for I2S buffers!");
        free(i2s_read_buff);
        free(flash_write_buff);
        return;
    }

    // Đọc dữ liệu ban đầu để ổn định
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

    Serial.println(" *** Recording Start *** ");
    isRecording = true;

    Led();

    // Ghi dữ liệu âm thanh vào wav_buffer
    while (flash_wr_size < wav_buffer_size) {
        Led();

        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
        i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, i2s_read_len);
    
        int remaining = wav_buffer_size - flash_wr_size;
        int copy_len = (i2s_read_len < remaining) ? i2s_read_len : remaining;
    
        memcpy(wav_buffer + flash_wr_size, flash_write_buff, copy_len);
        flash_wr_size += copy_len;
    
        Serial.printf("Sound recording %u%%\n", flash_wr_size * 100 / wav_buffer_size);
    }
    
    
    // Giải phóng bộ đệm
    free(i2s_read_buff);
    free(flash_write_buff);
    Serial.println("Recording Finished");
    isRecording = false;
    Led();

    Serial.printf("wav_buffer is ready with %d bytes of WAV data\n", wav_buffer_size);
}

void wavHeader(byte* header, int wavSize) {
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    unsigned int fileSize = wavSize + HEADER_SIZE - 8;
    header[4] = (byte)(fileSize & 0xFF);
    header[5] = (byte)((fileSize >> 8) & 0xFF);
    header[6] = (byte)((fileSize >> 16) & 0xFF);
    header[7] = (byte)((fileSize >> 24) & 0xFF);
    header[8] = 'W';
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f';
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 0x10;
    header[17] = 0x00;
    header[18] = 0x00;
    header[19] = 0x00;
    header[20] = 0x01;
    header[21] = 0x00;
    header[22] = 0x01;
    header[23] = 0x00;
    header[24] = 0x80;
    header[25] = 0x3E;
    header[26] = 0x00;
    header[27] = 0x00;
    header[28] = 0x00;
    header[29] = 0x7D;
    header[30] = 0x00;
    header[31] = 0x00;
    header[32] = 0x02;
    header[33] = 0x00;
    header[34] = 0x10;
    header[35] = 0x00;
    header[36] = 'd';
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (byte)(wavSize & 0xFF);
    header[41] = (byte)((wavSize >> 8) & 0xFF);
    header[42] = (byte)((wavSize >> 16) & 0xFF);
    header[43] = (byte)((wavSize >> 24) & 0xFF);
}