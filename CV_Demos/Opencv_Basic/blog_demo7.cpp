#include "blog_demo7.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>
using namespace std;
using namespace cv;
/*
识别图像中的圆和直线
霍夫线变换
霍夫线变换是一种在图像中寻找直线的方法。OpenCV中支持三种霍夫线变换，分别是标准霍夫线变换、多尺度霍夫线变换、累计概率霍夫线变换。
在OpenCV中可以调用函数HoughLines来调用标准霍夫线变换和多尺度霍夫线变换。HoughLinesP函数用于调用累积概率霍夫线变换。
我们都知道，二维坐标轴上表示一条直线的方程式y = a*x + b，我们想求出一条直线就得想方设法求出其中的a和b的值。如果用极坐标来表示就是
rho=xcosTheta+ysinTheta
theta就是直线与水平线所成的角度，而rho就是圆的半径（也可以理解为原点到直线的距离），同样地，这两个参数也是表征一条直线的重要参数，
确定他们俩了，也就确定一条直线了。正如下图所示。

这个，我已经从纸上画出来了，rho是长度，x和y是点坐标，theta是角度，最后求出rho。x和y没用了。

在OpenCV里，我们只需调用HoughLines就是可以得到表征一条直线的这两个参数值！
*/

void blog_demo7_HoughLines()
{
	Mat img = imread("data/building2.jpg");//building1有亮光
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat midImage, dstImage;
	Canny(img, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);//灰度化？反了？
	imshow("canny", midImage);
	imshow("gray?",dstImage);
	vector<Vec2f> lines;
	HoughLines(midImage, lines, 1, CV_PI / 180, 300,0,0);//得到直线集合
	//参数5：阈值，越大，检测越精准，速度越快，线少，每张图效果都不一样，这个需要手动调
	//得到的lines包含rho和theta，没有直线上的点，根据rho和theta建立直线
	//画线
	for (size_t i = 0; i < lines.size(); i++)//循环什么意思？每条线都画个图？
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;//这是用角度和斜边重新找回x和y，x0,y0算是一个定点。最近的点
		//他是得到两个点，再画出直线，估计是每次在dstImage画线，每次都刷新imshow，
		pt1.x = cvRound(x0 + 1000 * (-b));//todo:cvRound和1000都是什么意思？
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		float angle = theta / CV_PI * 180;
		cout << "line " << i << ": " << "rho:" << rho << " theta:" << theta << " angle:" << angle << endl;

		//todo:LINE_AA是？是line类型？好像参数位置也不一样了，他是3.x的,LINE_AA换成CV_AA
		line(dstImage, pt1, pt2, Scalar( i*5,i*10,i*15/*55, 100,195*/), 1, CV_AA/*LINE_AA*/);//Scalar调节线段颜色，这算是个展示，无所谓，可以加个变量看看效果
		imshow("边缘检测后的图", midImage);
		imshow("最终效果图", dstImage);


	}





	waitKey();
}
void blog_demo7_HoughLinesP()
{
	Mat srcImage = imread("data/building2.jpg");//building1的效果明显好于不带P的

	imshow("Src Pic", srcImage);

	Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	vector<Vec4i> lines;//4个i？现在是4个点坐标了，不同之前的两个浮点数组成直线，现在可以用两个点组成直线，不用自己计算两个点了
	//与HoughLines不同的是，HoughLinesP得到lines的是含有直线上点的坐标的，所以下面进行划线时就不再需要自己求出两个点来确定唯一的直线了
	HoughLinesP(midImage, lines, 1, CV_PI / 180, 80, 50, 10);//注意第五个参数，为阈值

															 //依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];

		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, CV_AA/*LINE_AA*/); //Scalar函数用于调节线段颜色

		imshow("边缘检测后的图", midImage);
		imshow("最终效果图", dstImage);
	}
	waitKey();
}
void blog_demo7_HoughCircles()
{
	Mat srcImage = imread("data/balls.jpg");
	Mat midImage, dstImage;//临时变量和目标图的定义  

	imshow("【原始图】", srcImage);

	//【3】转为灰度图，进行图像平滑  
	cvtColor(srcImage, midImage, CV_BGR2GRAY);//转化边缘检测后的图为灰度图  
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//【4】进行霍夫圆变换  
	vector<Vec3f> circles;//和前边的主要区别也就是，接口不一样，传进来的接收容器不一样，拿三个float走，圆心+半径。
	HoughCircles(midImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 150, 0, 0); //注意第七的参数为阈值，可以自行调整，值越大，检测的圆更精准

																				 //【5】依次在图中绘制出圆  
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//绘制圆心  
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//绘制圆轮廓  
		circle(srcImage, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	//【6】显示效果图    
	imshow("【效果图】", srcImage);

	waitKey(0);
}