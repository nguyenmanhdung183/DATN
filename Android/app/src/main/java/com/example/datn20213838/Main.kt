package com.example.datn

import android.util.Log
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.datn20213838.AddDeviceUI
import com.example.datn20213838.AddRoomUI
import com.example.datn20213838.DeviceEdit
import com.example.datn20213838.GlobalData.activeRoom
import com.example.datn20213838.GlobalData.activeTaskbar
import com.example.datn20213838.GlobalData.controlAriconditioner
import com.example.datn20213838.GlobalData.edittingDevice
import com.example.datn20213838.GlobalData.edittingRoom
import com.example.datn20213838.RoomEdit
import com.example.datn20213838.roomList


@Composable
fun Main(){//taskbar khai báo rồi ở chỗ Main activity:)))))
    Column (modifier = Modifier.padding(10.dp)){
        Column (modifier = Modifier.weight(1f)){
            //home, noti, setting, editroom, editdevice, addroom, adddevicem, airconditioner, devicelist
            if(activeTaskbar.value=="home") HomeScreen()
            if(activeTaskbar.value=="noti") NotiMain()
            if(activeTaskbar.value=="setting") SetupUI()
            if(activeTaskbar.value=="devicelist") ControlMain(activeRoom.value,roomList[activeRoom.value].name)
            if(activeTaskbar.value=="editroom") RoomEdit(edittingRoom.value,roomList[edittingRoom.value].name)
            if(activeTaskbar.value=="addroom") AddRoomUI()
            if(activeTaskbar.value=="adddevice") AddDeviceUI()
            if(activeTaskbar.value=="airconditioner") AirConditionerControl(controlAriconditioner.value,FindName(edittingDevice.value))
            if(activeTaskbar.value=="editdevice") DeviceEdit(edittingDevice.value, FindName(edittingDevice.value))
            if(activeTaskbar.value=="editroom") RoomEdit(edittingRoom.value,roomList[edittingRoom.value].name)

        }
        Taskbar()
    }
}
fun FindName(id:Int):String{
    for(room in roomList){
        for(device in room.devices){
            if (device.id==id) return device.name
        }
    }
    return ""
}