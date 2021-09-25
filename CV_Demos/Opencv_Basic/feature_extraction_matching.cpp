#include "feature_extraction_matching.h"

#if 0

// 除了能完成基本功能的demo，还有很多问题会遇到。
// 比如数据对接，比如线程的包裹。
// 比如gpu和cuda的加速。
// 比如我要怎么组织目录，让一堆图片在一个目录？然后呢？两两对比，逐渐融合？然后还要伴随着删除。
// 前提还是融合真的符合预期，我现在还没概念，这种依赖特征的融合，是两图缩放一下就拼接了，还是完全扭曲的离散的拼接？
// 如果是后者，还要处理后续各种问题。


void featureExtract_surf()
{

	bool debug_display = false;
// 	Mat image01 = imread("data/huairou_center\\HRA5C00480.JPG", 1);//D:\VS2015\Projects
// 	Mat image02 = imread("data/huairou_center\\HRA5C00481.JPG", 1);
	Mat image01 = imread("data/1.JPG", 1);//D:\VS2015\Projects
	Mat image02 = imread("data/2.JPG", 1);
	//
	namedWindow("p2", 0);
	namedWindow("p1", 0);
	imshow("p2", image01);
	imshow("p1", image02);
	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	if (debug_display)
	{
		namedWindow("p2_cvt", 0);
		namedWindow("p1_cvt", 0);
		imshow("p2_cvt", image2);//其实就是灰度图
		imshow("p1_cvt", image1);

	}

	SiftFeatureDetector siftDetector(800);
	SurfFeatureDetector surfDetector(800);//好像是数值越小点越多？//Ptr<SURF> detector = SURF::create( )；
	vector<KeyPoint> keyPoint1, keyPoint2;
	Mat imageDesc1, imageDesc2;
	SurfDescriptorExtractor surfDescriptor;//特征点描述
	SiftDescriptorExtractor siftDescriptor;//特征点描述

	if (0)//暂时不提
	{
		siftDetector.detect(image1, keyPoint1);
		siftDetector.detect(image2, keyPoint2);
		siftDescriptor.compute(image1, keyPoint1, imageDesc1);
		siftDescriptor.compute(image2, keyPoint2, imageDesc2);
	}
	else//实测，surf还是比较差的
	{
		surfDetector.detect(image1, keyPoint1);
		surfDetector.detect(image2, keyPoint2);
		surfDescriptor.compute(image1, keyPoint1, imageDesc1);
		surfDescriptor.compute(image2, keyPoint2, imageDesc2);
		
	}


	FlannBasedMatcher matcher;
	vector<DMatch> matchPoints;
	matcher.match(imageDesc1, imageDesc2, matchPoints, Mat());
	cout << "total match points: " << matchPoints.size() << endl;
	Mat img_match;
	drawMatches(image01, keyPoint1, image02, keyPoint2, matchPoints, img_match);
	namedWindow("match", 0);
	imshow("match", img_match);
	imwrite("data/match.jpg", img_match);

	waitKey(70000);
	destroyAllWindows();


	


}


void featureExtract_surf_knn()
{
	Mat image01 = imread("data/huairou_center\\HRA5C00480.JPG", 1);//D:\VS2015\Projects
	Mat image02 = imread("data/huairou_center\\HRA5C00481.JPG", 1);
// 	Mat image01 = imread("data/1.JPG", 1);
// 	Mat image02 = imread("data/2.JPG", 1);
	imshow("p2", image01);
	imshow("p1", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);
	SurfFeatureDetector surfDetector(2000);//hessian矩阵阈值，越大，点少，精准
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector.detect(image1, keyPoint1);
	surfDetector.detect(image2, keyPoint2);

	SurfDescriptorExtractor surfDescriptor;//特征点描述
	Mat imageDesc1, imageDesc2;
	surfDescriptor.compute(image1, keyPoint1, imageDesc1);
	surfDescriptor.compute(image2, keyPoint2, imageDesc2);

	//下边就和之前不同了，之前直接用两个description去match，现在还有训练
	FlannBasedMatcher matcher;
	vector<vector<DMatch>>matchePoints;//双重数组，应该是每一个点，和这个点找到的N个匹配点。
	vector<DMatch> GoodMatchePoints;
	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	cout << "total match points: " << matchePoints.size() << endl;
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.6 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}
	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first match", first_match);




	waitKey();
}


void featureExtract_sift()
{
	Mat image01 = imread("data/1.JPG", 1);//D:\VS2015\Projects
	Mat image02 = imread("data/2.JPG", 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
	imshow("p2", image01);
	imshow("p1", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);
	SiftFeatureDetector siftDetector(2000);//hessian矩阵阈值，越大，点少，精准
	vector<KeyPoint> keyPoint1, keyPoint2;
	siftDetector.detect(image1, keyPoint1);
	siftDetector.detect(image2, keyPoint2);

	SiftDescriptorExtractor siftDescriptor;//特征点描述
	Mat imageDesc1, imageDesc2;
	siftDescriptor.compute(image1, keyPoint1, imageDesc1);
	siftDescriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
	cout << "total match points: " << matchePoints.size() << endl;

	Mat img_match;
	drawMatches(image01, keyPoint1, image02, keyPoint2, matchePoints, img_match);//两个图，两个点集，一个关系集合，画起来一个大图？
	imshow("match", img_match);
	imwrite("data/sift_match.jpg", img_match);
	waitKey();



}


void featureExtract_sift_knn()
{
	Mat image01 = imread("data/11.JPG", 1);//D:\VS2015\Projects
	Mat image02 = imread("data/12.JPG", 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
	imshow("p2", image01);
	imshow("p1", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);
	//"hessian矩阵阈值，越大，点少，精准"，但是从结果看，我感觉这是生成的配对对数啊？（或者这一步是features数量（keypoint产出），总之，感觉和他说的相反）
	SiftFeatureDetector surfDetector(1800);
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector.detect(image1, keyPoint1);
	surfDetector.detect(image2, keyPoint2);

	SiftDescriptorExtractor surfDescriptor;//特征点描述
	Mat imageDesc1, imageDesc2;
	surfDescriptor.compute(image1, keyPoint1, imageDesc1);
	surfDescriptor.compute(image2, keyPoint2, imageDesc2);

	//下边就和之前不同了，之前直接用两个description去match，现在还有训练
	FlannBasedMatcher matcher;
	vector<vector<DMatch>>matchePoints;//双重数组，应该是每一个点，和这个点找到的N个匹配点。
	vector<DMatch> GoodMatchePoints;
	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);//knn的train，不就是放入第一个数据集么。。。。
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;
	for (int i = 0; i < matchePoints.size(); i++)
	{
		//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		//这里只有index，是没有坐标之类的信息的，直接就是distance
		//哪来的距离？应该就是knn时候算出来的。
		//matchePoints不一定是一一对应，一个queryIdx可能对应多（k）个trainIdx。也就是一个i下有多个配对,[0]和[1]是不同的配对信息，distance已经排序了
		if (matchePoints[i][0].distance < 0.6 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}
	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first match", first_match);

	imwrite("data/sift_match_knn.jpg", first_match);



	waitKey();
}


void OptimizeSeam(Mat &img1, Mat& trans, Mat& dst,four_corners_t corners)
{
	int start = MIN(corners.left_top.x, corners.left_bottom.x);//重叠区域的左边界，取了一个小的(肯定啊，要处理每一个细节，如果是斜边，当然不能以右边为准)
	double processWidth = img1.cols - start;//重叠区域宽度，img1在左？img1.cols是重叠区右边界？
	int rows = dst.rows;
	int cols = img1.cols;//列数*通道数？
	double alpha = 1;//img1的像素的权重
	for (int i = 0; i < rows; i++)
	{
		uchar* p = img1.ptr<uchar>(i);
		uchar* t = trans.ptr<uchar>(i);//trans是谁的trans？
		uchar* d = dst.ptr<uchar>(i);
		for (int j = start; j < cols; j++)//遍历所有重叠的列么
		{
			if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)//无像素，黑点，这是三通道交替铺的？
			{
				alpha = 1;//完全用img1
			}
			else//有像素，计算权重
			{//“img1中像素的权重，与当前处理点距重叠区域左边界的距离成正比，实验证明，这种方法确实好”
				//当前点是j，j-start是距离左边界，宽度是重叠区宽度，被整体宽度减完，不是和右边的距离了么，可能他说错了，直觉是越近权重越大，而不是距离成正比！！！
				//这个也有一个缺点，他是整列去调整的吧，parallax-tolerant image stitching是local的。
				alpha = (processWidth - (j - start)) / processWidth;//随着j的向右移动，alpha逐渐变小，也就是tran提供的更多了
			}
			//根据权重，用p和t拼d。
			d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
			d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);

		}
	}
	

}

four_corners_t CalcCorners(const Mat& H, const Mat& src)//变换矩阵与原图
{
	four_corners_t corners;
	double v2[] = { 0, 0, 1 };//左上角
	double v1[3];//变换后的坐标值
	Mat V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	Mat V1 = Mat(3, 1, CV_64FC1, v1);  //列向量

	V1 = H * V2;
	//左上角(0,0,1)
	cout << "V2: " << V2 << endl;
	cout << "V1: " << V1 << endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//左下角(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//右上角(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//右下角(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //列向量
	V1 = Mat(3, 1, CV_64FC1, v1);  //列向量
	V1 = H * V2;
	corners.right_bottom.x = v1[0] / v1[2];
	corners.right_bottom.y = v1[1] / v1[2];
	return corners;
}

void surfBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy)
{
	Mat image01 = imread(src_path_right, 1);//D:\VS2015\Projects
	Mat image02 = imread(src_path_left, 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
	imshow("p1", image01);
	imshow("p2", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	//提取特征点    
	SurfFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备    
	SurfDescriptorExtractor Descriptor;
	Mat imageDesc1, imageDesc2;
	Descriptor.compute(image1, keyPoint1, imageDesc1);
	Descriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);





	//转换数据类型，用于配准
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners= CalcCorners(homo,image01);//这是图像1变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2
	imshow("直接经过透视矩阵变换", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//图像拷贝，前边是变换得到imageTransform，现在要拼上去了。
	int dst_width = imageTransform1.cols;//“最右点的长度作为拼接图的长度”（这是图一扭曲后的列数，等于最右边的坐标？）
	int dst_height = image02.rows;//图2的行数作为高度(其实imageTransform1也一样，imageTransform1是图1的变换，和图2一样的rows，warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多

	imshow("b_dst", dst);

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	OptimizeSeam(image02, imageTransform1, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);





	waitKey();

}

void siftBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy)
{
	Mat image01 = imread(src_path_right, IMREAD_UNCHANGED);//D:\VS2015\Projects
	Mat image02 = imread(src_path_left, 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
// 	imshow("p1", image01);
// 	imshow("p2", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	//提取特征点    
	SiftFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备    
	SiftDescriptorExtractor Descriptor;
	Mat imageDesc1, imageDesc2;
	Descriptor.compute(image1, keyPoint1, imageDesc1);
	Descriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);





	//转换数据类型，用于配准
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners = CalcCorners(homo, image01);//这是图像1变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2
	imshow("直接经过透视矩阵变换", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//图像拷贝，前边是变换得到imageTransform，现在要拼上去了。
	int dst_width = imageTransform1.cols;//“最右点的长度作为拼接图的长度”（这是图一扭曲后的列数，等于最右边的坐标？）
	int dst_height = image02.rows;//图2的行数作为高度(其实imageTransform1也一样，imageTransform1是图1的变换，和图2一样的rows，warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多

	imshow("b_dst", dst);
// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	OptimizeSeam(image02, imageTransform1, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);





	//waitKey();

}


void orbBased_extraction_match(char* src_path_right, char* src_path_left, char* dst_path, int accuracy)
{
	Mat image01 = imread(src_path_right, 1);//D:\VS2015\Projects
	Mat image02 = imread(src_path_left, 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
	imshow("p1", image01);
	imshow("p2", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	//提取特征点    
	OrbFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备    
	OrbDescriptorExtractor Descriptor;
	Mat imageDesc1, imageDesc2;
	Descriptor.compute(image1, keyPoint1, imageDesc1);
	Descriptor.compute(image2, keyPoint2, imageDesc2);
	
	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

// 	vector<Mat> train_desc(1, imageDesc1);
// 	matcher.add(train_desc);
// 	matcher.train();
// 	matcher.knnMatch(imageDesc2, matchePoints, 3);
// 	cout << "total match points: " << matchePoints.size() << endl;


	flann::Index flannIndex(imageDesc1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
	Mat macthIndex(imageDesc2.rows, 2, CV_32SC1), matchDistance(imageDesc2.rows, 2, CV_32FC1);
	flannIndex.knnSearch(imageDesc2, macthIndex, matchDistance, 2, flann::SearchParams());



	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchDistance.rows; i++)
	{
		if (matchDistance.at<float>(i, 0) < 0.4 * matchDistance.at<float>(i, 1))
		{
			DMatch dmatches(i, macthIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
			GoodMatchePoints.push_back(dmatches);
		}
	}

	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);


	//转换数据类型，用于配准
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners = CalcCorners(homo, image01);//这是图像1变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2
	imshow("直接经过透视矩阵变换", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//图像拷贝，前边是变换得到imageTransform，现在要拼上去了。
	int dst_width = imageTransform1.cols;//“最右点的长度作为拼接图的长度”（这是图一扭曲后的列数，等于最右边的坐标？）
	int dst_height = image02.rows;//图2的行数作为高度(其实imageTransform1也一样，imageTransform1是图1的变换，和图2一样的rows，warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多

	imshow("b_dst", dst);

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	OptimizeSeam(image02, imageTransform1, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);





	waitKey();

}

int opencvStitch()
{
	char* dst = "data/neimeng\\dst.jpg";
	bool try_use_gpu = false;
	vector<Mat> imgs;
// 	for (int i = 1; i <= 60; i++)
// 	{
// 		char path[200] = { 0 };
// 		if(i<10)
// 			sprintf(path,"data/neimeng\\NMW209XS000%d.jpg",i);
// 		else
// 			sprintf(path, "data/neimeng\\NMW209XS00%d.jpg", i);
// 		Mat image1 = imread(path, IMREAD_UNCHANGED);
// 		imgs.push_back(image1);
// 	}


	dst = "data/frames\\dst.jpg";
	for (int i = 10; i <= 16; i++)
	{
		char path[200] = { 0 };
		sprintf(path, "data/frames\\image%d.jpg", i);
		Mat image1 = imread(path, IMREAD_UNCHANGED);
		imgs.push_back(image1);
	}

// 	Mat image1 = imread("data/neimeng\\NMW209XS0001.jpg", IMREAD_UNCHANGED);
// 	Mat image2 = imread("data/neimeng\\NMW209XS0002.jpg", IMREAD_UNCHANGED);
// 	Mat image3 = imread("data/neimeng\\NMW209XS0003.jpg", IMREAD_UNCHANGED);
// 	Mat image4 = imread("data/neimeng\\NMW209XS0004.jpg", IMREAD_UNCHANGED);
// 	Mat image5 = imread("data/neimeng\\NMW209XS0005.jpg", IMREAD_UNCHANGED);
// 	Mat image6 = imread("data/neimeng\\NMW209XS0006.jpg", IMREAD_UNCHANGED);
// 	Mat image7 = imread("data/neimeng\\NMW209XS0007.jpg", IMREAD_UNCHANGED);
// 	Mat image8 = imread("data/neimeng\\NMW209XS0008.jpg", IMREAD_UNCHANGED);
// 	Mat image9 = imread("data/neimeng\\NMW209XS0009.jpg", IMREAD_UNCHANGED);
// 	Mat image10 = imread("data/neimeng\\NMW209XS0010.jpg", IMREAD_UNCHANGED);
// 	Mat image11 = imread("data/neimeng\\NMW209XS0011.jpg", IMREAD_UNCHANGED);
// 	Mat image12 = imread("data/neimeng\\NMW209XS0012.jpg", IMREAD_UNCHANGED);
// 	Mat image13 = imread("data/neimeng\\NMW209XS0013.jpg", IMREAD_UNCHANGED);
// 	Mat image14 = imread("data/neimeng\\NMW209XS0014.jpg", IMREAD_UNCHANGED);
// 	Mat image15 = imread("data/neimeng\\NMW209XS0015.jpg", IMREAD_UNCHANGED);

// 	if (image01.empty() || image02.empty() || image03.empty() || image04.empty() || image05.empty())
// 	{
// 		cout << "Can't read image" << endl;
// 		return -1;
// 	}

// 	imgs.push_back(image1);
// 	imgs.push_back(image2);
// 	imgs.push_back(image3);
// 	imgs.push_back(image4);
// 	imgs.push_back(image5);
// 	imgs.push_back(image6);
// 	imgs.push_back(image7);
// 	imgs.push_back(image8);
// 	imgs.push_back(image9);
// 	imgs.push_back(image10);
// 	imgs.push_back(image11);
// 	imgs.push_back(image12);
// 	imgs.push_back(image13);
// 	imgs.push_back(image14);
// 	imgs.push_back(image15);

	for (vector<Mat>::iterator it = imgs.begin(); it != imgs.end(); ++it)
	{
		//it->resize(1500, 1000);
		cv::resize(*it, *it, cv::Size(1500, 1000));
	}

	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);


	Mat pano;
	Stitcher::Status status = stitcher.stitch(imgs, pano);
	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return -1;
	}
	imwrite(dst, pano);
	Mat pano2 = pano.clone();
	// 显示源图像，和结果图像
	imshow("全景图像", pano);
	if (waitKey() == 27)
		return 0;


}

int opencvStitch2()
{
	char* dst = "data/frames\\dst2.jpg";
	bool try_use_gpu = false;
	vector<Mat> imgs;
	for (int i = 10; i <= 26; i++)
	{
		char path[200] = { 0 };
		sprintf(path, "data/frames\\image%d.jpg", i);
		Mat image1 = imread(path, IMREAD_UNCHANGED);
		imgs.push_back(image1);
	}

	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);

	vector<Mat> temps;
	Mat temp_pano;
	int i = 0;
	for (vector<Mat>::iterator it = imgs.begin(); it != imgs.end(); ++it)
	{
		if (i % 3 != 2)
		{
			temps.push_back(*it);
			i++;
			continue;
		}
// 		if (it == imgs.begin())
// 		{//如果是第一张，
// 			temps.push_back(*it);
// 			continue;
// 		}
		temps.push_back(*it);
		Stitcher::Status status = stitcher.stitch(temps, temp_pano);
		if (status != Stitcher::OK)
		{
			cout << "Can't stitch images, error code = " << int(status) << endl;
			return -1;
		}
		temps.clear();
		temps.push_back(temp_pano);
		i++;
	}
	imshow("result", temp_pano);
	if (waitKey() == 27)
		return 0;
	imwrite(dst, temp_pano);

}
void siftBased_extraction_match_advanced(char* src_path_right, char* src_path_left, char* dst_path, int accuracy)
{
	Mat image01 = imread(src_path_right, IMREAD_UNCHANGED);//D:\VS2015\Projects
	Mat image02 = imread(src_path_left, 1);
	// 	Mat image01 = imread("data/1.JPG", 1);
	// 	Mat image02 = imread("data/2.JPG", 1);
// 	imshow("p1", image01);
// 	imshow("p2", image02);

	Mat image1, image2;
	cvtColor(image01, image1, CV_RGB2GRAY);
	cvtColor(image02, image2, CV_RGB2GRAY);

	//提取特征点    
	SiftFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备    
	SiftDescriptorExtractor Descriptor;
	Mat imageDesc1, imageDesc2;
	Descriptor.compute(image1, keyPoint1, imageDesc1);
	Descriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	//这里是新加的,<int,int>型的set?
	//MatchesSet matches;
	std::set<std::pair<int, int>> matches;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
			matches.insert(make_pair(matchePoints[i][0].queryIdx, matchePoints[i][0].trainIdx));
		}
	}
	cout << "\n1->2 matches: " << GoodMatchePoints.size() << endl;

	//反向的匹配
	FlannBasedMatcher matcher2;
	matchePoints.clear();//重用这个容器
	vector<Mat> train_desc2(1, imageDesc2);
	matcher2.add(train_desc2);
	matcher2.train();
	matcher2.knnMatch(imageDesc1, matchePoints, 2);
	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			//双向匹配不是筛选匹配点，是增加匹配点，是并集？注意看k-v的选择，和第一遍是反的，其实是可以重复的
			if (matches.find(make_pair(matchePoints[i][0].trainIdx,matchePoints[i][0].queryIdx)) == matches.end())
			{
				GoodMatchePoints.push_back(DMatch(matchePoints[i][0].trainIdx, matchePoints[i][0].queryIdx, matchePoints[i][0].distance));
			}

		}
	}
	cout << "1->2 & 2->1 matches: " << GoodMatchePoints.size() << endl;


	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);





	//转换数据类型，用于配准
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners = CalcCorners(homo, image01);//这是图像1变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2
	imshow("直接经过透视矩阵变换", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//图像拷贝，前边是变换得到imageTransform，现在要拼上去了。
	int dst_width = imageTransform1.cols;//“最右点的长度作为拼接图的长度”（这是图一扭曲后的列数，等于最右边的坐标？）
	int dst_height = image02.rows;//图2的行数作为高度(其实imageTransform1也一样，imageTransform1是图1的变换，和图2一样的rows，warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多

	imshow("b_dst", dst);
	//imwrite(strcat(dst_path, "before_seam_optimized"), dst);//这样写在jpg后边了

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	OptimizeSeam(image02, imageTransform1, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);

	//waitKey();
}

void surfBased_extraction_match_new(char* src_path_main, char* src_path_new, char* dst_path, int accuracy)
{
	Mat image_main = imread(src_path_main, IMREAD_UNCHANGED);//D:\VS2015\Projects
	Mat image_new = imread(src_path_new, IMREAD_UNCHANGED);

	//Mat image_main, image2;
	cvtColor(image_main, image_main, CV_RGB2GRAY);
	cvtColor(image_new, image_new, CV_RGB2GRAY);

	SurfFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint_main, keyPoint_new;
	Detector.detect(image_main, keyPoint_main);
	Detector.detect(image_new, keyPoint_new);

	SurfDescriptorExtractor Descriptor;
	Mat imageDesc_main, imageDesc_new;
	Descriptor.compute(image_main, keyPoint_main, imageDesc_main);
	Descriptor.compute(image_new, keyPoint_new, imageDesc_new);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	vector<Mat> train_desc(1, imageDesc_main);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc_new, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image_new, keyPoint_new, image_main, keyPoint_main, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);
	waitKey();

	//转换数据类型，用于配准
	vector<Point2f> imagePoints_main, imagePoints_new;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints_new.push_back(keyPoint_new[GoodMatchePoints[i].queryIdx].pt);
		imagePoints_main.push_back(keyPoint_main[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	//Mat homo = findHomography(keyPoint_new, keyPoint_main, CV_RANSAC);//这里改为，新图到主图的映射
	//Mat homo = findHomography(keyPoint_main, keyPoint_new, CV_RANSAC);//这里改为，新图到主图的映射
	Mat homo = findHomography(imagePoints_new, imagePoints_main, CV_RANSAC);//这里改为，新图到主图的映射
	////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners = CalcCorners(homo, image_new);//这是图像new变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	//改一个灵活的方案：新图的映射可能在各个方向扩展，所以宽高都不能以主图为准，且要考虑正负，
	//先各个方向都取max（也不叫max，左边叫min）但是不能真用负数，最后还要换算总宽度
	int new_left, new_right, new_top, new_bottom;//两图比较，左边取min，右边取max
	new_left = MIN(0, MIN(corners.left_top.x, corners.left_bottom.x));//主图的左边，一定是0吧？
	new_right = MAX(image_main.cols, MAX(corners.right_top.x, corners.right_bottom.x));//主图的右边是列数
	new_top = MIN(0, MIN(corners.left_top.y, corners.right_top.y));
	new_bottom = MAX(0, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom更大吧
	int new_width = new_right - new_left;
	int new_height = new_bottom - new_top;
	
	//这一步做了什么？是不是负数会有问题？他的这个size怎么算？，输入是image_new，产出是imageTransform_new，什么形式？变换后的图？
	//这一步是关键，可能这一步产生了新位置？新位置不是corners都产生了么？这一步算提前贴到一张空白上？
	//总之，新图怎么贴，决定了旧图放在dst哪，因为dst不再是原图加宽了，新图也不是一定放右边了，坐标系变了。
	//最后，按目前逻辑，不管怎么调整，首先，还是有原图的复制过程，这会不会耗时，暂时先不管。（也没有原图不复制直接网上贴东西的方案吧，又不是文本，这应该是opencv提供的一种优化可能性）

//	Mat imagePerspectiveTransform_new;
// 	perspectiveTransform(image_new, image_new, homo);
// 	imshow("perspectiveTransform", image_new);
// 	waitKey();
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2


	//这个size同样需要放大，问题是放大后，图片放在放大后的什么位置了，以原坐标系还是新坐标系为准？
	//这个接口给堵死了？他这样永远不能向左偏
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	imshow("warpPerspective", imageTransform_new);
	imwrite("trans1.jpg", imageTransform_new);
	waitKey();

	//图像拷贝

	Mat dst(new_height, new_width, CV_8UC3);
	//最后，想拼回去的时候，也要拼新坐标，而不是之前的负数
	dst.setTo(0);

	//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪
	imageTransform_new.copyTo(dst(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
	//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多
	image_main.copyTo(dst(Rect(0, 0, image_main.cols, image_main.rows)));

	imshow("b_dst", dst);
	// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	OptimizeSeam(image_main, imageTransform_new, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);

	//waitKey();

}

void surfBased_extraction_match_new_resize(char* src_path_main, char* src_path_new, char* dst_path, int accuracy)
{
	//image_main.copyTo(dst(Rect(0, 0, image_main.cols, image_main.rows)));

	//把main   resize
	//todo:最后还要把这个大图算出上下限，再缩小一点。
	Mat image_main = imread(src_path_main, IMREAD_UNCHANGED);//D:\VS2015\Projects
	resize(image_main, image_main,Size(image_main.cols, image_main.rows));//todo:这里是缩放，暂时先不缩放
	Mat image_new = imread(src_path_new, IMREAD_UNCHANGED);

	cv::Mat image_large = Mat(Size(image_main.cols * 3, image_main.rows * 3), CV_8UC3, Scalar(0, 0, 0));
	image_main.copyTo(image_large(Rect(image_main.cols, image_main.rows, image_main.cols, image_main.rows)));


	//Mat image_main, image2;
// 	cvtColor(image_large, image_large, CV_RGB2GRAY);
// 	cvtColor(image_new, image_new, CV_RGB2GRAY);

	SurfFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint_main, keyPoint_new;
	Detector.detect(image_large, keyPoint_main);
	Detector.detect(image_new, keyPoint_new);

	SurfDescriptorExtractor Descriptor;
	Mat imageDesc_main, imageDesc_new;
	Descriptor.compute(image_large, keyPoint_main, imageDesc_main);
	Descriptor.compute(image_new, keyPoint_new, imageDesc_new);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	vector<Mat> train_desc(1, imageDesc_main);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc_new, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,获取优秀匹配点
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6的系数推荐，0.4，准确度高，0.6，匹配点数量多，0.5，一般。注意，distanc就是distance，就是看点的空间位置关系
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image_new, keyPoint_new, image_large, keyPoint_main, GoodMatchePoints, first_match);
	//imshow("first_match ", first_match);
	//waitKey();

	//转换数据类型，用于配准
	vector<Point2f> imagePoints_main, imagePoints_new;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Good只留一对信息，所以，两个点集，刚好取DMatch两个成员,根据knn，点集1是train，所以同一份goodmatch，不同的后缀，注意
		//从match又分离成两个点了，GoodMatch取出的分别对应两个点集的下标，然后取点坐标pt，push回Point2f。
		imagePoints_new.push_back(keyPoint_new[GoodMatchePoints[i].queryIdx].pt);
		imagePoints_main.push_back(keyPoint_main[GoodMatchePoints[i].trainIdx].pt);
	}

	//求变换矩阵来配准
	//homography好像就是拿出几个点作为参考，算出两张图的映射关系，从1到2的投影映射矩阵，3*3的
	//Mat homo = findHomography(keyPoint_new, keyPoint_main, CV_RANSAC);//这里改为，新图到主图的映射
	//Mat homo = findHomography(keyPoint_main, keyPoint_new, CV_RANSAC);//这里改为，新图到主图的映射
	Mat homo = findHomography(imagePoints_new, imagePoints_main, CV_RANSAC);//这里改为，新图到主图的映射
																			////也可以使用getPerspectiveTransform方法获得透视变换矩阵，不过要求只能有4个点，效果稍差  
																			//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "变换矩阵：\n" << homo << endl << endl;//3*3的

	four_corners_t  corners = CalcCorners(homo, image_new);//这是图像new变换后的四个角应该的坐标，把图1贴到图2就依赖这个
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//图像配准
	//改一个灵活的方案：新图的映射可能在各个方向扩展，所以宽高都不能以主图为准，且要考虑正负，
	//先各个方向都取max（也不叫max，左边叫min）但是不能真用负数，最后还要换算总宽度
	int new_left, new_right, new_top, new_bottom, new_width, new_height;//两图比较，左边取min，右边取max
	int reserved_left, reserved_right, reserved_top, reserved_bottom, reserved_width, reserved_height;//所谓reserved，是最后保存的图像，要去黑边

	if (1)//这是按large内直接贴new算的坐标，如果我还要去黑边的话，x不能从0到cols，但是很多逻辑还是靠这套宽高算的
	{
		new_left = MIN(0, MIN(corners.left_top.x, corners.left_bottom.x));//主图的左边，一定是0吧？
		new_right = MAX(image_large.cols, MAX(corners.right_top.x, corners.right_bottom.x));//主图的右边是列数
		new_top = MIN(0, MIN(corners.left_top.y, corners.right_top.y));
		new_bottom = MAX(image_large.rows, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom更大吧
		new_width = new_right - new_left;
		new_height = new_bottom - new_top;

		reserved_left = MIN(image_main.cols, MIN(corners.left_top.x, corners.left_bottom.x));//保留部分就不从0开始算了，从主图漂移的起点算起
		reserved_right = MAX(image_main.cols*2, MAX(corners.right_top.x, corners.right_bottom.x));//这2都是假设原图是放大三倍，并且居中放置
		reserved_top = MIN(image_main.rows, MIN(corners.left_top.y, corners.right_top.y));
		reserved_bottom = MAX(image_main.rows*2, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom更大吧
		reserved_width = reserved_right - reserved_left;
		reserved_height = reserved_bottom - reserved_top;


	}


	//这一步做了什么？是不是负数会有问题？他的这个size怎么算？，输入是image_new，产出是imageTransform_new，什么形式？变换后的图？
	//这一步是关键，可能这一步产生了新位置？新位置不是corners都产生了么？这一步算提前贴到一张空白上？
	//总之，新图怎么贴，决定了旧图放在dst哪，因为dst不再是原图加宽了，新图也不是一定放右边了，坐标系变了。
	//最后，按目前逻辑，不管怎么调整，首先，还是有原图的复制过程，这会不会耗时，暂时先不管。（也没有原图不复制直接网上贴东西的方案吧，又不是文本，这应该是opencv提供的一种优化可能性）

	//	Mat imagePerspectiveTransform_new;
	// 	perspectiveTransform(image_new, image_new, homo);
	// 	imshow("perspectiveTransform", image_new);
	// 	waitKey();
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//这是权重调整？imageTransform2






	
#if 1
	//这个size同样需要放大，问题是放大后，图片放在放大后的什么位置了，以原坐标系还是新坐标系为准？
	//这个接口给堵死了？他这样永远不能向左偏
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	//imshow("warpPerspective", imageTransform_new);
	imwrite("trans1.jpg", imageTransform_new);
	//waitKey();
	//图像拷贝
	//测试：如果旧图足够large，其实不需要新建一个sdst，直接往image_large放就行了
	cv::Mat dst = Mat(Size(new_width, new_height), CV_8UC3, Scalar(0, 0, 0));
	//Mat(Size(image_main.cols * 3, image_main.rows * 3), CV_8UC3, Scalar(0, 0, 0));
	//Mat dst(new_height, new_width, CV_8UC3);
	//最后，想拼回去的时候，也要拼新坐标，而不是之前的负数
	//dst.setTo(0);
	//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多
	//image_large.copyTo(dst(Rect(0, 0, image_large.cols, image_large.rows)));
	//imshow("image_main", image_main);
	
	//因为旧图是不规则的，切多余黑边的时候也总会留黑边，所以必须把旧图先贴上去，新图后贴上去
	//但是怎么这个顺序会找不到旧图呢？因为新图带一个更大的黑边（warpPerspective做出来的）
	//waitKey();

	//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪



	Mat mask3 = image_main.clone();

	imageTransform_new.copyTo(dst(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
	//imwrite("dst1.jpg", dst);
	image_main.copyTo(dst(Rect(image_main.cols, image_main.rows, image_main.cols, image_main.rows)), mask3);//换成把原图拷贝进去


	//imshow("b_dst", dst);
	//OptimizeSeam(image_large, imageTransform_new, dst, corners);
	//imshow("dst", dst);
	imwrite(dst_path, dst(Rect(reserved_left, reserved_top, reserved_width, reserved_height)));
	//cv::Mat dst2 = Mat(Size(reserved_width, reserved_height), CV_8UC3, Scalar(0, 0, 0));
	//dst(Rect(reserved_left, reserved_top, reserved_width, reserved_height)).copyTo(dst2(Rect(0, 0, reserved_width, reserved_height)));
	//imwrite("test.jpg", dst2);
#else
	//方法1：直接往large上映射
// 	Mat imageTransform_new;
// 	warpPerspective(image_new, image_large, homo, Size(new_width, new_height));
// 	imshow("warpPerspective", image_large);
// 	bool ret = imwrite("trans2.jpg", image_large);
// 	std::cout << "ret is " << ret << endl;
	//waitKey();
	//方法2，先做一张图，再copyto，但是需要mask
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	//Mat mask(Size(imageTransform_new.cols, imageTransform_new.rows),);
	Mat mask = imageTransform_new.clone();
	imageTransform_new.copyTo(image_large(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
#endif

	//如果按上边的理解，这也拷贝到0，0起点？可能前者经过变换，本来就size更大，位置更偏？差不多
	//image_large.copyTo(dst(Rect(0, 0, image_large.cols, image_large.rows)));
	//矩形？roi？其实是trans后的图，直接贴到trans后的位置，rect的0,0怎么理解？就是全图大小，只是贴过去自然靠右边？数据不透明，不好跟踪


	// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//图像融合（去裂缝处理）：拼的不自然，后处理,这里的处理思路是加权融合，在重叠部分由前一幅图像慢慢过渡到第二幅图像，即将图像的重叠区域的像素值按一定的权值相加合成新的图像。
	//OptimizeSeam(image_large, imageTransform_new, dst, corners);
	//imshow("image_large", image_large);
	//imwrite(dst_path, image_large);

	//waitKey();

}


#endif