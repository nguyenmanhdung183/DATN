package com.example.datn20213838

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.datn.Main
import com.example.datn.geUpdateNoti
import com.example.datn20213838.ui.theme.DATN20213838Theme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            DATN20213838Theme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        name = "Android",
                        modifier = Modifier.padding(innerPadding)
                    )
                    geUpdateNoti()
                    Main()
                }
            }
        }
    }
}

object GlobalData {
    var activeTaskbar = mutableStateOf("home")// xem thanh taskbar đang ở phần nào
    var activeRoom= mutableStateOf(-1)// mỗi phòng có 1 id
    var haveNotis = mutableStateOf(true)// có thông báo thì hiện chấm đỏ
    var controlAriconditioner = mutableStateOf( -1)// vào menu điều hiển điều hoà
    var edittingRoom = mutableStateOf(-1)// chỉnh sửa phòng
    var edittingDevice = mutableStateOf(-1)// chỉnh sửa thiết bị
    //var editDevice = mutableStateOf(false)// chỉnh sửa thiết bị
    var getUpdateDelay = mutableStateOf(1000)// 1000ms
    var deviceStateChange = mutableStateOf(false)
    var URLFB = mutableStateOf("https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/")
    var newestNoti= mutableStateOf("null")

}



@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {

}



@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    DATN20213838Theme {
        Greeting("Android")
    }
}