#include"wit.h"

std::string handle_room_name(std::string room){
    if(room.find("phòng") != std::string::npos){
        room.erase(room.find("phòng"), 7);
    }

    return room;
}