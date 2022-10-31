#include <jni.h>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include "BitmapMatUtils.h"


#define TAG "TAG-OPENCV"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

using namespace std;
using namespace cv;

Mat wrapTransform(const Mat &gray, const RotatedRect &mRect) {
    int width = mRect.size.width;
    int height = mRect.size.height;
    Mat result(height, width, gray.type());
    Point2f pts[4];
    mRect.points(pts);
    vector<Point> srcPoint;
    for (int i = 0; i < 4; ++i) {
        srcPoint.push_back(pts[i]);
    }
    vector<Point> dstPoint;
    dstPoint.push_back(Point(0, 0));
    dstPoint.push_back(Point(width, 0));
    dstPoint.push_back(Point(width, height));
    dstPoint.push_back(Point(0, height));
    Mat M = findHomography(srcPoint, dstPoint);
    warpPerspective(gray, result, M, result.size());
    return result;

}


bool verifyXDirection(const Mat &mat) {
    int centerBlack = 0, leftWhite = 0, rightWhite = 0, leftBlack = 0, rightBlack = 0;
    int width = mat.cols;
    int height = mat.rows;
    int centerX = width / 2;
    int centerY = height / 2;
    uchar pixels = mat.at<uchar>(centerY, centerX);
    if (pixels == 255) return false;

    int start = 0, end = 0, offset = 0;
    bool findLeft = false, findRight = false;
    while (true) {
        offset++;
        if ((centerX - offset) <= 0 || (centerX + offset) >= (width - 1)) {
            break;
        }
        //左边
        pixels = mat.at<uchar>(centerY, centerX - offset);
        if (!findLeft && pixels == 255) {
            start = centerX - offset;
            findLeft = true;
        }
        //右边
        pixels = mat.at<uchar>(centerY, centerX + offset);
        if (!findRight && pixels == 255) {
            end = centerX + offset;
            findRight = true;
        }
        //跳出循环
        if (findLeft && findRight) {
            break;
        }
    }
    if (start == 0 || end == 0) {
        return false;
    }
    centerBlack = end - start;
    //左右中间的白色
    //右边
    for (int col = end; col < width - 1; col++) {
        pixels = mat.at<uchar>(centerY, col);
        if (pixels == 0) {
            break;
        }
        rightWhite++;
    }
    //左边
    for (int col = start; col > 0; col--) {
        pixels = mat.at<uchar>(centerY, col);
        if (pixels == 0) {
            break;
        }
        leftWhite++;
    }

    if (leftWhite == 0 || rightWhite == 0) {
        return false;
    }

    //两边黑色
    for (int col = end + rightWhite; col < width - 1; col++) {
        pixels = mat.at<uchar>(centerY, col);
        if (pixels == 255) {
            break;
        }
        rightBlack++;
    }

    for (int col = start - leftWhite; col > 0; col--) {
        pixels = mat.at<uchar>(centerY, col);
        if (pixels == 255) {
            break;
        }
        leftBlack++;
    }


    if (leftBlack == 0 || rightBlack == 0) {
        return false;
    }

    float sum = centerBlack + leftWhite + rightWhite + leftBlack + rightBlack;
    centerBlack = (centerBlack / sum) * 7 + 0.5;
    leftWhite = (leftWhite / sum) * 7 + 0.5;
    leftBlack = (leftBlack / sum) * 7 + 0.5;
    rightWhite = (rightWhite / sum) * 7 + 0.5;
    rightBlack = (rightBlack / sum) * 7 + 0.5;
    return (centerBlack == 3 || centerBlack == 4) && (leftBlack == rightBlack) &&
           (leftWhite == rightWhite) && (leftWhite == leftBlack) && (leftWhite == 1);

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_code_detection_QRCodeUtils_isContainQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {
    int itemCounts = 0;
    Mat src;
    BitmapMatUtils::bitmap2mat(env, bitmap, src);
    if (src.empty()) {
        LOGE("src imread error");
        return itemCounts >= 3;
    }
    //对图像进行灰度处理
    Mat gray;
    cvtColor(src, gray, COLOR_BGRA2GRAY);
    //二值化
    threshold(gray, gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    //寻找轮廓
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    Scalar color = Scalar(0, 0, 255);
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
            Mat qrROI = wrapTransform(gray, mRect);
            if (verifyXDirection(qrROI)) {
                itemCounts++;
                //drawContours(src, contours, i, Scalar(0, 255, 0), 4,LINE_4);

            }
        }
    }


    /* bool isWrite = imwrite("/sdcard/opencv/code_1.png", src);
     if (isWrite) {
         LOGE("写入成功");
     } else {
         LOGE("写入失败");
     }
 */
    return itemCounts >= 3;
}
