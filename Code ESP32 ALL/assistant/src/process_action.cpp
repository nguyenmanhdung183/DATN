#include "process_action.h"

bool on_off(Data data) {
    return data.status == "bật";
}

String get_device(Data data) {
    String device = data.device;
    if (device == "điều hòa" || device == "điều hòa không khí" || device == "máy lạnh" || device == "máy lạnh không khí") return "ac";
    if (device == "quạt" || device == "quạt điện") return "fan";
    if (device == "đèn" || device == "đèn điện" || device == "bóng đèn" || device == "bóng đèn điện") return "light";
    return "null";
}