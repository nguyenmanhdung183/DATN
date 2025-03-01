package com.example.datn

import android.util.Log
import androidx.compose.foundation.Image
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
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
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.ColorPainter
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.datn20213838.DeleteNoti
import com.example.datn20213838.GlobalData.newestNoti
import com.example.datn20213838.NotiData
import com.example.datn20213838.R
import com.example.datn20213838.listenToFirebase2
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch


@Composable
fun NotiMain() {
    val notiListState = remember { mutableStateOf(notiList.toList()) } // State lưu danh sách thông báo
    val currentNoti2 = remember { mutableStateOf("null") } // State lưu thông báo hiện tại
    val currentNoti1 = remember { mutableStateOf("null") } // State lưu thông báo hiện tại

    LaunchedEffect(newestNoti.value) {
        currentNoti1.value = newestNoti.value

        if (currentNoti1.value != currentNoti2.value) {
            Log.d("currentNoti1", currentNoti1.value)
            Log.d("currentNoti2", currentNoti2.value)

            currentNoti2.value = currentNoti1.value

            // Cập nhật danh sách thông báo khi có thông báo mới
            notiListState.value = notiList.toList()
        }
    }

    Spacer(modifier = Modifier.padding(15.dp))
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .padding(start = 20.dp, end = 20.dp, top = 10.dp, bottom = 10.dp),
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = "Thông Báo",
            fontSize = 30.sp,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.matchParentSize()
        )

        Image(
            painter = painterResource(id = R.drawable.trashblack),
            contentDescription = "Icon",
            modifier = Modifier
                .size(36.dp)
                .align(Alignment.BottomEnd)
                .pointerInput(Unit) {
                    detectTapGestures(
                        onTap = {
                            Log.d("ấn nút xoá thông báo", "ấn nút xoá thông báo")

                            // Gọi hàm xoá
                            DeleteNoti()

                            // Cập nhật lại UI
                            notiList.clear()
                            notiListState.value = emptyList()
                        }
                    )
                }
        )
    }

    // Danh sách thông báo
    Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
        Spacer(modifier = Modifier.padding(15.dp))
        for (noti in notiListState.value.asReversed()) {
            NotiBox(noti.text, "${noti.hour}:${noti.minute}")
        }

    }
}

@Composable
fun NotiBox(text: String, time: String, isRead: Boolean=false){
    Box(contentAlignment = Alignment.Center,
        modifier = Modifier.height(90.dp)){
        Image(painter = ColorPainter(Color(0xeed9d9d9)),
            contentDescription = "",
            modifier = Modifier
                .fillMaxWidth()
                .padding(10.dp)
                .size(68.dp)
                .clip(shape = RoundedCornerShape(40))
        )

        Row(horizontalArrangement = Arrangement.SpaceAround,
            modifier = Modifier.fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically){
            Text(
                text=text,
                modifier = Modifier.padding(start = 40.dp).weight(1f),
                fontWeight = FontWeight.Medium,
                fontSize = 28.sp,
                color = Color.White
            )
            Text(
                text=time,
                modifier = Modifier.padding(start=30.dp, end = 30.dp),
                fontWeight = FontWeight.Medium,
                fontSize = 20.sp
            )
        }
    }
}


fun ClearNotiList(){
    notiList.clear()
}


fun deleteAllNotificationsFromDTB() {
    val database = FirebaseDatabase.getInstance().reference
    val notiRef = database.child("main").child("noti")

    notiRef.removeValue()
        .addOnSuccessListener {
            Log.d("Firebase", "✅ Đã xóa tất cả thông báo thành công!")
        }
        .addOnFailureListener { e ->
            Log.e("Firebase", "❌ Lỗi khi xóa tất cả thông báo: ${e.message}")
        }
}




var notiList = mutableListOf<NotiData>()

fun geUpdateNoti() {
    CoroutineScope(Dispatchers.IO).launch {
        listenToFirebase2("main/newestNoti").collect { status ->
            if (status != newestNoti.value) {
                newestNoti.value = status // Cập nhật giá trị mới nhất
                fetchNotificationsFromFirebase() // Fetch lại noti khi có cập nhật
            }
            Log.d("status", status)
        }
    }
}


fun fetchNotificationsFromFirebase() {
    val database = FirebaseDatabase.getInstance().reference
    val notiRef = database.child("main").child("noti")

    notiRef.addValueEventListener(object : ValueEventListener {
        override fun onDataChange(snapshot: DataSnapshot) {
            notiList.clear() // Clear old list before updating it with new data
            Log.d("clear", "Cleared noti list")

            for (notiSnapshot in snapshot.children) {
                val notiId = notiSnapshot.key ?: continue
                val text = notiSnapshot.child("text").getValue(String::class.java) ?: "null"
                val timeRaw = notiSnapshot.child("time").getValue(String::class.java) ?: "null"
                Log.d("Firebase", "🔔 Thông báo từ $timeRaw")
                // Split the timeRaw and parse the data
                val parsedData = parseTimeData(timeRaw)
                val deviceId = parsedData[0] as String
                val hour = parsedData[1] as String
                val minute = parsedData[2] as String
                val day = parsedData[3] as String
                val month = parsedData[4] as String
                val year = parsedData[5] as String

                // Create a NotiData object and add it to the list
                val noti = NotiData(
                    deviceId = deviceId,
                    notiId = notiId,
                    hour = hour,
                    minute = minute,
                    day = day,
                    month = month,
                    year = year,
                    text = text
                )
                notiList.add(noti)

                Log.d("Firebase", "🔔 Thông báo từ $deviceId lúc $hour:$minute ngày $day/$month/$year - $text")
            }

            // Log the updated list
            Log.d("Firebase", "🔔 Danh sách thông báo cập nhật: $notiList")
        }

        override fun onCancelled(error: DatabaseError) {
            Log.e("Firebase", "❌ Lỗi khi lấy dữ liệu", error.toException())
        }
    })
}

// Parsing the time data
fun parseTimeData(timeRaw: String): List<Any> {
    val parts = timeRaw.split(".")
    Log.d("parseTimeData", "Parts: $parts")

    return if (parts.size == 7) {
        try {
            // Parse all parts into their correct types
            val deviceId = parts[0]
            val hour = parts[1]
            val minute = parts[2]
            val second=parts[3]
            val day = parts[4]
            val month = parts[5]
            val year = parts[6]

            listOf(deviceId, hour, minute, day, month, year)
        } catch (e: Exception) {
            // In case parsing fails, provide default values
            Log.e("parseTimeData", "Error parsing time data: $timeRaw", e)
            listOf("Unknown", "00", "00", "01", "01", "2000")
        }
    } else {
        // If the timeRaw format doesn't have 6 parts, return default values
        Log.e("parseTimeData", "Invalid time format: $timeRaw")
        listOf("Unknown", "00", "00", "01", "01", "2000")
    }
}