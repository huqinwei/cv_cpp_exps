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
	blur(grayImage, edge, Size(3, 3));//降噪用的
	Canny(edge, edge, 3, 9, 3);//阈值1，阈值2，孔径？
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
	//x方向梯度，实际看所谓x方向，更像纵向边缘，也许是横向梯度明显的位置==纵向线条？看参数命名，好像dx=1,dy=0没错啊。那可能就是看梯度，作者的也是这个效果（除非他用错了）
	Sobel(img, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	imshow("x sobel(before abs)", grad_x);//出不来
	imshow("x sobel", abs_grad_x);
	Sobel(img, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);//前两个数字算是指定轴吧，注意x和y是相反的，第三个是filtersize，第四五个是scale之类的
	convertScaleAbs(grad_y, abs_grad_y);
	imshow("y sobel(before abs)", grad_y);//出不来
	imshow("y sobel", abs_grad_y);

	addWeighted(abs_grad_x, 0.8, abs_grad_y, 0.2, 0, dst);//这个比例可以自己调整试试
	imshow("total sobel 1", dst);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);//这个比例可以自己调整试试
	imshow("total sobel 2", dst);




	waitKey();
}
void blog_demo6_laplacian()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	imshow("origin", img);

	Mat gray, dst, abs_dst,img_blurred;
	GaussianBlur(img, img_blurred, Size(3, 3), 0, 0, BORDER_DEFAULT);//高斯的blur，blur模糊，降噪。
	cvtColor(img_blurred, gray, COLOR_RGB2GRAY);
	Laplacian(gray, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(gray)",abs_dst);

	Laplacian(img_blurred, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);//彩色其实区别倒是不大，顶多就是有点颜色
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(color)", abs_dst);

	Laplacian(img, dst, CV_16S, 3, 1, 0, BORDER_DEFAULT);//不blur的话，确实可能信息多了一些，从“边缘提取”角度，算是噪音了。因为非边缘也很亮了。
	convertScaleAbs(dst, abs_dst);
	imshow("laplace(non blur)", abs_dst);



	waitKey();
}