#include "feature_keypoint_scale.h"
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
//todo:orb
void featureKeypointScaleDemo() {
    const int use_inlier_mask = 1;

	cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create(2048);
    char im1_path[512];
    char im2_path[512];
    if (1)
    {
        sprintf(im1_path, "D:/alk_workspace/exp_code_collection_root_path/data/dota0.jpg");
        sprintf(im2_path, "D:/alk_workspace/exp_code_collection_root_path/data/dota0_axe.jpg");
    }
    else
    {
        sprintf(im1_path, "D:/alk_workspace/exp_code_collection_root_path/data/dota0_axe.jpg");
        sprintf(im2_path, "D:/alk_workspace/exp_code_collection_root_path/data/dota0_axe_rotated.jpg");
    }
    cv::Mat im1, im2, im1_add_axe;

    im1 = imread(im1_path, cv::IMREAD_COLOR);
    im2 = imread(im2_path, cv::IMREAD_COLOR);
    im1_add_axe = imread(im1_path, cv::IMREAD_COLOR);//读取im1作为基底，把斧王复制过来

    cv::resize(im2, im2, cv::Size(im2.cols / 2, im2.rows / 2));
    if (1)
        im2.copyTo(im1_add_axe(Rect(0, 0, im2.cols, im2.rows)));
    else
        im2.copyTo(im1_add_axe(Rect(im1.cols - im2.cols, im1.rows - im2.rows, im2.cols, im2.rows)));

//     cv::imshow("im1_add_axe", im1_add_axe);
//     cv::waitKey();

    vector<cv::KeyPoint> key_points1, key_points2, key_points1_filtered, key_points2_filtered;
    sift->detect(im1, key_points1);
    sift->detect(im1_add_axe, key_points2);

    cv::Mat descriptors1, descriptors2;
    sift->compute(im1, key_points1, descriptors1);
    sift->compute(im1_add_axe, key_points2, descriptors2);
    vector<cv::DMatch> matchs;
    vector<cv::DMatch> matchs_filtered;
    cv::BFMatcher  bf_matcher(cv::NORM_L2, true);
    bf_matcher.match(descriptors1, descriptors2, matchs);

    vector<cv::Point2f> points1, points2;

    int cnt = 0;
    for (const auto& match : matchs) {
        if (cnt++ >= 120)
            break;
        matchs_filtered.push_back(match);
        key_points1_filtered.push_back(key_points1[match.queryIdx]);
        key_points2_filtered.push_back(key_points2[match.trainIdx]);
    }

    cv::Mat good_match;
    cv::drawMatches(im1, key_points1, im1_add_axe, key_points2, matchs_filtered, good_match);
    cv::imshow("good_match", good_match);
    cv::waitKey();


}

void featureKeypointScaleAndRotateDemo() {
    const int use_inlier_mask = 1;

    cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create(2048);//2048
    char im1_path[512];
    char im2_path[512];
    char im1_rotated_path[512];
    //D:\experiment_data\pictures_other
    if (1)
    {
        sprintf(im1_path, "D:\\experiment_data\\pictures_other\\dota0.jpg");
        sprintf(im2_path, "D:\\experiment_data\\pictures_other\\dota0_axe.jpg");
        sprintf(im1_rotated_path, "D:\\experiment_data\\pictures_other\\dota0_rotated.jpg");
    }
    else
    {
        sprintf(im1_path, "D:\\experiment_data\\pictures_other\\dota0_axe.jpg");
        sprintf(im2_path, "D:\\experiment_data\\pictures_other\\dota0_axe_rotated.jpg");
    }
    cv::Mat im1, im2, im1_rotated_path_add_axe;

    im1 = imread(im1_path, cv::IMREAD_COLOR);
    im2 = imread(im2_path, cv::IMREAD_COLOR);
    im1_rotated_path_add_axe = imread(im1_rotated_path, cv::IMREAD_COLOR);

    cv::resize(im2, im2, cv::Size(im2.cols / 2, im2.rows / 2));
    if (0)
        im2.copyTo(im1_rotated_path_add_axe(Rect(0, 0, im2.cols, im2.rows)));
    else
        im2.copyTo(im1_rotated_path_add_axe(Rect(im1_rotated_path_add_axe.cols - im2.cols, im1_rotated_path_add_axe.rows - im2.rows, im2.cols, im2.rows)));

    //     cv::imshow("im1_add_axe", im1_add_axe);
    //     cv::waitKey();



    vector<cv::KeyPoint> key_points1, key_points2, key_points1_filtered, key_points2_filtered;
    sift->detect(im1, key_points1);
    sift->detect(im1_rotated_path_add_axe, key_points2);

    cv::Mat descriptors1, descriptors2;
    sift->compute(im1, key_points1, descriptors1);
    sift->compute(im1_rotated_path_add_axe, key_points2, descriptors2);
    vector<cv::DMatch> matchs;
    vector<cv::DMatch> matchs_filtered;
    cv::BFMatcher  bf_matcher(cv::NORM_L2, true);
    bf_matcher.match(descriptors1, descriptors2, matchs);

    vector<cv::Point2f> points1, points2;
    for (const auto& match : matchs) {
        points1.push_back(key_points1[match.queryIdx].pt);
        points2.push_back(key_points2[match.trainIdx].pt);
    }
    vector<uchar> inlier_mask;
    Mat H = cv::findHomography(points1, points2, RANSAC, 106, inlier_mask);
    std::cout << "H: " << H << std::endl;

    for (int i = 0; i < matchs.size(); ++i) {
        if (!inlier_mask[i]) {
            matchs.erase(matchs.begin() + i);
            inlier_mask.erase(inlier_mask.begin() + i);
            --i;
        }
    }

    int cnt = 0;
    for (const auto& match : matchs) {
        if (cnt++ >= 120)
            break;
        matchs_filtered.push_back(match);
        key_points1_filtered.push_back(key_points1[match.queryIdx]);
        key_points2_filtered.push_back(key_points2[match.trainIdx]);
    }





    cv::Mat good_match;
    cv::drawMatches(im1, key_points1, im1_rotated_path_add_axe, key_points2, matchs_filtered, good_match);
    cv::imshow("good_match", good_match);
    cv::waitKey();


}
