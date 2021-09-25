#include "blog_demo5.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;
//ͼƬ���ź�ͼ�������
/*
resize�������Խ�Դͼ��ȷ��ת��Ϊָ���ߴ��Ŀ��ͼ��
Ҫ��Сͼ��һ���Ƽ�ʹ��CV_INETR_AREA����ֵ����Ҫ�Ŵ�ͼ���Ƽ�ʹ��CV_INTER_LINEAR��
*/
void blog_demo5_resize_specifiedsize()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat dst = Mat::zeros(240, 320, CV_8UC3);
	resize(img, dst, dst.size());
	imshow("after resize", dst);




	waitKey();
}
void blog_demo5_resize_specifiedratio()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat dst;
	resize(img, dst, Size(), 0.5, 0.5);
	imshow("after resize", dst);


	waitKey();
}
void blog_demo5_pyramid()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);
	
	Mat down, down_up, up;
	resize(img, up, Size(), 2.0, 2.0);
	resize(img, down, Size(), 0.5, 0.5);
	resize(down, down_up, Size(), 2.0, 2.0);//����϶�Ҫ��ʧ��Ϣ��
	imshow("up", up);
	imshow("down", down);
	imshow("down_up", down_up);//�����һ���Աȣ��ֶ������ϲ������������²������ϲ�����


	//�������Ҳһ�㣬��ûʵ����ʾ����������˵����ֱ���Ƽ�resize����ƪ������ˡ�����������
	Mat dst, dst2;
	pyrUp(img, dst, Size(img.cols * 2, img.rows * 2));//! upsamples and smoothes the image
	pyrDown(img, dst2, Size(img.cols*0.5, img.rows*0.5));
	imshow("after upsample", dst);
	imshow("after downsample", dst2);//ֻ�Ա�resize�Ĵ�down������ȷʵģ���ˡ�����������Ϊsmoooth��



	waitKey();
}


