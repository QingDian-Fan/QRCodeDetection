//
// Created by Nathaniel on 2022/10/27.
//

#include "BitmapMatUtils.h"

/**
 * Bitmap -> Mat
 * @param env
 * @param bitmap
 * @param map
 * @return
 */

int BitmapMatUtils::bitmap2mat(JNIEnv *env, jobject &bitmap, Mat &mat) {
    AndroidBitmapInfo bitmapInfo;
    int getInfo = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfo < 0) {
        return getInfo;
    }
    void *pixels;
    int lockPixels = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixels < 0) {
        return lockPixels;
    }
    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        mat.create(bitmapInfo.height, bitmapInfo.width, CV_8UC4);
        mat.data = reinterpret_cast<uchar *>(pixels);
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        mat.create(bitmapInfo.height, bitmapInfo.width, CV_8UC2);
        mat.data = reinterpret_cast<uchar *>(pixels);
    } else {
        return -1;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}


/**
 * Mat -> Bitmap
 * @param env
 * @param map
 * @param bitmap
 * @return
 */
int BitmapMatUtils::mat2bitmap(JNIEnv *env, Mat &mat, jobject &bitmap) {
    AndroidBitmapInfo bitmapInfo;
    int getInfo = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfo < 0) {
        return -1;
    }
    void *pixels;
    int lockPixels = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixels < 0) {
        return lockPixels;
    }
    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, pixels);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, COLOR_GRAY2RGBA);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, COLOR_RGB2BGRA);
        } else if (mat.type() == CV_8UC4) {
            mat.copyTo(temp);
        }
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, pixels);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, COLOR_GRAY2BGR565);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, COLOR_RGB2BGR565);
        } else if (mat.type() == CV_8UC4) {
            cvtColor(mat, temp, COLOR_RGBA2BGR565);
        }
    } else {
        return -1;
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}


jobject BitmapMatUtils::createBitmap(JNIEnv *env, jint width, jint height, char* type) {
    const char *bitmap_config_class_name = "android/graphics/Bitmap$Config";
    jclass bitmap_config_class = env->FindClass(bitmap_config_class_name);

    jmethodID bitmap_config_mid = env->GetStaticMethodID(bitmap_config_class, "valueOf",
                                                         "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;");
    jstring configName = env->NewStringUTF(type);
    jobject bitmap_config = env->CallStaticObjectMethod(bitmap_config_class, bitmap_config_mid,
                                                        bitmap_config_class, configName);

    const char *bitmap_class_name = "android/graphics/Bitmap";
    jclass bitmap_class = env->FindClass(bitmap_class_name);
    jmethodID create_bitmap_mid = env->GetStaticMethodID(bitmap_class, "createBitmap",
                                                         "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(bitmap_class, create_bitmap_mid, width, height,
                                                 bitmap_config);
    return bitmap;
}
