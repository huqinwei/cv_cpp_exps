#include "blog_demo5.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;
//图片缩放和图像金字塔
/*
resize函数可以将源图像精确地转化为指定尺寸的目标图像。
要缩小图像，一般推荐使用CV_INETR_AREA来插值；若要放大图像，推荐使用CV_INTER_LINEAR。
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
	resize(down, down_up, Size(), 2.0, 2.0);//这个肯定要损失信息了
	imshow("up", up);
	imshow("down", down);
	imshow("down_up", down_up);//最好来一个对比，手动纯粹上采样，或者是下采样再上采样？


	//好像这个也一般，他没实际演示金字塔，还说缩放直接推荐resize，这篇就完结了。。。。。。
	Mat dst, dst2;
	pyrUp(img, dst, Size(img.cols * 2, img.rows * 2));//! upsamples and smoothes the image
	pyrDown(img, dst2, Size(img.cols*0.5, img.rows*0.5));
	imshow("after upsample", dst);
	imshow("after downsample", dst2);//只对比resize的纯down，好像确实模糊了。。。。。因为smoooth？



	waitKey();
}


