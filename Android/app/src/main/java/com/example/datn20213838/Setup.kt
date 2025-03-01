package com.example.datn

import android.util.Log
import androidx.compose.foundation.Image
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.pointer.PointerIcon.Companion.Text
import androidx.compose.ui.input.pointer.motionEventSpy
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.TextUnit
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.datn20213838.R
import com.example.datn20213838.SaveRoom
import com.example.datn20213838.Test

@Composable
fun SetupUI(){
    Column (){
       Box(
           modifier = Modifier.padding(top = 40.dp).fillMaxWidth(),
           contentAlignment = Alignment.Center
       ){
           Text(
               text="ĐỒ ÁN TỐT NGHIỆP\n\nNguyễn Mạnh Dũng-20213838\n\n",
               fontSize = 26.sp,
              // fontStyle = FontStyle.Italic,
               //fontFamily = FontFamily.Cursive,
               fontWeight = FontWeight.Bold,
               textAlign = TextAlign.Center
           )
       }
        Image(
            painter = painterResource(id= R.drawable.hust),
            contentDescription = "HUST",
            //modifier = Modifier.size(0.dp)
            modifier = Modifier.pointerInput(Unit){
                detectTapGestures (
                    onTap = {
                        Test()
                            },
                    )
            }
        )



    }
}