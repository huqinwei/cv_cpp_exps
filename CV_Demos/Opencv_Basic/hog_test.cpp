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

	//����ʹ����������ݶȵķ�ֵ�ͷ���
	Mat mag, angle;
	cartToPolar(gx, gy, mag, angle, 1);

	//�о�ȱ��һ����ֱ��ͼ��û�аɣ�������ݶȺͽǶȣ�ûͳ��

	//ûʲô�������˵opencv���ӻ��Ѿ����������ûһ����ӡ���
	

}
#endif