#pragma once

#if 0
#include "feature_extraction_matching.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/stitching/stitcher.hpp>



#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/stitching/warpers.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/camera.hpp"

using namespace std;
using namespace cv;
//第一步：特征的提取和配准
void featureExtract_surf();
void featureExtract_surf_knn();
void featureExtract_sift();
void featureExtract_sift_knn();

//后边还有orb、FAST、Harris


//第二步：拼起来
typedef struct  
{
	Point2f left_top;
	Point2f left_bottom;
	Point2f right_top;
	Point2f right_bottom;
}four_corners_t;
four_corners_t CalcCorners(const Mat& H, const Mat& src);


//左图原图，右图trans，（左右关系一定吗？把两个图换一下呢？），目标，右图trans的四角。反之，tran变成负坐标，总之，img1是正中，trans是歪的那个，反之好像不能执行，左右顺序不可变
void OptimizeSeam(Mat &img1, Mat& trans, Mat& dst, four_corners_t corners);

//下边这几个方案适用范围比较窄，必须分清左右，映射完直接贴，不然坐标之类的都对不上，有负数
//实际图像肯定有扭转的，也有识别错误的，这都不可能
void surfBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
void siftBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
//orb是brief的改进版，但是orb仍然没有解决尺度不变性，也许可以试试，无人机应该是等高的
//实际跑下来，图的scale不一定恒等，只是说差不多，而且还有走S形重复的部分，
void orbBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
//void fastBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);

//要把上边的方案实际修改一版不限制上下左右的，其实ImageStitch就是不限制的。
//或者先用内蒙之类的图，精选一下有严格左右顺序的，别用怀柔的。
//他们以前的方案，不知道是不是完全体，看起来就是局部拼接而已，而且感觉是利用航向角和GPS拼接的。
//不管用不用他们的方案，最起码，可以考虑结合各种信息优化，比如，是否要拼几张小图，再合进大图？
//也不一定就比每次合进去一张快？内存？总的来说就一个机子吧，内存如果不够，怎么分也不够啊。



//opencv自带stitch默认优先surf。。。。
//opencv自带stitch不同于目前代码的单向找映射，是双向找。
//opencv的stitch应该很慢
int opencvStitch();
//自己主动写个循环去调用会怎样？
int opencvStitch2();
//仿照stitch的改进版本？对比一下同样条件下，两种方法获取的匹配点的数量
void siftBased_extraction_match_advanced(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);





void surfBased_extraction_match_new(char* src_path_main, char* src_path_new, char* dst_path, int accuracy);

void surfBased_extraction_match_new_resize(char* src_path_main, char* src_path_new, char* dst_path, int accuracy);




//关于融合的问题，还需要考虑，看看多图效果，是否还要融合，融合好像会导致图片模糊










// template <template <typename> typename T1, typename T2>
// struct Fun1_ {
// 	using type = typename T1<T2>::type;
// };
// void template_test()



#endif