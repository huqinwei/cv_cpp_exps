#include "blog_demo7.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>
using namespace std;
using namespace cv;
/*
ʶ��ͼ���е�Բ��ֱ��
�����߱任
�����߱任��һ����ͼ����Ѱ��ֱ�ߵķ�����OpenCV��֧�����ֻ����߱任���ֱ��Ǳ�׼�����߱任����߶Ȼ����߱任���ۼƸ��ʻ����߱任��
��OpenCV�п��Ե��ú���HoughLines�����ñ�׼�����߱任�Ͷ�߶Ȼ����߱任��HoughLinesP�������ڵ����ۻ����ʻ����߱任��
���Ƕ�֪������ά�������ϱ�ʾһ��ֱ�ߵķ���ʽy = a*x + b�����������һ��ֱ�߾͵��뷽�跨������е�a��b��ֵ������ü���������ʾ����
rho=xcosTheta+ysinTheta
theta����ֱ����ˮƽ�����ɵĽǶȣ���rho����Բ�İ뾶��Ҳ�������Ϊԭ�㵽ֱ�ߵľ��룩��ͬ���أ�����������Ҳ�Ǳ���һ��ֱ�ߵ���Ҫ������
ȷ���������ˣ�Ҳ��ȷ��һ��ֱ���ˡ�������ͼ��ʾ��

��������Ѿ���ֽ�ϻ������ˣ�rho�ǳ��ȣ�x��y�ǵ����꣬theta�ǽǶȣ�������rho��x��yû���ˡ�

��OpenCV�����ֻ�����HoughLines���ǿ��Եõ�����һ��ֱ�ߵ�����������ֵ��
*/

void blog_demo7_HoughLines()
{
	Mat img = imread("data/building2.jpg");//building1������
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat midImage, dstImage;
	Canny(img, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);//�ҶȻ������ˣ�
	imshow("canny", midImage);
	imshow("gray?",dstImage);
	vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300,0,0);//�õ�ֱ�߼���
	//����5����ֵ��Խ�󣬼��Խ��׼���ٶ�Խ�죬���٣�ÿ��ͼЧ������һ���������Ҫ�ֶ���
	//�õ���lines����rho��theta��û��ֱ���ϵĵ㣬����rho��theta����ֱ��
	//����
	for (size_t i = 0; i < lines.size(); i++)//ѭ��ʲô��˼��ÿ���߶�����ͼ��
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;//�����ýǶȺ�б�������һ�x��y��x0,y0����һ�����㡣����ĵ�
		//���ǵõ������㣬�ٻ���ֱ�ߣ�������ÿ����dstImage���ߣ�ÿ�ζ�ˢ��imshow��
		pt1.x = cvRound(x0 + 1000 * (-b));//todo:cvRound��1000����ʲô��˼��
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		float angle = theta / CV_PI * 180;
		cout << "line " << i << ": " << "rho:" << rho << " theta:" << theta << " angle:" << angle << endl;

		//todo:LINE_AA�ǣ���line���ͣ��������λ��Ҳ��һ���ˣ�����3.x��,LINE_AA����CV_AA
		line(dstImage, pt1, pt2, Scalar( i*5,i*10,i*15/*55, 100,195*/), 1, CV_AA/*LINE_AA*/);//Scalar�����߶���ɫ�������Ǹ�չʾ������ν�����ԼӸ���������Ч��
		imshow("��Ե�����ͼ", midImage);
		imshow("����Ч��ͼ", dstImage);


	}





	waitKey();
}
void blog_demo7_HoughLinesP()
{
	Mat srcImage = imread("data/building2.jpg");//building1��Ч�����Ժ��ڲ���P��

	imshow("Src Pic", srcImage);

	Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	vector<Vec4i> lines;//4��i��������4���������ˣ���֮ͬǰ���������������ֱ�ߣ����ڿ��������������ֱ�ߣ������Լ�������������
	//��HoughLines��ͬ���ǣ�HoughLinesP�õ�lines���Ǻ���ֱ���ϵ������ģ�����������л���ʱ�Ͳ�����Ҫ�Լ������������ȷ��Ψһ��ֱ����
	HoughLinesP(midImage, lines, 1, CV_PI / 180, 80, 50, 10);//ע������������Ϊ��ֵ

															 //���λ���ÿ���߶�
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];

		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, CV_AA/*LINE_AA*/); //Scalar�������ڵ����߶���ɫ

		imshow("��Ե�����ͼ", midImage);
		imshow("����Ч��ͼ", dstImage);
	}
	waitKey();
}
void blog_demo7_HoughCircles()
{
	Mat srcImage = imread("data/balls.jpg");
	Mat midImage, dstImage;//��ʱ������Ŀ��ͼ�Ķ���  

	imshow("��ԭʼͼ��", srcImage);

	//��3��תΪ�Ҷ�ͼ������ͼ��ƽ��  
	cvtColor(srcImage, midImage, CV_BGR2GRAY);//ת����Ե�����ͼΪ�Ҷ�ͼ  
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//��4�����л���Բ�任  
	vector<Vec3f> circles;//��ǰ�ߵ���Ҫ����Ҳ���ǣ��ӿڲ�һ�����������Ľ���������һ����������float�ߣ�Բ��+�뾶��
	HoughCircles(midImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 150, 0, 0); //ע����ߵĲ���Ϊ��ֵ���������е�����ֵԽ�󣬼���Բ����׼

																				 //��5��������ͼ�л��Ƴ�Բ  
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//����Բ��  
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//����Բ����  
		circle(srcImage, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	//��6����ʾЧ��ͼ    
	imshow("��Ч��ͼ��", srcImage);

	waitKey(0);
}