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
//��һ������������ȡ����׼
void featureExtract_surf();
void featureExtract_surf_knn();
void featureExtract_sift();
void featureExtract_sift_knn();

//��߻���orb��FAST��Harris


//�ڶ�����ƴ����
typedef struct  
{
	Point2f left_top;
	Point2f left_bottom;
	Point2f right_top;
	Point2f right_bottom;
}four_corners_t;
four_corners_t CalcCorners(const Mat& H, const Mat& src);


//��ͼԭͼ����ͼtrans�������ҹ�ϵһ���𣿰�����ͼ��һ���أ�����Ŀ�꣬��ͼtrans���Ľǡ���֮��tran��ɸ����꣬��֮��img1�����У�trans������Ǹ�����֮������ִ�У�����˳�򲻿ɱ�
void OptimizeSeam(Mat &img1, Mat& trans, Mat& dst, four_corners_t corners);

//�±��⼸���������÷�Χ�Ƚ�խ������������ң�ӳ����ֱ��������Ȼ����֮��Ķ��Բ��ϣ��и���
//ʵ��ͼ��϶���Ťת�ģ�Ҳ��ʶ�����ģ��ⶼ������
void surfBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
void siftBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
//orb��brief�ĸĽ��棬����orb��Ȼû�н���߶Ȳ����ԣ�Ҳ��������ԣ����˻�Ӧ���ǵȸߵ�
//ʵ����������ͼ��scale��һ����ȣ�ֻ��˵��࣬���һ�����S���ظ��Ĳ��֣�
void orbBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);
//void fastBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);

//Ҫ���ϱߵķ���ʵ���޸�һ�治�����������ҵģ���ʵImageStitch���ǲ����Ƶġ�
//������������֮���ͼ����ѡһ�����ϸ�����˳��ģ����û���ġ�
//������ǰ�ķ�������֪���ǲ�����ȫ�壬���������Ǿֲ�ƴ�Ӷ��ѣ����Ҹо������ú���Ǻ�GPSƴ�ӵġ�
//�����ò������ǵķ����������룬���Կ��ǽ�ϸ�����Ϣ�Ż������磬�Ƿ�Ҫƴ����Сͼ���ٺϽ���ͼ��
//Ҳ��һ���ͱ�ÿ�κϽ�ȥһ�ſ죿�ڴ棿�ܵ���˵��һ�����Ӱɣ��ڴ������������ô��Ҳ��������



//opencv�Դ�stitchĬ������surf��������
//opencv�Դ�stitch��ͬ��Ŀǰ����ĵ�����ӳ�䣬��˫���ҡ�
//opencv��stitchӦ�ú���
int opencvStitch();
//�Լ�����д��ѭ��ȥ���û�������
int opencvStitch2();
//����stitch�ĸĽ��汾���Ա�һ��ͬ�������£����ַ�����ȡ��ƥ��������
void siftBased_extraction_match_advanced(char* src_path_right, char* src_path_left, char* dst_path, int accuracy);





void surfBased_extraction_match_new(char* src_path_main, char* src_path_new, char* dst_path, int accuracy);

void surfBased_extraction_match_new_resize(char* src_path_main, char* src_path_new, char* dst_path, int accuracy);




//�����ںϵ����⣬����Ҫ���ǣ�������ͼЧ�����Ƿ�Ҫ�ںϣ��ںϺ���ᵼ��ͼƬģ��










// template <template <typename> typename T1, typename T2>
// struct Fun1_ {
// 	using type = typename T1<T2>::type;
// };
// void template_test()



#endif