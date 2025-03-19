package com.example.datn20213838

import android.content.Context
import android.util.Log
import android.widget.Toast
import com.example.datn20213838.GlobalData.AuthMsg
import com.example.datn20213838.GlobalData.AuthState
import com.example.datn20213838.GlobalData.activeTaskbar
import com.example.datn20213838.GlobalData.userId
import com.google.android.gms.tasks.Task
import com.google.firebase.auth.AuthResult
import com.google.firebase.auth.FirebaseAuth
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch


var auth = FirebaseAuth.getInstance()

fun registerUser(context: Context, email: String?, password: String?) {
    email?.let { mail ->
        password?.let { pass ->
            auth.createUserWithEmailAndPassword(mail, pass)
                .addOnCompleteListener { task ->
                    if (task.isSuccessful) {
                        val user = auth.currentUser
                        AuthMsg.value = "Đăng ký thành công"
                        showToast(context, "Đăng ký thành công: ${user?.email}")

                        // Cập nhật userId sau khi đăng ký thành công
                        userId.value = getCurrentUserId() ?: "default_user"

                        // Chạy đăng nhập & tạo topic trong Coroutine để tránh block UI
                        CoroutineScope(Dispatchers.Main).launch {
                            delay(1000) // Đợi một chút để đảm bảo Firebase cập nhật
                            loginUser(mail, pass)
                            AuthState.value = isUserLoggedIn()

                            delay(1000) // Đợi cập nhật trạng thái
                            CreateUserTopic()
                            showToast(context, "Create Topic Success")
                            fetchRoom()
                            activeTaskbar.value="home"

                        }
                    } else {
                        AuthMsg.value = "Lỗi khi đăng ký"
                        showToast(context, "Lỗi: ${task.exception?.message}")
                    }
                }
        } ?: showToast(context, "Password không hợp lệ")
    } ?: showToast(context, "Email không hợp lệ")
}


fun loginUser(email: String?, password: String?) {
    auth.signInWithEmailAndPassword(email!!, password!!)
        .addOnCompleteListener { task: Task<AuthResult?> ->
            if (task.isSuccessful) {
                val user = auth.currentUser
                AuthMsg.value = "Đăng nhập thành công"
                Log.d("Login", "Đăng nhập thành công: " + user!!.email)

                // Chạy coroutine để cập nhật userId liên tục cho đến khi khác "default_user"
                CoroutineScope(Dispatchers.IO).launch {
                    while (userId.value == "default_user") {
                        userId.value = getCurrentUserId() ?: "default_user"
                        delay(500) // Chờ 500ms trước khi kiểm tra lại
                    }
                    fetchRoom() // Gọi hàm sau khi userId đã cập nhật
                }
            } else {
                AuthMsg.value = "Đăng nhập thất bại, xem lại email/mật khẩu"
                Log.e("Login", "Lỗi: " + task.exception!!.message)
            }
        }
}


fun logoutUser() {
    auth.signOut()
    Log.d("Logout", "Người dùng đã đăng xuất")
    userId.value = "default_user"
    roomList.clear()
}

fun isUserLoggedIn(): Boolean {
    return auth.currentUser != null
}

// lấy id người dùng hiện tại
fun getCurrentUserId(): String? {
    val auth = FirebaseAuth.getInstance()
    return auth.currentUser?.uid
}
// lấy email người dùng hiện tại
fun getCurrentUserEmail(): String? {
    val auth = FirebaseAuth.getInstance()
    return auth.currentUser?.email
}


fun sendPasswordResetEmail(email: String, context: Context) {
    FirebaseAuth.getInstance().sendPasswordResetEmail(email)
        .addOnCompleteListener { task ->
            if (task.isSuccessful) {
                Toast.makeText(context, "Kiểm tra email để đặt lại mật khẩu!", Toast.LENGTH_LONG).show()
            } else {
                Toast.makeText(context, "Lỗi: ${task.exception?.message}", Toast.LENGTH_LONG).show()
            }
        }
}
