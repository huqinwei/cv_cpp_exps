/*

*/

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>
#include "blur.h"
#include "feature_keypoint_scale.h"

#define TEST_NODE_SEGMENTATION

int main()
{


	featureKeypointScaleDemo();


	featureKeypointScaleAndRotateDemo();

	cv::Mat mat1;

	gaussian_blur_test();

	return 0;

}

/*
#pragma warning(disable:4996)
//#include "stdafx.h"
// #include <opencv2\opencv.hpp>
// #include <opencv2\core\core.hpp>
// #include <opencv2\imgproc\imgproc.hpp>
// #include <opencv2\objdetect\objdetect.hpp>
// #include <opencv2\imgproc\types_c.h>
//#include <opencv2\objdetect\objdetect_c.h>

// #include <tchar.h>
// #include <opencv2\imgproc\types_c.h>
// #include <opencv2\improc.hpp>
// #include <opencv2/opencv.hpp>
#include "highgui/highgui.hpp"
//#include "opencv2/nonfree/nonfree.hpp"//SiftFeatureDetector，需要单独补充，如果还不行，using namespace cv::xfeatures2d
//#include "opencv2/legacy/legacy.hpp"

//using namespace cv::xfeatures2d;
//#include "opencv2/xfeatures2d.hpp"

// #include "hog_test.h"
#include "blog_demo2.h"
#include "blog_demo3.h"
#include "blog_demo4.h"
#include "blog_demo5.h"
#include "blog_demo6.h"
#include "blog_demo7.h"
// #include "feature_extraction_matching.h"
// #include "video_frame_extraction.h"
// #include "perspectiveTransformDemo.h"
// #include "roi_demo.h"
// #include "other_api_demo.h"
// #include "image_mosaicing.h"
#include<ctime>
#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#include "feature_detection_demos.h"
#include "opencv2/imgproc.hpp"

#include "opencv2/features2d.hpp"
//#include "feature_extraction_matching.h"//opencv3不适用
#if 1
#include <stdio.h>
#include <math.h>

#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc.hpp>

//https://blog.csdn.net/u012936940/article/details/79512867
/*
int flann_demo2() {


    cv::Mat img1 = cv::imread("data/1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat img2 = cv::imread("data/2.jpg", CV_LOAD_IMAGE_GRAYSCALE);


    // SURF 特征检测与匹配
    int minHessian = 1500;
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
    cv::Ptr<cv::DescriptorExtractor> descriptor = cv::xfeatures2d::SURF::create();
    cv::Ptr<cv::DescriptorMatcher> matcher1 = cv::DescriptorMatcher::create("FlannBased");////BruteForce，这个博客的例子原本是bruteforce，因为我测试flann，所以换了
    //    BFMatcher matcher1(NORM_L2);

    std::vector<cv::KeyPoint> keyPoint1, keyPoint2;
    cv::Mat descriptors1, descriptors2;
    std::vector<cv::DMatch> matches;

    // 检测特征点
    detector->detect(img1, keyPoint1);//暂时跑不通，提取失败
    detector->detect(img2, keyPoint2);
    // 提取特征点描述子
    descriptor->compute(img1, keyPoint1, descriptors1);
    descriptor->compute(img2, keyPoint2, descriptors2);
    // 匹配图像中的描述子
    matcher1->match(descriptors1, descriptors2, matches);

    cv::Mat img_keyPoint1, img_keyPoint2;
    drawKeypoints(img1, keyPoint1, img_keyPoint1, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
    cv::imshow("keyPoint1 SURF", img_keyPoint1);
    cv::drawKeypoints(img2, keyPoint2, img_keyPoint2, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
    cv::imshow("keyPoint2 SURF", img_keyPoint2);

    cv::Mat img_matches;
    cv::drawMatches(img1, keyPoint1, img2, keyPoint2, matches, img_matches);
    cv::imshow("img_matches", img_matches);

    cout << "keyPoint1.size = " << keyPoint1.size() << endl;
    cout << "keyPoint2.size = " << keyPoint2.size() << endl;
    cout << "descriptors1.size = " << descriptors1.size() << endl;
    cout << "descriptors1.size = " << descriptors2.size() << endl;
    cout << "matches.size = " << matches.size() << endl;
    //    for (int i = 0; i < matches.size(); i++)
    //        cout << matches[i].distance << ' ';
    //    cout << endl;
    cv::waitKey(0);

    return 0;
}
*/

//https://blog.csdn.net/App_12062011/article/details/51987352
//opencv2的命令？需要适配到opencv3
/*
int flann_demo()
{


    cv::Mat img_1 = cv::imread("data/1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat img_2 = cv::imread("data/2.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    if (!img_1.data || !img_2.data)
    {
        std::cout << " --(!) Error reading images " << std::endl; return -1;
    }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    cv::xfeatures2d::SurfFeatureDetector detector(minHessian);

    std::vector<cv::KeyPoint> keypoints_1, keypoints_2;

    detector.detect(img_1, keypoints_1);
    detector.detect(img_2, keypoints_2);

    //-- Step 2: Calculate descriptors (feature vectors)
    cv::xfeatures2d::SurfDescriptorExtractor extractor;

    cv::Mat descriptors_1, descriptors_2;

    extractor.compute(img_1, keypoints_1, descriptors_1);
    extractor.compute(img_2, keypoints_2, descriptors_2);

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match(descriptors_1, descriptors_2, matches);

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
    //-- PS.- radiusMatch can also be used here.
    std::vector< cv::DMatch > good_matches;

    for (int i = 0; i < descriptors_1.rows; i++)
    {
        if (matches[i].distance < 2 * min_dist)
        {
            good_matches.push_back(matches[i]);
        }
    }

    //-- Draw only "good" matches
    cv::Mat img_matches;
    cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2,
        good_matches, img_matches, cv::Scalar::all(-1), Scalar::all(-1),
        vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    //-- Show detected matches
    cv::imshow("Good Matches", img_matches);

    for (int i = 0; i < good_matches.size(); i++)
    {
        printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
    }

    cv::waitKey(0);




}

#include "feature_matching.h"
int main()
{
    //flann_demo2();
    return orb__sift_feature_matching_with_rotation();
    return fast_feature_detection();


    double result;
    double x = 4.0;
    result = exp(x);
    printf("'e'raisedtothepower\of%lf(e^%lf)=%lf\n", x, x, result);
    return 0;




    cv::Mat im1 = imread("F:/google_data_analyse/data2/314050417361734340_256_256_0.jpg", cv::IMREAD_UNCHANGED);//IMREAD_UNCHANGED

    cv::resize(im1, im1, cv::Size(im1.cols * 2, im1.rows * 2));

    imwrite("F:/google_data_analyse/data2/314050417361734340_512_512_0.jpg", im1);


    canny_detection();

    //FAST()

    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 	fastBased_DrawMatches_with_rotate();
    // 	surfBased_DrawMatches();
    //draw_matches();
    //draw_keypoints();
    //file_demo();
    //hog_demo();

    //blog_demo2_create_mat();
    blog_demo2_threshold_binary();
    blog_demo2_erode();
    blog_demo2_blur();
    blog_demo2_canny();
    blog_demo2_trackbar();
    blog_demo2_data_sctructure();
    blog_demo2_access_to_pixel();
    blog_demo2_equalize_hist();
    //filters_demo();
    //blog_demo4_dilate();
    //blog_demo4_erode();
    //blog_demo4_morphologyEx();
    //blog_demo5_resize_specifiedsize();
    //blog_demo5_resize_specifiedratio();
    //blog_demo5_pyramid();

    //blog_demo6_canny();
    //blog_demo6_sobel();
    //*blog_demo6_laplacian();
    //blog_demo7_HoughLines();
    //*blog_demo7_HoughLinesP();
    //blog_demo7_HoughCircles();

    //*featureExtract_surf_knn();
    //featureExtract_sift();
    //featureExtract_sift_knn();
    //system("pause");
    //waitKey(100000);

    // 	cout << float(4) / 100 << endl;
    // 	cout << 4 / 100 << endl;



    //perspectiveTransformDemo1();
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    //D:\VS2015\Projects\pictures\dataset6_opencv _reverse
    char* src_right = "data/Center\\jpg_small\\4.jpg";
    char* src_left = "data/Center\\jpg_small\\5.jpg";
    char* dst = "data/Center\\jpg_small\\sift_4_5_3800_advance.jpg";
    // 	siftBased_extraction_match_advanced(src_right, src_left, dst, 3800);
    //surfBased_extraction_match_new(src_right, src_left, dst, 1800);
    //surfBased_extraction_match_new_resize(src_right, src_left, dst, 1800);
    //siftBased_extraction_match(src_right, src_left, dst, 3800);
    //siftBased_extraction_match(src_right, src_left, dst, 800);
    //surfBased_extraction_match(src_right, src_left, dst, 1300);
    //siftBased_extraction_match(src_right, src_left, dst, 800);
    //orbBased_extraction_match(src_right, src_left, dst, 3000);

    //roi_demo1();

    //opencvStitch();
    //opencvStitch2();

    system("pause");
    return 0;
}

#else

#endif


*/
