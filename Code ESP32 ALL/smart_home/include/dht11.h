#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>
#include "DHTesp.h" 

class DHT11{
    private:
        int pin;
        DHTesp dht;
        float temperature = 0;
        float humidity = 0;
    public:
        DHT11(int pin) {
            this->pin = pin;
        }
        void begin() {
            dht.setup(pin, DHTesp::DHT11);
        }
        void readSensor() {
            temperature = dht.getTemperature();
            humidity = dht.getHumidity();
        }
        float getTemperature() {
            return temperature;
        }
        float getHumidity() {
            return humidity;
        }
};
#endif
