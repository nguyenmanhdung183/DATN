package com.example.datn20213838

import android.util.Log
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

data class Room(
    val id: Int,         // ID duy nhất của phòng
    val name: String,       // Tên phòng ("Phòng khách", "Phòng ngủ")
    val devices: MutableList<Device> = mutableListOf() // Danh sách thiết bị trong phòng
)
data class Device(
    val id: Int,     // ID duy nhất của thiết bị
    val name: String,   // Tên thiết bị ("Đèn trần", "Quạt")
    val type: String,   // "light", "fan","ac"
    val topic: String,  // MQTT Topic dùng để gửi nhận dữ liệu
    var state: Boolean,  // (Bật/Tắt)
    var otherState:String?=""// trạng thái khác(nhiệt độ điều hoà)
)


/////////////////////////////////////DATA///
data class NotiData(
    val deviceId: String,
    val notiId:String,
    val hour: String,
    val minute: String,
    val day:String,
    val month:String,
    val year:String,
    val text: String,
    val isRead: Boolean = false
)

var roomList = mutableListOf(
    Room(
        id = 0,
        name = "Phòng khách",
        devices = mutableListOf(
            Device(id = 1, name = "Đèn trần", type = "light", topic = "home/livingroom/light1", state = false),
            Device(id = 2, name = "Quạt 1", type = "fan", topic = "home/livingroom/fan1", state = false),
            Device(id = 3, name = "Điều hoà", type = "ac", topic = "home/livingroom/ac1", state = false)
        )
    ),
    Room(
        id = 1,
        name = "Phòng ngủ",
        devices = mutableListOf(
            Device(id = 4, name = "Đèn ngủ", type = "light", topic = "home/bedroom/light1", state = false)
        )
    ),

)






fun getDevice(deviceId: Int): Device? {
    for (room in roomList) {
        for (device in room.devices) {
            if (device.id == deviceId) {
                return device
            }
        }
    }
    return null // Trả về null nếu không tìm thấy thiết bị
}



