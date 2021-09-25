
#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <chrono>
#include <windows.h>
//#include "LLog.h"

using namespace std;
using namespace cv;
using namespace std::chrono;




#define _CRT_SECURE_NO_WARNINGS



//大小同图，特征匹配，验证不同尺寸的匹配优先级
//将小图贴到大图，组成新图，再同原图对比
void featureKeypointScaleDemo();

void featureKeypointScaleAndRotateDemo();

