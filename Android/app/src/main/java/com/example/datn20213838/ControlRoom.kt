package com.example.datn

import android.util.Log
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Image
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column

import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.ColorPainter
import androidx.compose.ui.input.pointer.motionEventSpy
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.datn20213838.AddDevice
import com.example.datn20213838.ChangeTemperature
import com.example.datn20213838.Edit
import com.example.datn20213838.GlobalData.activeRoom
import com.example.datn20213838.GoToAirConditionerControlScreen
import com.example.datn20213838.GoToDeviceList
import com.example.datn20213838.R
import com.example.datn20213838.TurnOff
import com.example.datn20213838.TurnOn
import com.example.datn20213838.UpdateDevice
import com.example.datn20213838.getDevice
import com.example.datn20213838.listenToFirebase
import com.example.datn20213838.roomList
import kotlinx.coroutines.flow.combine

@Composable

fun ControlMain(roomId:Int, roomName:String){
    Column(modifier = Modifier.padding(10.dp)){
        //Text
        Spacer(modifier = Modifier.padding(15.dp))
        Row(
            modifier = Modifier
                .padding(10.dp)
                .fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceAround
        ){
            Text(
                text = roomName,
                // modifier = Modifier.align(Alignment.Center),
                fontSize = 30.sp,
                fontWeight = FontWeight.Bold
            )
        }

        //Room
        Column (modifier = Modifier.verticalScroll(rememberScrollState())){
            Spacer(modifier = Modifier.padding(15.dp))

//            Fan("Quạt 1", 1)
//            Light("Đèn 1", 2)
//            AirConditioner("ĐH 1",3, 25)
            for(room in roomList){
                if(room.id==roomId){
                    for(device in room.devices){
                        when(device.type){
                            "fan" -> Fan(device.name, device.id)
                            "light" -> Light(device.name, device.id)
                            "ac" -> AirConditioner(device.name, device.id)
                        }
                    }
                }
            }

            AddDeviceBox()
        }
    }
}

//điều hoà
@Composable
fun AirConditioner(name:String, id:Int){
    var isDelete = remember { mutableStateOf(false) }
    val device = getDevice(id)
    var isOn = remember { mutableStateOf(device?.state ?: false) }
    var temperature= remember { mutableStateOf("20") }
    // Lắng nghe Firebase và cập nhật state của device
    LaunchedEffect(id) {
        combine(
            listenToFirebase(id),               // Lắng nghe trạng thái "status"
            listenToFirebase(id, "temperature") // Lắng nghe trạng thái "temperature"
        ) { status, temp ->
            Pair(status, temp)
        }.collect { (status, temp) ->
            device?.state = (status == "on")
            isOn.value = device?.state ?: false
            device?.otherState = temp
            temperature.value = temp
        }
    }

    Box(contentAlignment = Alignment.Center,
        modifier = Modifier.pointerInput(Unit){
            detectTapGestures (
                onLongPress = {
                    Log.d("long click vào phòng", "long click vào phòng")
                    //táp dài
                    isDelete.value=!isDelete.value
                },
                onTap = {
                    Log.d("click vào thiết bị điều hoà", "click vào thiết bị điều hoà")
                    // táp ngắn ->  truy cập menu điều hoà
                    GoToAirConditionerControlScreen(id)
                }
            )
        }
    ){
        if(isDelete.value){
            Row(verticalAlignment = Alignment.CenterVertically){
                // nút xoá
                Image(
                    painter = painterResource(id = R.drawable.editgreen),
                    contentDescription = "Icon",
                    modifier = Modifier
                        .size(50.dp)
                        .pointerInput(Unit) {
                            detectTapGestures(
                                onTap = {
                                    Log.d("ấn nút chỉnh sửa thiết bị", "ấn nút chỉnh sửa thiết bị")
                                    // sửa thiết bị ở dưới đây
                                    Edit(id)
                                },

                                )
                        }

                )

                //box
                Box(contentAlignment = Alignment.Center){
                    Image(painter = ColorPainter(Color(0xffd9d9d9)),
                        contentDescription = "",
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(top = 15.dp, bottom = 15.dp, start = 10.dp)
                            .size(100.dp)
                            .clip(shape = RoundedCornerShape(30))
                    )

                    Row(horizontalArrangement = Arrangement.SpaceAround,
                        modifier = Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically){
                        //
                        Text(
                            text = name,
                            fontSize = 26.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color.White
                        )

                    }
                }
            }
        }else{
            Image(painter = ColorPainter(Color(0xffaff4c6)),
                contentDescription = "",
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 15.dp, bottom = 15.dp)
                    .size(100.dp)
                    .clip(shape = RoundedCornerShape(30))
            )

            Row(horizontalArrangement = Arrangement.SpaceAround,
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically){
                //
                Text(
                    text = name,
                    fontSize = 32.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.White,
                    modifier = Modifier.weight(5f).padding(start = 55.dp).fillMaxWidth(),
                )
                Row(horizontalArrangement = Arrangement.SpaceAround,
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.weight(4f).padding(end=5.dp)

                ){
                    Box(modifier = Modifier
                        .size(36.dp)
                        .align(Alignment.CenterVertically)
                        .border(3.dp, Color.White, RoundedCornerShape(10.dp)),
                        contentAlignment = Alignment.Center){
                        Text(text = temperature.value,
                            color = Color.White,
                            fontSize = 16.sp,
                            fontWeight = FontWeight.Medium
                        )
                    }

                    if(isOn.value){
                        Image(
                            painter = painterResource(id = R.drawable.poweron),
                            contentDescription = "Icon",
                            modifier = Modifier.size(40.dp).pointerInput(Unit){
                                detectTapGestures (
                                    onTap = {
                                        Log.d("tắt điều hoà", "tắt điều hoà")
                                        TurnOff(id)
                                        isOn.value = !isOn.value
                                    },
                                )
                            }// Kích thước của icon
                        )
                    }else{
                        Image(
                            painter = painterResource(id = R.drawable.poweroff),
                            contentDescription = "Icon",
                            modifier = Modifier.size(40.dp).pointerInput(Unit){
                                detectTapGestures (
                                    onTap = {
                                        Log.d("bật điều hoà", "bật điều hoà")
                                        TurnOn(id)
                                        isOn.value = !isOn.value

                                    },
                                )
                            }// Kích thước của icon
                        )
                    }

                }

            }
        }
    }
}

//quạt
@Composable
fun Fan(name: String, id: Int) {
    var isDelete = remember { mutableStateOf(false) }
    val device = getDevice(id)
    var isOn = remember { mutableStateOf(device?.state ?: false) }

    // Lắng nghe Firebase và cập nhật state của device
    LaunchedEffect(id) {
        listenToFirebase(id).collect { status ->
            device?.state = (status == "on")  // Cập nhật vào device.state trước
            isOn.value = device?.state ?: false  // Cập nhật vào isOn
        }
    }




    Box(
        contentAlignment = Alignment.Center,
        modifier = Modifier.pointerInput(Unit) {
            detectTapGestures(
                onLongPress = {
                    Log.d("long click vào thiết bị", "long click vào thiết bị")
                    isDelete.value = !isDelete.value
                }
            )
        }
    ) {
        if (isDelete.value) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                // Nút sửa
                Image(
                    painter = painterResource(id = R.drawable.editgreen),
                    contentDescription = "Icon",
                    modifier = Modifier
                        .size(50.dp)
                        .pointerInput(Unit) {
                            detectTapGestures(
                                onTap = {
                                    Log.d("ấn nút sửa quạt", "ấn nút sửa quạt")
                                    Edit(id)
                                }
                            )
                        }
                )

                // Box hiển thị tên thiết bị
                Box(contentAlignment = Alignment.Center) {
                    Image(
                        painter = ColorPainter(Color(0xffd9d9d9)),
                        contentDescription = "",
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(top = 15.dp, bottom = 15.dp, start = 10.dp)
                            .size(100.dp)
                            .clip(shape = RoundedCornerShape(30))
                    )
                    Text(
                        text = name,
                        fontSize = 26.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
        } else {
            // Hiển thị quạt
            Image(
                painter = ColorPainter(Color(0xffaff4c6)),
                contentDescription = "",
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 15.dp, bottom = 15.dp)
                    .size(100.dp)
                    .clip(shape = RoundedCornerShape(30))
            )

            Row(
                horizontalArrangement = Arrangement.SpaceAround,
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = name,
                    fontSize = 32.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.White,
                    modifier = Modifier
                        .weight(2f)
                        .padding(start = 55.dp)
                        .fillMaxWidth()
                )

                // Hiển thị icon bật/tắt quạt
                Image(
                    painter = painterResource(id = if (isOn.value) R.drawable.son else R.drawable.soff),
                    contentDescription = "Icon",
                    modifier = Modifier
                        .size(40.dp)
                        .weight(1f)
                        .pointerInput(Unit) {
                            detectTapGestures(
                                onTap = {
                                    if (isOn.value) {
                                        Log.d("tắt quạt", "tắt quạt")
                                        TurnOff(id)
                                    } else {
                                        Log.d("bật quạt", "bật quạt")
                                        TurnOn(id)
                                    }
                                    isOn.value = !isOn.value
                                }
                            )
                        }
                )
            }
        }
    }
}


@Composable
fun Light(name: String, id: Int) {
    var isDelete = remember { mutableStateOf(false) }
    val device = getDevice(id)
    var isOn = remember { mutableStateOf(device?.state ?: false) }

    // Lắng nghe Firebase và cập nhật state của device
    LaunchedEffect(id) {
        listenToFirebase(id).collect { status ->
            device?.state = (status == "on")  // Cập nhật vào device.state trước
            isOn.value = device?.state ?: false  // Cập nhật vào isOn
        }
    }



    Box(
        contentAlignment = Alignment.Center,
        modifier = Modifier.pointerInput(Unit) {
            detectTapGestures(
                onLongPress = {
                    Log.d("long click vào thiết bị", "long click vào thiết bị")
                    isDelete.value = !isDelete.value
                }
            )
        }
    ) {
        if (isDelete.value) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                // Nút sửa
                Image(
                    painter = painterResource(id = R.drawable.editgreen),
                    contentDescription = "Icon",
                    modifier = Modifier
                        .size(50.dp)
                        .pointerInput(Unit) {
                            detectTapGestures(
                                onTap = {
                                    Log.d("ấn nút sửa đèn", "ấn nút sửa đèn")
                                    Edit(id)
                                }
                            )
                        }
                )

                // Box hiển thị tên thiết bị
                Box(contentAlignment = Alignment.Center) {
                    Image(
                        painter = ColorPainter(Color(0xffd9d9d9)),
                        contentDescription = "",
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(top = 15.dp, bottom = 15.dp, start = 10.dp)
                            .size(100.dp)
                            .clip(shape = RoundedCornerShape(30))
                    )
                    Text(
                        text = name,
                        fontSize = 26.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
        } else {
            // Hiển thị quạt
            Image(
                painter = ColorPainter(Color(0xffaff4c6)),
                contentDescription = "",
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 15.dp, bottom = 15.dp)
                    .size(100.dp)
                    .clip(shape = RoundedCornerShape(30))
            )

            Row(
                horizontalArrangement = Arrangement.SpaceAround,
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = name,
                    fontSize = 32.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.White,
                    modifier = Modifier
                        .weight(2f)
                        .padding(start = 55.dp)
                        .fillMaxWidth()
                )

                // Hiển thị icon bật/tắt quạt
                Image(
                    painter = painterResource(id = if (isOn.value) R.drawable.son else R.drawable.soff),
                    contentDescription = "Icon",
                    modifier = Modifier
                        .size(40.dp)
                        .weight(1f)
                        .pointerInput(Unit) {
                            detectTapGestures(
                                onTap = {
                                    if (isOn.value) {
                                        Log.d("tắt quạt", "tắt quạt")
                                        TurnOff(id)
                                    } else {
                                        Log.d("bật quạt", "bật quạt")
                                        TurnOn(id)
                                    }
                                    isOn.value = !isOn.value
                                }
                            )
                        }
                )
            }
        }
    }
}







@Composable
//cửa
fun Door(name:String){

}

@Composable
fun AddDeviceBox(){
    Box(contentAlignment = Alignment.Center){
        Image(painter = ColorPainter(Color(0xffaff4c6)),
            contentDescription = "",
            modifier = Modifier
                .fillMaxWidth()
                .padding(top = 15.dp, bottom = 15.dp,)
                .size(100.dp)
                .clip(shape = RoundedCornerShape(30)),
        )

        Row(horizontalArrangement = Arrangement.SpaceAround,
            modifier = Modifier
                .fillMaxWidth()
                .pointerInput(Unit) {
                    detectTapGestures(
                        onTap = {
                            Log.d("ấn nút add device", "ấn nút add device")
                            // xử lý thêm phòng ở đây
                            AddDevice()
                        },
                    )
                },
            verticalAlignment = Alignment.CenterVertically){
            //
            Image(
                painter = painterResource(id = R.drawable.plus),
                contentDescription = "Icon",
                modifier = Modifier.size(40.dp)// Kích thước của icon
            )

        }
    }
}



////////////////////// Menu điều hoà

@Composable
fun AirConditionerControl(id: Int, name: String) {
    val device = getDevice(id)
    var isOn = remember { mutableStateOf(device?.state ?: false) }

    // Lắng nghe Firebase và cập nhật state của device
    LaunchedEffect(id) {
        listenToFirebase(id).collect { status ->
            device?.state = (status == "on")  // Cập nhật vào device.state trước
            isOn.value = device?.state ?: false  // Cập nhật vào isOn
        }
    }

    //////////////////////
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(15.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // **Tiêu đề Remote**
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(10.dp),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = "Remote $name",
                fontSize = 30.sp,
                fontWeight = FontWeight.Bold,
                modifier = Modifier.align(Alignment.Center)
            )

            // **Nút Close ở góc dưới cùng**
            Image(
                painter = painterResource(id = R.drawable.close),
                contentDescription = "Close",
                modifier = Modifier
                    .size(36.dp)
                    .align(Alignment.TopEnd)
                    .clickable {
                        Log.d("ấn nút Lưu thông tin", "ấn nút lưu thông tin")
                        GoToDeviceList(activeRoom.value)
                    }
            )
        }

        // **Điều khiển Remote**
        Column(
            modifier = Modifier
                .verticalScroll(rememberScrollState())
                .padding(top = 20.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            // **Nút UP**
            ScalableImage(R.drawable.up) {
                Log.d("bấm lên", "bấm lên")
                ChangeTemperature(id, "up")
            }

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(100.dp),
                horizontalArrangement = Arrangement.Center
            ) {
                // **Nút LEFT**
                ScalableImage(R.drawable.leftblack) {
                    Log.d("bấm trái", "bấm trái")
                    ChangeTemperature(id, "left")
                }

                // **Nút POWER**
                ScalableImage(if (isOn.value) R.drawable.poweron else R.drawable.powerblack) {
                    Log.d("bấm nguồn", "bấm nguồn")
                    isOn.value = !isOn.value
                    ChangeTemperature(id, if (isOn.value) "on" else "off")
                }

                // **Nút RIGHT**
                ScalableImage(R.drawable.rightblack) {
                    Log.d("bấm phải", "bấm phải")
                    ChangeTemperature(id, "right")
                }
            }

            // **Nút DOWN**
            ScalableImage(R.drawable.down) {
                Log.d("bấm xuống", "bấm xuống")
                ChangeTemperature(id, "down")
            }
        }
    }
}




@Composable
fun ScalableImage(imageRes: Int, onClick: () -> Unit) {
    var isPressed = remember { mutableStateOf(false) }

    val scale = animateFloatAsState(
        targetValue = if (isPressed.value) 1.2f else 1f,
        animationSpec = tween(100), label = "scaleAnimation"
    )

    Image(
        painter = painterResource(id = imageRes),
        contentDescription = "Remote Button",
        modifier = Modifier
            .size(100.dp)
            .scale(scale.value)
            .pointerInput(Unit) {
                detectTapGestures(
                    onPress = {
                        isPressed.value = true
                        tryAwaitRelease() // Đợi thả ra
                        isPressed.value = false
                        onClick()
                    }
                )
            }
    )
}
