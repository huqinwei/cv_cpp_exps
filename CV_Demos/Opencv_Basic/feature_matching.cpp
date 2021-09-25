#include "feature_matching.h"



#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>


int orb__sift_feature_matching_with_rotation() {
    //ʵ��������windows��ͼ����һ�������Ρ��ǵ㡱�����ֻ�������һ���Ƕȵ���ת������ͼsize��һ�����ֱ�ʹ�ò�ͬ��detector��resize scale���жԱ�ʵ��
    //ʵ������1.sift����ת�����Ե���ӦҪ����


    cv::Mat img_dota = cv::imread("data/dota2.jpg");//��ʱû����

    cv::Mat img = cv::imread("data/corner.png");
    cv::Mat img_rotated = cv::imread("data/corner_rotated.png");

    double resize_ratio = 1;//���ű�������������֤����ƥ��Ч��
    cv::resize(img, img, cv::Size(img.cols*resize_ratio, img.rows*resize_ratio));
    cv::resize(img_rotated, img_rotated, cv::Size(img_rotated.cols*resize_ratio, img_rotated.rows*resize_ratio));


    bool use_orb=false;
    cv::Ptr<cv::Feature2D> detector_;
    if(use_orb)
        detector_ = cv::ORB::create(50);//cv::Ptr<cv::ORB> 
    else
        detector_ = cv::xfeatures2d::SIFT::create(0, 3, 0.04, 20);//sift�Աȣ���ת�����Ե�֧��//cv::Ptr<cv::Feature2D> 
    std::vector<cv::KeyPoint> Keypoints1, Keypoints2,KeyPoints_dota;
    cv::Mat descriptors1, descriptors2, desc_dota;
    //orb->detectAndCompute(img_dota, cv::noArray(), KeyPoints_dota, desc_dota);
    detector_->detectAndCompute(img, cv::Mat(), Keypoints1, descriptors1);
    detector_->detectAndCompute(img_rotated, cv::Mat(), Keypoints2, descriptors2);

#if 0 //��ͼչʾ
    cv::Mat ShowKeypoints1, ShowKeypoints2;
    cv::drawKeypoints(img, Keypoints1, ShowKeypoints1);
    cv::imshow("Keypoints", ShowKeypoints1);
    cv::waitKey(2000);
#endif
    
    //todo��Ϊ�˸��õ�ƥ�䣬�滻ƥ�䷽��
    //Matching
    std::vector<cv::DMatch> matches;
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");//BruteForce//BruteForce-Hamming
    matcher->match(descriptors1, descriptors2, matches);
    std::cout << "find out total " << matches.size() << " matches" << std::endl;

    //���ӻ�
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


    //��Ҫnonfree��֧��
    //cv::Ptr<cv::FeatureDetector> fast = cv::FeatureDetector::create("FAST");




    return 0;
}