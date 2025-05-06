import serial
import time
import os

SERIAL_PORT = 'COM4'  # Thay bằng cổng của bạn
BAUD_RATE = 115200
#WAV_FILE = 'bdpk.wav'  # File WAV mono, 16-bit, 16kHz
WAV_FILE = os.path.join(os.getcwd(), "scripts/bdpk.wav")  # Đảm bảo đường dẫn chính xác

def send_wav():
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)

    with open(WAV_FILE, 'rb') as f:
        wav_data = f.read()

    print(f"Sending {len(wav_data)} bytes...")
    ser.write(wav_data)
    ser.flush()

    while True:
        if ser.in_waiting > 0:
            print(ser.readline().decode('utf-8').strip())
        time.sleep(0.1)

    ser.close()

if __name__ == "__main__":
    send_wav()