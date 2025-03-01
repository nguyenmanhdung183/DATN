
package com.example.datn20213838

import android.util.Log
import androidx.compose.foundation.Image
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.ColorPainter
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.datn20213838.GlobalData.activeRoom

@Composable
fun DeviceEdit(deviceId:Int, nameOfDevice:String){
    Spacer(modifier = Modifier.padding(15.dp))
    Box(modifier = Modifier.fillMaxWidth().padding(start = 20.dp, end = 20.dp, top = 10.dp, bottom = 10.dp),
        contentAlignment = Alignment.Center){
        Text(
            text = "Sửa ${nameOfDevice}",
            // modifier = Modifier.align(Alignment.Center),
            fontSize = 30.sp,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.matchParentSize(),


            )
        Row(modifier = Modifier.size(100.dp).align(Alignment.BottomEnd),
            horizontalArrangement = Arrangement.SpaceAround,

            ){
            Image(
                painter = painterResource(id = R.drawable.save), contentDescription = "Icon",
                modifier = Modifier.size(36.dp).pointerInput(Unit){
                    detectTapGestures (
                        onTap = {
                            Log.d("ấn nút close", "ấn nút close")
                            //close
                            UpdateDevice(deviceId)
                        },

                        )
                }
            )

            Image(
                painter = painterResource(id = R.drawable.close), contentDescription = "Icon",
                modifier = Modifier.size(36.dp).padding(end=0.dp).pointerInput(Unit){
                    detectTapGestures (
                        onTap = {
                            Log.d("ấn nút Lưu thông tin", "ấn nút lưu thông tin")
                            //close
                            GoToDeviceList(activeRoom.value)

                        },

                        )
                }
            )
        }
    }
    //Noti
    Column (modifier = Modifier.verticalScroll(rememberScrollState())){
        Spacer(modifier = Modifier.padding(15.dp))
        // thêm các ô dữ liệu vào đây


    }
}

@Composable
fun AddDeviceUI(){
    Spacer(modifier = Modifier.padding(15.dp))
    Box(modifier = Modifier.fillMaxWidth().padding(start = 20.dp, end = 20.dp, top = 10.dp, bottom = 10.dp),
        contentAlignment = Alignment.Center){
        Text(
            text = "Thêm Thiết Bị",
            // modifier = Modifier.align(Alignment.Center),
            fontSize = 30.sp,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.matchParentSize(),


            )
        Row(modifier = Modifier.size(100.dp).align(Alignment.BottomEnd),
            horizontalArrangement = Arrangement.SpaceAround,

            ){
            Image(
                painter = painterResource(id = R.drawable.save), contentDescription = "Icon",
                modifier = Modifier.size(36.dp).pointerInput(Unit){
                    detectTapGestures (
                        onTap = {
                            Log.d("ấn nút close", "ấn nút close")
                            //close
                            SaveDevice()
                        },

                        )
                }
            )

            Image(
                painter = painterResource(id = R.drawable.close), contentDescription = "Icon",
                modifier = Modifier.size(36.dp).padding(end=0.dp).pointerInput(Unit){
                    detectTapGestures (
                        onTap = {
                            Log.d("ấn nút Lưu thông tin", "ấn nút lưu thông tin")
                            //close
                            GoToDeviceList(activeRoom.value)

                        },

                        )
                }
            )
        }
    }
    //Noti
    Column (modifier = Modifier.verticalScroll(rememberScrollState())){
        Spacer(modifier = Modifier.padding(15.dp))
        // thêm các ô dữ liệu vào đây


    }
}
