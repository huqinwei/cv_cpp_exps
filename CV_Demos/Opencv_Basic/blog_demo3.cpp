#include "blog_demo3.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;


void filters_demo()
{
	Mat img = imread("data/dota2.jpg");
	imshow("origin", img);


	//线性滤波：方框滤波、均值滤波、高斯滤波
	//方框滤波
	Mat box1, box2,box3;
	boxFilter(img, box1, -1, Size(5, 5));//-1原图深度？
	imshow("box filter1", box1);
	boxFilter(img, box2, -1, Size(15, 15));//框大，模糊
	imshow("box filter2", box2);
	boxFilter(img, box3, -1, Size(1, 1));//框越小，越不模糊
	imshow("box filter3", box3);

	//均值滤波——方框滤波的特殊情况——算全局方框么？但是也有尺寸啊
	Mat blur1, blur2;
	blur(img, blur1, Size(5, 5));
	imshow("blur1", blur1);
	blur(img, blur2, Size(25, 25));
	imshow("blur2", blur2);

	//高斯滤波
	Mat gauss1, gauss2;
	GaussianBlur(img, gauss1, Size(3, 3), 0, 0);
	imshow("gauss1", gauss1);
	GaussianBlur(img, gauss2, Size(15, 15), 0, 0);
	imshow("gauss2", gauss2);


	//非线性滤波：中值滤波、双边滤波
	//中值滤波，有点像average pooling又恢复回来，中值，不是均值，是中位数。
	//平均的话，噪声也算进来了，中值就不一样了。
	Mat med;
	medianBlur(img, med, 7);//第三个参数表示孔径的线性尺寸，它的值必须是大于1的奇数
	imshow("median", med);
	//双边滤波
	Mat bi;
	bilateralFilter(img, bi, 25, 25 * 2, 25 / 2);
	imshow("bi", bi);



	waitKey(0);
}





