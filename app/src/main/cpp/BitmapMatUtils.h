//
// Created by Nathaniel on 2022/10/27.
//

#ifndef QR_CODE_DETECTION_BITMAPMATUTILS_H
#define QR_CODE_DETECTION_BITMAPMATUTILS_H

#include <jni.h>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>

using namespace cv;

class BitmapMatUtils {
public:
    static int bitmap2mat(JNIEnv *env,jobject &bitmap,Mat &map);

    static int mat2bitmap(JNIEnv *env,Mat &map,jobject &bitmap);

    static jobject createBitmap(JNIEnv *env,jint width,jint height,int type);
};


#endif //QR_CODE_DETECTION_BITMAPMATUTILS_H