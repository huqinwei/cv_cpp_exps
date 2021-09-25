#include "blog_demo4.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;
//��ʴ������
//���������Ÿ�������
//��ʴ�Ǽ��ٸ�������

/*
opencv����һ���ܺõĺ���getStructuringElement������ֻҪ�������������Ӧ�Ĵ���������Ϳ��Խ�����Ӧ�Ĳ����ˣ�ʹ�������ǳ����㡣
���Ҳ����������ͱ����㣬ֱ��һ��������,��morphologyEx

�����㣺�ȸ�ʴ�����ͣ���������С����
�����㣺�������ٸ�ʴ�������ų�С�ͺڶ�
��̬ѧ�ݶȣ���������ͼ�븩��ͼ֮����ڱ�������ı�Ե������
��ñ��ԭͼ���뿪����ͼ֮����ڷ�����ڽ�����һЩ�İ߿顣
��ñ����������ԭͼ��֮����ڷ�����ڽ��㰵һЩ�İ߿顣

*/

void blog_demo4_dilate()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	//imshow("origgg", img);//֤�����־��Ǵ��ڵ�key���Բ��Ͼ���һ�����Ե��Ͼ���ǰ���������Ǹ�
	imshow("origin", img);

	Mat dst;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(15, 15));//���εľ���ˣ�����������״ MORPH_CROSS
	cout << element1 << endl;
	Mat element2 = getStructuringElement(MORPH_CROSS, Size(15, 15));//ʮ�ֽ���
	cout << element2 << endl;
	Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));//��Բ��
	cout << element3 << endl;

	dilate(img, dst, element1);
	namedWindow("dilate1", WINDOW_NORMAL);
	imshow("dilate1", dst);

	dilate(img, dst, element2);
	namedWindow("dilate2", WINDOW_NORMAL);
	imshow("dilate2", dst);

	dilate(img, dst, element3);
	namedWindow("dilate3", WINDOW_NORMAL);
	imshow("dilate3", dst);


	waitKey();
}

void blog_demo4_erode()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat dst;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	erode(img, dst, element);
	namedWindow("erode", WINDOW_NORMAL);
	imshow("erode", dst);


	waitKey();
}

void blog_demo4_morphologyEx()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat dst;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));

	//�߼���̬ѧ����������������Ϳ����ˣ�����Ҫѡ�����ֲ��������޸ĵ����������Ϳ����ˡ�������ʾ������̬ѧ�ݶȴ���
	morphologyEx(img, dst, MORPH_GRADIENT, element);
	namedWindow("gradient", WINDOW_NORMAL);
	imshow("gradient", dst);

	morphologyEx(img, dst, MORPH_OPEN, element);
	namedWindow("open", WINDOW_NORMAL);
	imshow("open", dst);
	morphologyEx(img, dst, MORPH_CLOSE, element);
	namedWindow("close", WINDOW_NORMAL);
	imshow("close", dst);
	morphologyEx(img, dst, MORPH_TOPHAT, element);
	namedWindow("tophat", WINDOW_NORMAL);
	imshow("tophat", dst);
	morphologyEx(img, dst, MORPH_BLACKHAT, element);
	namedWindow("blackhat", WINDOW_NORMAL);
	imshow("blackhat", dst);

	waitKey();


}
