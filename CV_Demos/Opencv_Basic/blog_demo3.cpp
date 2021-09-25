#include "blog_demo3.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;


void filters_demo()
{
	Mat img = imread("data/dota2.jpg");
	imshow("origin", img);


	//�����˲��������˲�����ֵ�˲�����˹�˲�
	//�����˲�
	Mat box1, box2,box3;
	boxFilter(img, box1, -1, Size(5, 5));//-1ԭͼ��ȣ�
	imshow("box filter1", box1);
	boxFilter(img, box2, -1, Size(15, 15));//���ģ��
	imshow("box filter2", box2);
	boxFilter(img, box3, -1, Size(1, 1));//��ԽС��Խ��ģ��
	imshow("box filter3", box3);

	//��ֵ�˲����������˲����������������ȫ�ַ���ô������Ҳ�гߴ簡
	Mat blur1, blur2;
	blur(img, blur1, Size(5, 5));
	imshow("blur1", blur1);
	blur(img, blur2, Size(25, 25));
	imshow("blur2", blur2);

	//��˹�˲�
	Mat gauss1, gauss2;
	GaussianBlur(img, gauss1, Size(3, 3), 0, 0);
	imshow("gauss1", gauss1);
	GaussianBlur(img, gauss2, Size(15, 15), 0, 0);
	imshow("gauss2", gauss2);


	//�������˲�����ֵ�˲���˫���˲�
	//��ֵ�˲����е���average pooling�ָֻ���������ֵ�����Ǿ�ֵ������λ����
	//ƽ���Ļ�������Ҳ������ˣ���ֵ�Ͳ�һ���ˡ�
	Mat med;
	medianBlur(img, med, 7);//������������ʾ�׾������Գߴ磬����ֵ�����Ǵ���1������
	imshow("median", med);
	//˫���˲�
	Mat bi;
	bilateralFilter(img, bi, 25, 25 * 2, 25 / 2);
	imshow("bi", bi);



	waitKey(0);
}





