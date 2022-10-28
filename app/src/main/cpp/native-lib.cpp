#include <jni.h>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include "BitmapMatUtils.h"


#define TAG "TAG-OPENCV"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

using namespace std;
using namespace cv;


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_code_detection_QRCodeUtils_isContainQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat src;
    BitmapMatUtils::bitmap2mat(env, bitmap, src);
    if (src.empty()) {
        LOGE("src imread error");
        return -1;
    }
    //对图像进行灰度处理
    Mat gray;
    cvtColor(src, gray, COLOR_BGRA2GRAY);
    //二值化
    threshold(gray, gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //寻找轮廓
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    //循环所有轮廓  进行筛选
    for (int i = 0; i < contours.size(); ++i) {
        double area = contourArea(contours[i]);
        if (area < 49) {//区域面积小于49的排除
            continue;
        }
        RotatedRect mRect = minAreaRect(contours[i]);
        float width = mRect.size.width;
        float height = mRect.size.height;
        //求出宽高比  进一步过滤
        float ratio = min(width, height) / max(width, height);
        if (ratio > 0.9 && width < gray.cols / 2 && height < gray.rows / 2) {

        }
    }

    return 0;

}




/*extern "C"
JNIEXPORT jboolean JNICALL
Java_com_code_detection_MainActivity_isContainQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {

}*/