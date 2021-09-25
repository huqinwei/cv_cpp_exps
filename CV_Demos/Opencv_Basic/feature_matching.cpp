#include "feature_matching.h"



#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>


int orb__sift_feature_matching_with_rotation() {
    //实验条件：windows画图画的一个人字形“角点”，在手机端做了一定角度的旋转，两个图size不一样，分别使用不同的detector和resize scale进行对比实验
    //实验结果：1.sift对旋转不变性的适应要更好


    cv::Mat img_dota = cv::imread("data/dota2.jpg");//暂时没用上

    cv::Mat img = cv::imread("data/corner.png");
    cv::Mat img_rotated = cv::imread("data/corner_rotated.png");

    double resize_ratio = 1;//缩放比例，缩放来保证特征匹配效果
    cv::resize(img, img, cv::Size(img.cols*resize_ratio, img.rows*resize_ratio));
    cv::resize(img_rotated, img_rotated, cv::Size(img_rotated.cols*resize_ratio, img_rotated.rows*resize_ratio));


    bool use_orb=false;
    cv::Ptr<cv::Feature2D> detector_;
    if(use_orb)
        detector_ = cv::ORB::create(50);//cv::Ptr<cv::ORB> 
    else
        detector_ = cv::xfeatures2d::SIFT::create(0, 3, 0.04, 20);//sift对比，旋转不变性的支持//cv::Ptr<cv::Feature2D> 
    std::vector<cv::KeyPoint> Keypoints1, Keypoints2,KeyPoints_dota;
    cv::Mat descriptors1, descriptors2, desc_dota;
    //orb->detectAndCompute(img_dota, cv::noArray(), KeyPoints_dota, desc_dota);
    detector_->detectAndCompute(img, cv::Mat(), Keypoints1, descriptors1);
    detector_->detectAndCompute(img_rotated, cv::Mat(), Keypoints2, descriptors2);

#if 0 //单图展示
    cv::Mat ShowKeypoints1, ShowKeypoints2;
    cv::drawKeypoints(img, Keypoints1, ShowKeypoints1);
    cv::imshow("Keypoints", ShowKeypoints1);
    cv::waitKey(2000);
#endif
    
    //todo：为了更好的匹配，替换匹配方法
    //Matching
    std::vector<cv::DMatch> matches;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");//BruteForce//BruteForce-Hamming
    matcher->match(descriptors1, descriptors2, matches);
    std::cout << "find out total " << matches.size() << " matches" << std::endl;

    //可视化
    cv::Mat ShowMatches;
    cv::drawMatches(img, Keypoints1, img_rotated, Keypoints2, matches, ShowMatches);
    cv::imshow("matches", ShowMatches);
    cv::waitKey(0);
    


    return 0;
}


int fast_feature_detection() {
    cv::Mat img = cv::imread("data/dota2.jpg");
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, CV_RGB2GRAY);


    //需要nonfree的支持
    //cv::Ptr<cv::FeatureDetector> fast = cv::FeatureDetector::create("FAST");




    return 0;
}