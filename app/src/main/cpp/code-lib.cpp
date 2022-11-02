#include <jni.h>
#include <string>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include "BitmapMatUtils.h"


#define TAG "TAG-OPENCV"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

using namespace std;
using namespace cv;


//找到二维码所在的矩形区域
void Find_QR_Rect(Mat &mat, vector<Mat> &ROI_Rect) {
    Mat gray;
    cvtColor(mat, gray, COLOR_BGR2GRAY);

    Mat blur;
    GaussianBlur(gray, blur, Size(3, 3), 0);

    Mat bin;
    threshold(blur, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

    //通过Size（5，1）开运算消除边缘毛刺
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 1));
    Mat open;
    morphologyEx(bin, open, MORPH_OPEN, kernel);
    //通过Size（21，1）闭运算能够有效地将矩形区域连接 便于提取矩形区域
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(21, 1));
    Mat close;
    morphologyEx(open, close, MORPH_CLOSE, kernel1);


    //使用RETR_EXTERNAL找到最外轮廓
    vector<vector<Point>> MaxContours;
    findContours(close, MaxContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < MaxContours.size(); i++) {
        Mat mask = Mat::zeros(mat.size(), mat.type());
        mask = Scalar::all(255);

        double area = contourArea(MaxContours[i]);

        //通过面积阈值找到二维码所在矩形区域
        if (area > 6000 && area < 100000) {
            //计算最小外接矩形
            RotatedRect MaxRect = minAreaRect(MaxContours[i]);
            //计算最小外接矩形宽高比
            double ratio = MaxRect.size.width / MaxRect.size.height;

            if (ratio > 0.8 && ratio < 1.2) {
                Rect MaxBox = MaxRect.boundingRect();
                //将矩形区域从原图抠出来
                Mat ROI = mat(Rect(MaxBox.tl(), MaxBox.br()));

                ROI.copyTo(mask(MaxBox));

                ROI_Rect.push_back(mask);
            }

        }
    }
}


//对找到的矩形区域进行识别是否为二维码
int Dectect_QR_Rect(Mat &mat, Mat &canvas, vector<Mat> &ROI_Rect) {
    //用于存储检测到的二维码
    vector<vector<Point>> QR_Rect;

    //遍历所有找到的矩形区域
    for (int i = 0; i < ROI_Rect.size(); i++) {
        Mat gray;
        cvtColor(ROI_Rect[i], gray, COLOR_BGR2GRAY);

        Mat bin;
        threshold(gray, bin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

        //通过hierarchy、RETR_TREE找到轮廓之间的层级关系
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);

        //父轮廓索引
        int ParentIndex = -1;
        int cn = 0;

        //用于存储二维码矩形的三个“回”
        vector<Point> rect_points;
        for (int i = 0; i < contours.size(); i++) {
            //hierarchy[i][2] != -1 表示该轮廓有子轮廓  cn用于计数“回”中第几个轮廓
            if (hierarchy[i][2] != -1 && cn == 0) {
                ParentIndex = i;
                cn++;
            } else if (hierarchy[i][2] != -1 && cn == 1) {
                cn++;
            } else if (hierarchy[i][2] == -1) {
                //初始化
                ParentIndex = -1;
                cn = 0;
            }

            //如果该轮廓存在子轮廓，且有2级子轮廓则认定找到‘回’
            if (hierarchy[i][2] != -1 && cn == 2) {
                drawContours(canvas, contours, ParentIndex, Scalar::all(255), -1);

                RotatedRect rect;

                rect = minAreaRect(contours[ParentIndex]);

                rect_points.push_back(rect.center);

            }

        }

        //将找到地‘回’连接起来
        for (int i = 0; i < rect_points.size(); i++) {
            line(canvas, rect_points[i], rect_points[(i + 1) % rect_points.size()],
                 Scalar::all(255), 5);
        }

        QR_Rect.push_back(rect_points);

    }


    return QR_Rect.size();

}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_code_detection_QRCodeUtils_detectionQRCode(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat mat;
    BitmapMatUtils::bitmap2mat(env, bitmap, mat);
    if (mat.empty()) {
        LOGE("Mat is empty");
        return bitmap;
    }

    vector<Mat> ROI_Rect;
    Find_QR_Rect(mat, ROI_Rect);

    Mat canvas = Mat::zeros(mat.size(), mat.type());
    int counts = Dectect_QR_Rect(mat, canvas, ROI_Rect);
    if (counts <= 0) {
        LOGE("Can not detect QR code!");
        return bitmap;
    }
    LOGE("检测到%d个二维码。", counts);
    //框出二维码所在位置
    Mat gray;
    cvtColor(canvas, gray, COLOR_BGR2GRAY);

    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Point2f points[4];

    for (int i = 0; i < contours.size(); i++) {
        RotatedRect rect = minAreaRect(contours[i]);

        rect.points(points);

        for (int j = 0; j < 4; j++) {
            line(mat, points[j], points[(j + 1) % 4], Scalar(0, 255, 0), 2);
        }

    }
    imwrite("/sdcard/opencv/code_1.png", mat);
    jobject new_bitmap = BitmapMatUtils::createBitmap(env,mat.cols,mat.rows,"ARGB_8888");
    BitmapMatUtils::mat2bitmap(env, mat, new_bitmap);
    return new_bitmap;


}