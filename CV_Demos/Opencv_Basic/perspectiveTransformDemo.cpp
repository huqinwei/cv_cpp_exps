
#include "perspectiveTransformDemo.h"
#if 0
void perspectiveTransformDemo1()
{

	Mat img = imread("data/1.jpg", IMREAD_UNCHANGED);//D:\VS2015\Projects
	int img_height = img.rows;
	int img_width = img.cols;
	vector<Point2f> corners(4);
	corners[0] = Point2f(0, 0);
	corners[1] = Point2f(img_width - 1, 0);
	corners[2] = Point2f(0, img_height - 1);
	corners[3] = Point2f(img_width - 1, img_height - 1);

	//todo，如果我要负轴呢？也许还是办不到？也许正确方法应该是，放大背景，把本该映射到负轴的，提前改到正轴？所以主要还是改homo这一步
	vector<Point2f> corners_trans(4);
	corners_trans[0] = Point2f(150, 250);
	corners_trans[1] = Point2f(771, 0);
	corners_trans[2] = Point2f(0, img_height - 1);
	corners_trans[3] = Point2f(650, img_height - 1);


	//我用原图坐标x,y，findhomo找出来的四个角x',y'，换成大图的一部分x'',y''，直接用x,y和x'',y''就得到了新的transform？
	//原图坐标有了！homo后的有了！剩最后一步，新坐标是什么合适？自己计算，
// 	large_width-origin_width=x_increment
// 	large_height- origin_width=y_increment
// 	让origin以x_increment，y_increment为起点进入large？这只是new在左上的情况，必须用new的corner来算，
	//只要确定new的方位，就能算出origin要放在large的什么位置。也能算出new应该再large的什么位置（x'',y''）



	Mat transform = getPerspectiveTransform(corners, corners_trans);//两套角坐标求出来的trans，才能给后边用，形式和单应的homo矩阵有区别？也不能负轴？
	cout << transform << endl;//看起来和homo是一样的形式
	vector<Point2f> points, points_trans;
	for(int i = 0; i < img_height; i++)//高度是y
	{
		for (int j = 0; j < img_width; j++)//宽度是x
		{
			points.push_back(Point2f(j, i));//所有点都要压进来？是所有坐标
		}
	}

	perspectiveTransform(points, points_trans, transform);
	Mat img_trans = Mat::zeros(img_height, img_width, CV_8UC3);
	int count = 0;
	for (int i = 0; i < img_height; i++) {
		uchar* p = img.ptr<uchar>(i);
		for (int j = 0; j < img_width; j++) {
			int y = points_trans[count].y;
			int x = points_trans[count].x;
			uchar* t = img_trans.ptr<uchar>(y);//t指向的是img_trans，注意看宽高的设置
			t[x * 3] = p[j * 3];//x是新坐标，j是旧坐标y呢？整列操作
			t[x * 3 + 1] = p[j * 3 + 1];
			t[x * 3 + 2] = p[j * 3 + 2];//+1和+2不是指三通道吧？但是j一次只+1，怎么能是三通道？
			count++;

		}
	}
	imshow("windows",img_trans);
	waitKey();


	//更多的问题：这种变换有黑色小点，需要插值，或者另一种方法，直接在新图反向找原图的点。https://blog.csdn.net/xiaowei_cqu/article/details/26471527
	//另一种方法就是findHomography了

}


#endif