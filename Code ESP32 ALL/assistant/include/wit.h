#ifndef WIT_H
#define WIT_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <HTTPClient.h>

struct Data {
    String room="";
    String device="";
    String status="";
};

class WIT {
private:
    Data dt;
    const char* WIT_AI_TOKEN = "Bearer XTZHEHVKIEDWYAT4HOCIL6PESBQKE24K";
   // const char* url = "https://api.wit.ai/speech?v=20200927";

    HTTPClient http;

    String handle_room_name(String room) {
        if (room.indexOf("phòng") != -1) {
            room.replace("phòng", "");
        }
        room.trim(); // Xóa khoảng trắng đầu và cuối
        return room;
    }



public:
    WIT() {}

    void sentData(uint8_t* audioData, int fileSize) {
        //size_t fileSize = file.size();
        http.begin("https://api.wit.ai/speech?v=20200927");
        http.addHeader("Authorization", WIT_AI_TOKEN);
        http.addHeader("Content-Type", "audio/wav");


        int httpResponseCode = http.POST(audioData, fileSize);
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Phản hồi từ Wit.ai:");
            Serial.println(response);
            //rp= response; //  data dạng string

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, response);

            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.f_str());
                return;
            }
            // Truy cập các trường trong JSON
            /*
            const char* room = doc["entities"]["room:room"][0]["value"];
            const char* device = doc["entities"]["device:device"][0]["value"];
            const char* status = doc["entities"]["state:state"][0]["value"];


            dt.room = handle_room_name(room);
            dt.device = device;
            dt.status = status;
            */

                    // Kiểm tra sự tồn tại của các trường trong JSON
        if (doc["entities"].containsKey("room:room") && doc["entities"]["room:room"].is<JsonArray>() && doc["entities"]["room:room"][0].containsKey("value")) {
            const char* room = doc["entities"]["room:room"][0]["value"];
            dt.room = handle_room_name(room);
        } else {
            Serial.println("Không tìm thấy thông tin phòng trong phản hồi.");
        }

        if (doc["entities"].containsKey("device:device") && doc["entities"]["device:device"].is<JsonArray>() && doc["entities"]["device:device"][0].containsKey("value")) {
            const char* device = doc["entities"]["device:device"][0]["value"];
            dt.device = device;
        } else {
            Serial.println("Không tìm thấy thông tin thiết bị trong phản hồi.");
        }

        if (doc["entities"].containsKey("state:state") && doc["entities"]["state:state"].is<JsonArray>() && doc["entities"]["state:state"][0].containsKey("value")) {
            const char* status = doc["entities"]["state:state"][0]["value"];
            dt.status = status;
        } else {
            Serial.println("Không tìm thấy thông tin trạng thái trong phản hồi.");
        }
        
            Serial.println(dt.room.c_str());
            Serial.println(dt.device.c_str());
            Serial.println(dt.status.c_str());

        } else {
            Serial.printf("Lỗi gửi POST: %d\n", httpResponseCode);
        }
        //delete[] audioData;

        http.end();

    }

    
    // String getRoom() { return dt.room; }
    // String getDevice() { return dt.device; }
    // String getStatus() { return dt.status; }
     Data getData() { return dt; }
    // void resetData() {
    //     dt.room = "";
    //     dt.device = "";
    //     dt.status = "";
    // }
};

#endif