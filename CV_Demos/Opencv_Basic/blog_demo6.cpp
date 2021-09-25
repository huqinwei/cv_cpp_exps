#include "blog_demo6.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;


void blog_demo6_canny()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat dst,edge,grayImage;
	dst.create(img.size(), img.type());
	cvtColor(img, grayImage, COLOR_BGR2GRAY);
	blur(grayImage, edge, Size(3, 3));//�����õ�
	Canny(edge, edge, 3, 9, 3);//��ֵ1����ֵ2���׾���
	imshow("canny", edge);


	waitKey();
}

void blog_demo6_sobel()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y, dst;
	//x�����ݶȣ�ʵ�ʿ���νx���򣬸��������Ե��Ҳ���Ǻ����ݶ����Ե�λ��==��������������������������dx=1,dy=0û�����ǿ��ܾ��ǿ��ݶȣ����ߵ�Ҳ�����Ч�����������ô��ˣ�
	Sobel(img, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	imshow("x sobel(before abs)", grad_x);//������
	imshow("x sobel", abs_grad_x);
	Sobel(img, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);//ǰ������������ָ����ɣ�ע��x��y���෴�ģ���������filtersize�����������scale֮���
	convertScaleAbs(grad_y, abs_grad_y);
	imshow("y sobel(before abs)", grad_y);//������
	imshow("y sobel", abs_grad_y);

	addWeighted(abs_grad_x, 0.8, abs_grad_y, 0.2, 0, dst);//������������Լ���������
	imshow("total sobel 1", dst);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);//������������Լ���������
	imshow("total sobel 2", dst);




	waitKey();
}
void blog_demo6_laplacian()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat gray, dst, abs_dst,img_blurred;
	GaussianBlur(img, img_blurred, Size(3, 3), 0, 0, BORDER_DEFAULT);//��˹��blur��blurģ�������롣
	cvtColor(img_blurred, gray, COLOR_RGB2GRAY);
	Laplacian(gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(gray)",abs_dst);

	Laplacian(img_blurred, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);//��ɫ��ʵ�����ǲ��󣬶�������е���ɫ
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(color)", abs_dst);

	Laplacian(img, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);//��blur�Ļ���ȷʵ������Ϣ����һЩ���ӡ���Ե��ȡ���Ƕȣ����������ˡ���Ϊ�Ǳ�ԵҲ�����ˡ�
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(non blur)", abs_dst);



	waitKey();
}