#ifndef PROCESS_ACTION_H
#define PROCESS_ACTION_H

#include <Arduino.h>
#include "wit.h"
#include "wifi_and_firebase.h"
#include <ArduinoJson.h>

class Action {
private:
    Data dt;
    String path;
    StaticJsonDocument<256> fb_json;

public:
    Action(Data data, String fb) : dt(data) {
        if(data.room == "" || data.device == ""|| data.status=="") return;
        DeserializationError error = deserializeJson(fb_json, fb);
        if (error) return;

    }

    String lower_shoter(String r) {
        String rt = r;
        rt.toLowerCase();
        rt.replace("phòng", "");
        rt.replace("phong", "");
        rt.trim();
        return rt;
    }

    String findIdFromRoomName(String roomName) {
        for (JsonPair room : fb_json.as<JsonObject>()) {
            if (room.value().containsKey("roomname") && lower_shoter(room.value()["roomname"]) == roomName) {
                return room.key().c_str();
            }
        }
        return "null";
    }

    String findIdFromDeviceName(String deviceName, String roomName) {
        String type = "null";
        deviceName.toLowerCase();
        if (deviceName == "đèn") type = "light";
        else if (deviceName == "quạt") type = "fan";
        else if (deviceName == "điều hoà" || deviceName == "máy lạnh") type = "ac";
        else if (deviceName == "tivi") type = "tv";
        else return "null";

        for (JsonPair room : fb_json.as<JsonObject>()) {
            if (room.value().containsKey("roomname") && lower_shoter(room.value()["roomname"]) == roomName) {
                for (JsonPair device : room.value().as<JsonObject>()) {
                    if (device.value().containsKey("devicetype") && device.value()["devicetype"] == type) {
                        return device.key().c_str();
                    }
                }
            }
        }
        return "null";
    }

    void decide() {
        if(dt.room == "" || dt.device == "" || dt.status == "") return; 
        
        String roomId = findIdFromRoomName(dt.room);
        String deviceId = findIdFromDeviceName(dt.device, dt.room);
        
        if (roomId == "null" || deviceId == "null") return;

        if (dt.status == "bật") {
            path = "/" + roomId + "/" + deviceId + "/status";
            uploadData(path, "on");
        } else if (dt.status == "tắt") {
            path = "/" + roomId + "/" + deviceId + "/status";
            uploadData(path, "off");
        }
    }
};

#endif