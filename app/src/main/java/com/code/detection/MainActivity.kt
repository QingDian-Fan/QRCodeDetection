package com.code.detection

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
       val qr_bitmap:Bitmap= BitmapFactory.decodeResource(resources,R.mipmap.icon_code_1)
        val containQRCode = QRCodeUtils.isContainQRCode(qr_bitmap)
        if (containQRCode) Toast.makeText(this,"包含",Toast.LENGTH_LONG).show()
        else Toast.makeText(this,"不包含",Toast.LENGTH_LONG).show()
    }
}
