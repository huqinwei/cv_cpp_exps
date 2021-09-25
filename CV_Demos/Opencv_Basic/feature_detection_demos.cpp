#include "feature_detection_demos.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;
Mat src, src_gray;
Mat dst, detected_edges;
int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";
static void CannyThreshold(int, void*)
{
    blur(src_gray, detected_edges, Size(3, 3));
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
    dst = Scalar::all(0);
    src.copyTo(dst, detected_edges);
    imshow(window_name, dst);
}
int canny_detection()//¹Ù·½Àý×Ó
{
    src = imread("data/dota2.jpg", IMREAD_COLOR); // Load an image
    dst.create(src.size(), src.type());
    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    namedWindow(window_name, WINDOW_AUTOSIZE);
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
    CannyThreshold(0, 0);
    waitKey(0);
    return 0;
}

// int canny_detection2()//https://www.bilibili.com/video/BV1qk4y1r7jw?p=4
// {
//     src = imread("data/dota2.jpg", IMREAD_GRAYSCALE); // Load an image
// 
//     namedWindow("edge_detection");
//     createTrackbar("minThreshold", "edge_detection", 50, 1000, lambda x : x);
//     createTrackbar("maxThreshold", "edge_detection", 100, 1000, lambda x : x);
// 
//     createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
//     CannyThreshold(0, 0);
// 
//     blur(src_gray, detected_edges, Size(3, 3));
//     Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
//     dst = Scalar::all(0);
//     src.copyTo(dst, detected_edges);
//     imshow(window_name, dst);
// 
//     waitKey(0);
//     return 0;
// }





