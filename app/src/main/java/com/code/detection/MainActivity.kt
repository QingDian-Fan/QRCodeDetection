package com.code.detection

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.ImageView
import android.widget.Toast


class MainActivity : AppCompatActivity() {

    private val mIvBitmap: ImageView by lazy { findViewById<ImageView>(R.id.iv_bitmap) }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val qr_bitmap:Bitmap= BitmapFactory.decodeResource(resources,R.mipmap.icon_code_3)
        val containQRCode = QRCodeUtils.detectionQRCode(qr_bitmap)
        mIvBitmap.setImageBitmap(containQRCode)
        Bitmap.createBitmap(10,10,Bitmap.Config.ARGB_8888);
       // Bitmap.createBitmap()
    }
}
