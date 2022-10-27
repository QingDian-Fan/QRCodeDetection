#include <jni.h>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_code_detection_QRCodeUtils_isContainQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {

}




/*extern "C"
JNIEXPORT jboolean JNICALL
Java_com_code_detection_MainActivity_isContainQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {

}*/