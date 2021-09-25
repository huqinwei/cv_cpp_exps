#include "hog_test.h"
#include <stdio.h>
//#include "highgui/highgui.hpp"
#include <opencv/highgui.h>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#if 0
using namespace cv;
void hog_demo()
{
	printf("hello\n");

	Mat img = imread("data/bolt.png", 1);//D:\VS2015\Projects
	img.convertTo(img, CV_32F, 1 / 255.0);
	//Mat gx, gy;
    CvArr* img_src = (CvArr*)&img;
    CvArr gx;
    CvArr gy;
	cvSobel(img_src, &gx,  1, 0, 1);
    cvSobel(img_src, &gy,  0, 1, 1);

	//现在使用上面计算梯度的幅值和方向
	Mat mag, angle;
	cartToPolar(gx, gy, mag, angle, 1);

	//感觉缺了一步，直方图，没有吧，这才是梯度和角度，没统计

	//没什么结果，据说opencv可视化费劲，所以这就没一个打印结果
	

}
#endif