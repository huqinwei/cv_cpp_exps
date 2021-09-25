#include "blog_demo4.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;
//腐蚀和膨胀
//膨胀是扩张高亮部分
//腐蚀是减少高亮部分

/*
opencv里有一个很好的函数getStructuringElement，我们只要往这个函数传相应的处理参数，就可以进行相应的操作了，使用起来非常方便。
里边也包含开运算和闭运算，直接一步就做了,用morphologyEx

开运算：先腐蚀再膨胀，用来消除小物体
闭运算：先膨胀再腐蚀，用于排除小型黑洞
形态学梯度：就是膨胀图与俯视图之差，用于保留物体的边缘轮廓。
顶帽：原图像与开运算图之差，用于分离比邻近点亮一些的斑块。
黑帽：闭运算与原图像之差，用于分离比邻近点暗一些的斑块。

*/

void blog_demo4_dilate()
{
	Mat img = imread("data/dota2.jpg");
	namedWindow("origin", WINDOW_NORMAL);
	//imshow("origgg", img);//证明名字就是窗口的key，对不上就另开一个，对得上就用前边声明的那个
	imshow("origin", img);

	Mat dst;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(15, 15));//矩形的卷积核，还有其他形状 MORPH_CROSS
	cout << element1 << endl;
	Mat element2 = getStructuringElement(MORPH_CROSS, Size(15, 15));//十字交叉
	cout << element2 << endl;
	Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));//椭圆？
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

	//高级形态学处理，调用这个函数就可以了，具体要选择哪种操作，就修改第三个参数就可以了。这里演示的是形态学梯度处理
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
