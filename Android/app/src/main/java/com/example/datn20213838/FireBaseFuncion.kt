package com.example.datn20213838
import com.example.datn20213838.GlobalData.URLFB
import com.google.firebase.database.*
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

fun sendDataToFirebase(status: String, deviceId:Int) {
    var  r="room"
    var  d="device"
    for (room in roomList){
        for (device in room.devices) {
            if(device.id==deviceId){
                r=r+room.id.toString()
                d=d+device.id.toString()
            }
        }
    }
    val database = FirebaseDatabase.getInstance(URLFB.value)
        .reference.child(r).child(d).child("status")

    database.setValue(status).addOnSuccessListener {
        println("Gửi dữ liệu thành công!")
    }.addOnFailureListener {
        println("Lỗi khi gửi dữ liệu: ${it.message}")
    }
}



fun listenToFirebase(deviceId: Int, str:String = "status"): Flow<String> = callbackFlow {//Flow + callbackFlow
    var roomPath: String? = null
    var devicePath: String? = null

    // Tìm đường dẫn thiết bị
    for (room in roomList) {
        for (device in room.devices) {
            if (device.id == deviceId) {
                roomPath = "room${room.id}"
                devicePath = "device${device.id}"
                break
            }
        }
    }

    // Nếu không tìm thấy thiết bị
    if (roomPath == null || devicePath == null) {
        trySend("Không tìm thấy thiết bị!").isSuccess
        close()
        return@callbackFlow
    }

    // Kết nối Firebase
    val database = FirebaseDatabase.getInstance()
        .reference.child(roomPath).child(devicePath).child(str)

    val listener = object : ValueEventListener {
        override fun onDataChange(snapshot: DataSnapshot) {
            val status = snapshot.getValue(String::class.java) ?: "Lỗi dữ liệu!"
            trySend(status).isSuccess
        }

        override fun onCancelled(error: DatabaseError) {
            close(error.toException())
        }
    }

    database.addValueEventListener(listener)

    // Hủy listener khi không còn cần thiết
    awaitClose { database.removeEventListener(listener) }
}

/*
class DeviceViewModel(deviceId: Int) : ViewModel() {
    val status: StateFlow<String> = listenToFirebase(deviceId)
        .stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), "Đang tải...")
}

*/


/*//hiển thị
@Composable
fun DeviceStatusScreen(deviceId: Int, viewModel: DeviceViewModel = viewModel()) {
    val status by viewModel.status.collectAsState()

    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Text(text = "Trạng thái thiết bị:", fontSize = 20.sp)
        Text(text = status, fontSize = 18.sp, color = Color.Blue)
    }
}

 */


fun sendDataToFirebase2(path: String, message: String) {
    val database = FirebaseDatabase.getInstance(URLFB.value).reference.child(path)

    database.setValue(message).addOnSuccessListener {
        println("Gửi dữ liệu thành công đến $path!")
    }.addOnFailureListener {
        println("Lỗi khi gửi dữ liệu đến $path: ${it.message}")
    }
}





fun listenToFirebase2(path: String): Flow<String> = callbackFlow {
    val database = FirebaseDatabase.getInstance().reference.child(path)

    val listener = object : ValueEventListener {
        override fun onDataChange(snapshot: DataSnapshot) {
            val data = snapshot.getValue(String::class.java) ?: "Lỗi dữ liệu!"
            trySend(data).isSuccess
        }

        override fun onCancelled(error: DatabaseError) {
            close(error.toException())
        }
    }

    database.addValueEventListener(listener)

    awaitClose { database.removeEventListener(listener) }

    /* cách gọi
    val statusFlow = listenToFirebase("room1/device2/status")
statusFlow.collect { status ->
    println("Trạng thái thiết bị: $status")
}

     */
}

















































/*
fun listenToFirebase(onDataChange: (String) -> Unit, deviceId:Int) {
    var  r="room"
    var  d="device"
    for (room in roomList){
        for (device in room.devices) {
            if(device.id==deviceId){
                r=r+room.id.toString()
                d=d+device.id.toString()
            }
        }
    }

    val database = FirebaseDatabase.getInstance("https://datn20213838-default-rtdb.asia-southeast1.firebasedatabase.app/")
        .reference.child(r).child(d).child("status")

    database.addValueEventListener(object : ValueEventListener {
        override fun onDataChange(snapshot: DataSnapshot) {
            val status = snapshot.getValue(String::class.java) ?: "Lỗi dữ liệu!"
            onDataChange(status)  // Gửi dữ liệu về UI
        }

        override fun onCancelled(error: DatabaseError) {
            println("Lỗi khi đọc dữ liệu: ${error.message}")
        }
    })
}
private var firebaseListener: ValueEventListener? = null

fun stopListeningToFirebase() {
    val database = FirebaseDatabase.getInstance()
        .reference.child("devices").child("device1").child("status")

    firebaseListener?.let {
        database.removeEventListener(it)
        firebaseListener = null  // Xóa listener sau khi hủy
    }
}
*/

//sendDataToFirebase("on") // Gửi trạng thái bật
//sendDataToFirebase("off") // Gửi trạng thái tắt

//listenToFirebase { newStatus ->
//    println("Trạng thái mới: $newStatus") // Cập nhật UI dựa trên dữ liệu Firebase
//}

//stopListeningToFirebase()