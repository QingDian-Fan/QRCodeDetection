package com.code.detection

import android.graphics.Bitmap

object QRCodeUtils {

    init {
        System.loadLibrary("native-lib")
    }

    external fun isContainQRCode(bitmap: Bitmap): Boolean

    external fun detectionQRCode(bitmap: Bitmap):Bitmap


}