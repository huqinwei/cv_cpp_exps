#include "blur.h"
#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <chrono>
#include <windows.h>

using namespace std;
using namespace cv;
using namespace std::chrono;


//实验结论：//越大越模糊
//锚点没改，-1代表中心？他是这么描述的，况且均值测不出来中心点
void blur_test()
{
#if 0
	char im1_path[512];
	//D:\experiment_data\pictures_other
	sprintf(im1_path, "D:\\experiment_data\\pictures_other\\dota1.jpg");
	cv::Mat im1 = imread(im1_path, cv::IMREAD_UNCHANGED);//IMREAD_UNCHANGED

	cv::Mat im2, im3, im4, im5;
	blur(im1, im2, cv::Size(1, 1), cv::Point(-1, -1));
	blur(im1, im3, cv::Size(3, 3), cv::Point(-1, -1));
	blur(im1, im4, cv::Size(5, 5), cv::Point(-1, -1));
	blur(im1, im5, cv::Size(19, 19), cv::Point(-1, -1));
	imshow("before", im1);
	imshow("after:2", im2);
	imshow("after:3", im3);
	imshow("after:4", im4);
	imshow("after:5", im5);
	waitKey();
#endif
	cv::Mat im;
	//todo:访问mat，自己操作mat元素，然后看滤波结果

}




void gaussian_blur_test()
{
#if 1
	//char im1_path[512];
	//D:\experiment_data\pictures_other
	//sprintf_s(im1_path, "D:\\experiment_data\\pictures_other\\dota1.jpg");
	//std::string img1_path = "../../../../../data/dota1.jpg";
	std::string img1_path = "D:/alk_workspace/exp_code_collection_root_path/data/dota1.jpg";

	cv::Mat im1 = imread(img1_path, cv::IMREAD_UNCHANGED);//IMREAD_UNCHANGED

	cv::Mat im2, im3, im4, im5;
	GaussianBlur(im1, im2, cv::Size(1, 1), 11, 11);
	GaussianBlur(im1, im3, cv::Size(3, 3), 11, 11);//标准差？和高斯分布有关的参数吧！3*3中参考的是11和11
	GaussianBlur(im1, im4, cv::Size(5, 5), 11, 11);
	GaussianBlur(im1, im5, cv::Size(19, 19), 11, 11);
	imshow("before", im1);
	imshow("after:2", im2);
	imshow("after:3", im3);
	imshow("after:4", im4);
	imshow("after:5", im5);
	waitKey();
#endif
	cv::Mat im;
	//todo:访问mat，自己操作mat元素，然后看滤波结果

}
