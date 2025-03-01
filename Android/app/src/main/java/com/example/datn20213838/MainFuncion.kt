package com.example.datn20213838

import android.util.Log
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import com.example.datn.ClearNotiList
import com.example.datn.deleteAllNotificationsFromDTB
import com.example.datn.geUpdateNoti
import com.example.datn20213838.GlobalData.URLFB
import com.example.datn20213838.GlobalData.activeRoom
import com.example.datn20213838.GlobalData.activeTaskbar
import com.example.datn20213838.GlobalData.controlAriconditioner
import com.example.datn20213838.GlobalData.edittingDevice
import com.example.datn20213838.GlobalData.edittingRoom
import com.google.firebase.database.FirebaseDatabase

fun Logdata(){
    Log.d("L - activeTaskbar", GlobalData.activeTaskbar.value)
    Log.d("L - activeRoom", GlobalData.activeRoom.value.toString())
    Log.d("L - haveNotis", GlobalData.haveNotis.value.toString())
    Log.d("L - controlAriconditioner", GlobalData.controlAriconditioner.value.toString())
    Log.d("L - edittingRoom", GlobalData.edittingRoom.value.toString())
    Log.d("L - edittingDevice", GlobalData.edittingDevice.value.toString())
}

fun DataReset(){
    controlAriconditioner.value = -1
    GlobalData.activeRoom.value = -1
    edittingRoom.value=-1
    edittingDevice.value=-1
    activeRoom.value=-1
    //GlobalData.haveNotis.value = true
}

fun BackToHomeScreen(){
    GlobalData.activeTaskbar.value = "home"
    DataReset()
    //GlobalData.haveNotis.value = true
}

fun BackToNotiScreen(){
    GlobalData.activeTaskbar.value = "noti"
    DataReset()

    //GlobalData.haveNotis.value = false
}

fun BackToSettingScreen(){
    GlobalData.activeTaskbar.value = "setting"
    DataReset()

    //GlobalData.haveNotis.value = true
}
fun GoToDeviceList(idRoom:Int){
    DataReset()
    activeRoom.value=idRoom
    activeTaskbar.value = "devicelist"
    Logdata()
}

fun GoToAirConditionerControlScreen(id:Int) {//UI
    val temp = activeRoom.value
    DataReset()
    activeRoom.value = temp
    activeTaskbar.value = "airconditioner"
    controlAriconditioner.value = id
    Logdata()
}


fun TurnOn(id: Int){
    Log.d("On", "On")
    SwitchStatePower(id)
}


fun TurnOff(id: Int){
    Log.d("Off", "Off")
    SwitchStatePower(id)

}

fun ChangeTemperature(int: Int, action:String){//up down left right on off
    Log.d("ChangeTemperature", "ChangeTemperature")
    if(action=="up") {
        ControlData(int, "up")
    }else if(action=="down"){
        ControlData(int, "down")
    }else if(action == "right"){
        ControlData(int, "right")

    }else if(action == "left"){
        ControlData(int, "left")

    }
    else if(action == "on"){
        SwitchStatePower(int)
    }else if (action=="off"){
        SwitchStatePower(int)
    }
}

fun DeleteDevice(){
    Log.d("DeleteDevice", "DeleteDevice")
}


fun DeleteRoom(){
    Log.d("DeleteRoom", "DeleteRoom")
    DataReset()
    activeTaskbar.value="deleteroom"

}


fun AddDevice(){//UI
    Log.d("AddDevice", "AddDevice")
    activeTaskbar.value="adddevice"
}
fun SaveDevice(){// chưa xong
    GoToDeviceList(activeRoom.value)
    ////
}
fun UpdateDevice(deviceId:Int){// nút save cho update
    GoToDeviceList(activeRoom.value)
}

fun AddRoom(){// xử lý nút bấm + thôi
    Log.d("AddRoom", "AddRoom")
    DataReset()
    activeTaskbar.value="addroom"
}
fun SaveRoom(){// chưa xong
    BackToHomeScreen()

    //xử lý data
}


fun Edit(idDevice:Int){//deit device
    Log.d("Edit", "Edit")
    val temp= activeRoom.value
    Logdata()
    DataReset()
    activeRoom.value=temp
    activeTaskbar.value="editdevice"
    edittingDevice.value=idDevice
}

fun EditRoom(id:Int){
    Log.d("ấn nút chỉnh sửa phòng", "ấn nút chỉnh sửa phòng ${id}")
    DataReset()
    activeTaskbar.value="editroom"
    edittingRoom.value=id



}

fun DeleteNoti(){
    Log.d("DeleteNoti", "DeleteNoti")
    deleteAllNotificationsFromDTB()
    ClearNotiList()
}

//////TEST

fun Test() {
    Log.d("Test", "Test")
    geUpdateNoti()
}

fun SwitchStatePower(deviceId: Int) {
    Log.d("SwitchState", "${deviceId.toString()}")
    // Lấy thiết bị 1 lần
    val device = getDevice(deviceId)

    if (device != null) {
        device.state = !device.state  // Đảo trạng thái của thiết bị
        Log.d("DeviceState", "Device ID: $deviceId, State: ${device.state}")

        if (device.state) {
            sendDataToFirebase("on", deviceId)
        } else {
            sendDataToFirebase("off", deviceId)
        }
    } else {
        Log.d("DeviceState", "Không tìm thấy thiết bị với ID: $deviceId")
    }
}


var count_=0
fun ControlData(deviceId: Int, action: String){
    Log.d("Control Action", "${deviceId.toString()}")
    // Lấy thiết bị 1 lần
    val device = getDevice(deviceId)


    if(action=="up" || action=="down"||action=="left"|| action=="right"){count_=count_+1}
    if (device != null) {
       //
        var  r="room"
        var  d="device"
        for (room in roomList) {
            for (device in room.devices) {
                if (device.id == deviceId) {
                    r = r + room.id.toString()
                    d = d + device.id.toString()
                }
            }
        }

        val database = FirebaseDatabase.getInstance(URLFB.value)
            .reference.child(r).child(d).child("action")
        database.setValue(action).addOnSuccessListener {
            println("Gửi dữ liệu thành công!")
        }.addOnFailureListener {
            println("Lỗi khi gửi dữ liệu: ${it.message}")
        }


        val database2 = FirebaseDatabase.getInstance(URLFB.value)
            .reference.child(r).child(d).child("count")
        database2.setValue(count_.toString()).addOnSuccessListener {
            println("Gửi dữ liệu thành công!")
        }.addOnFailureListener {
            println("Lỗi khi gửi dữ liệu: ${it.message}")
        }

    } else {
        Log.d("DeviceState", "Không tìm thấy thiết bị với ID: $deviceId")
    }
}



fun getUpdateState(){

}