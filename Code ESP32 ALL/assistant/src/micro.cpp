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
        return;
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
    for (int i = 0; i < len; i += 2) {//Mỗi mẫu âm thanh chiếm 2 byte, nên duyệt từng mẫu một.
        /*
        -> trích 12-bit dữ liệu ADC từ 2 byte
        Giả sử:
        s_buff[i + 0] = 0xD2 = 1101 0010  (LSB - 8 bit thấp)
        s_buff[i + 1] = 0x0A = 0000 1010  (MSB - chứa 4 bit cao)

        Bước 1: Lấy 4 bit thấp từ MSB
        s_buff[i + 1] & 0x0F = 0000 1010

        Bước 2: Dịch trái 8 bit
        (0000 1010) << 8 = 0000 1010 0000 0000  = 0x0A00

        Bước 3: OR với LSB (ghép thành 12-bit ADC)
        0000 1010 0000 0000
        | 0000 0000 1101 0010
        --------------------------
        0000 1010 1101 0010  = 0x0AD2 = 2770 (decimal)

        → Kết quả cuối cùng: dac_value = 2770
        */
        dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | (s_buff[i + 0])));

        //Ghi mẫu 16-bit, byte cao là 0, byte thấp là dữ liệu chuẩn hóa
        //d_buff chứa chuỗi PCM 16-bit dạng [0x00][value8bit], [0x00][value8bit] -> Phù hợp với định dạng WAV 16-bit Mono
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048; // Từ 12-bit (0–2048) xuống 8-bit (0–255)
        /*
        -> từ 12-bit xuống 8-bit
        Giả sử:
        dac_value = 1024  (giá trị ADC 12-bit đọc được, nằm trong khoảng 0–2048)

        Bước 1: Scale xuống 8-bit
        scaled = 1024 * 256 / 2048 = 128

        Bước 2: Ghi vào buffer 16-bit PCM:
        d_buff[j++] = 0     → byte cao (MSB)
        d_buff[j++] = 128   → byte thấp (LSB)

        ⇒ Kết quả trong d_buff:
        [0x00] [0x80] = 0x0080 = 128 (giá trị âm thanh chuẩn hóa dưới dạng 16-bit, nhưng chỉ dùng 8 bit thấp)

        */
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

    // Đọc dữ liệu ban đầu để ổn định vì ADC có thể chưa ổn định ngay khi khởi động
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

    Serial.println(" *** Recording Start *** ");
    isRecording = true;

    Led();

    // Ghi dữ liệu âm thanh vào wav_buffer
    while (flash_wr_size < wav_buffer_size) {// -> ghi 4Kb 1 lần
        Led();
         // Đọc dữ liệu âm thanh từ I2S vào bộ đệm i2s_read_buff
        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

         // Chuẩn hóa và chuyển đổi dữ liệu đọc được sang định dạng phù hợp, lưu vào flash_write_buff
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
    // ChunkID "RIFF" (4 bytes)
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';

    // ChunkSize (4 bytes): tổng kích thước file - 8 bytes (header không tính 8 byte đầu do khởi động)
    unsigned int fileSize = wavSize + HEADER_SIZE - 8;
    header[4] = (byte)(fileSize & 0xFF);           // byte thấp nhất
    header[5] = (byte)((fileSize >> 8) & 0xFF);
    header[6] = (byte)((fileSize >> 16) & 0xFF);
    header[7] = (byte)((fileSize >> 24) & 0xFF);   // byte cao nhất

    // Format "WAVE" (4 bytes)
    header[8] = 'W';
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';

    // Subchunk1ID "fmt " (4 bytes) - phần định dạng
    header[12] = 'f';
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';

    // Subchunk1Size (4 bytes) - kích thước phần fmt, 16 bytes cho PCM
    header[16] = 0x10;
    header[17] = 0x00;
    header[18] = 0x00;
    header[19] = 0x00;

    // AudioFormat (2 bytes) - 1 nghĩa là PCM (Linear quantization)
    header[20] = 0x01;
    header[21] = 0x00;

    // NumChannels (2 bytes) - số kênh, 1 = mono
    header[22] = 0x01;
    header[23] = 0x00;

    // SampleRate (4 bytes) - tần số lấy mẫu (little endian)
    // 16000 Hz = 0x00003E80 -> 0x80 0x3E 0x00 0x00
    header[24] = 0x80;
    header[25] = 0x3E;
    header[26] = 0x00;
    header[27] = 0x00;

    // ByteRate (4 bytes) = SampleRate * NumChannels * BitsPerSample/8
    // 16000 * 1 * 16/8 = 32000 = 0x00007D00 -> 0x00 0x7D 0x00 0x00
    header[28] = 0x00;
    header[29] = 0x7D;
    header[30] = 0x00;
    header[31] = 0x00;

    // BlockAlign (2 bytes) = NumChannels * BitsPerSample/8
    // 1 * 16/8 = 2 bytes mỗi mẫu
    header[32] = 0x02;
    header[33] = 0x00;

    // BitsPerSample (2 bytes) - 16 bit
    header[34] = 0x10;
    header[35] = 0x00;

    // Subchunk2ID "data" (4 bytes) - phần dữ liệu âm thanh
    header[36] = 'd';
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';

    // Subchunk2Size (4 bytes) - kích thước phần dữ liệu âm thanh
    header[40] = (byte)(wavSize & 0xFF);
    header[41] = (byte)((wavSize >> 8) & 0xFF);
    header[42] = (byte)((wavSize >> 16) & 0xFF);
    header[43] = (byte)((wavSize >> 24) & 0xFF);
}
