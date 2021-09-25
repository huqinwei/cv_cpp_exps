#include "feature_extraction_matching.h"

#if 0

// ��������ɻ������ܵ�demo�����кܶ������������
// �������ݶԽӣ������̵߳İ�����
// ����gpu��cuda�ļ��١�
// ������Ҫ��ô��֯Ŀ¼����һ��ͼƬ��һ��Ŀ¼��Ȼ���أ������Աȣ����ںϣ�Ȼ��Ҫ������ɾ����
// ǰ�ỹ���ں���ķ���Ԥ�ڣ������ڻ�û������������������ںϣ�����ͼ����һ�¾�ƴ���ˣ�������ȫŤ������ɢ��ƴ�ӣ�
// ����Ǻ��ߣ���Ҫ��������������⡣


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
		imshow("p2_cvt", image2);//��ʵ���ǻҶ�ͼ
		imshow("p1_cvt", image1);

	}

	SiftFeatureDetector siftDetector(800);
	SurfFeatureDetector surfDetector(800);//��������ֵԽС��Խ�ࣿ//Ptr<SURF> detector = SURF::create( )��
	vector<KeyPoint> keyPoint1, keyPoint2;
	Mat imageDesc1, imageDesc2;
	SurfDescriptorExtractor surfDescriptor;//����������
	SiftDescriptorExtractor siftDescriptor;//����������

	if (0)//��ʱ����
	{
		siftDetector.detect(image1, keyPoint1);
		siftDetector.detect(image2, keyPoint2);
		siftDescriptor.compute(image1, keyPoint1, imageDesc1);
		siftDescriptor.compute(image2, keyPoint2, imageDesc2);
	}
	else//ʵ�⣬surf���ǱȽϲ��
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
	SurfFeatureDetector surfDetector(2000);//hessian������ֵ��Խ�󣬵��٣���׼
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector.detect(image1, keyPoint1);
	surfDetector.detect(image2, keyPoint2);

	SurfDescriptorExtractor surfDescriptor;//����������
	Mat imageDesc1, imageDesc2;
	surfDescriptor.compute(image1, keyPoint1, imageDesc1);
	surfDescriptor.compute(image2, keyPoint2, imageDesc2);

	//�±߾ͺ�֮ǰ��ͬ�ˣ�֮ǰֱ��������descriptionȥmatch�����ڻ���ѵ��
	FlannBasedMatcher matcher;
	vector<vector<DMatch>>matchePoints;//˫�����飬Ӧ����ÿһ���㣬��������ҵ���N��ƥ��㡣
	vector<DMatch> GoodMatchePoints;
	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	cout << "total match points: " << matchePoints.size() << endl;
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
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
	SiftFeatureDetector siftDetector(2000);//hessian������ֵ��Խ�󣬵��٣���׼
	vector<KeyPoint> keyPoint1, keyPoint2;
	siftDetector.detect(image1, keyPoint1);
	siftDetector.detect(image2, keyPoint2);

	SiftDescriptorExtractor siftDescriptor;//����������
	Mat imageDesc1, imageDesc2;
	siftDescriptor.compute(image1, keyPoint1, imageDesc1);
	siftDescriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
	cout << "total match points: " << matchePoints.size() << endl;

	Mat img_match;
	drawMatches(image01, keyPoint1, image02, keyPoint2, matchePoints, img_match);//����ͼ�������㼯��һ����ϵ���ϣ�������һ����ͼ��
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
	//"hessian������ֵ��Խ�󣬵��٣���׼"�����Ǵӽ�������Ҹо��������ɵ���Զ���������������һ����features������keypoint����������֮���о�����˵���෴��
	SiftFeatureDetector surfDetector(1800);
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector.detect(image1, keyPoint1);
	surfDetector.detect(image2, keyPoint2);

	SiftDescriptorExtractor surfDescriptor;//����������
	Mat imageDesc1, imageDesc2;
	surfDescriptor.compute(image1, keyPoint1, imageDesc1);
	surfDescriptor.compute(image2, keyPoint2, imageDesc2);

	//�±߾ͺ�֮ǰ��ͬ�ˣ�֮ǰֱ��������descriptionȥmatch�����ڻ���ѵ��
	FlannBasedMatcher matcher;
	vector<vector<DMatch>>matchePoints;//˫�����飬Ӧ����ÿһ���㣬��������ҵ���N��ƥ��㡣
	vector<DMatch> GoodMatchePoints;
	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);//knn��train�������Ƿ����һ�����ݼ�ô��������
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 3);
	cout << "total match points: " << matchePoints.size() << endl;
	for (int i = 0; i < matchePoints.size(); i++)
	{
		//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		//����ֻ��index����û������֮�����Ϣ�ģ�ֱ�Ӿ���distance
		//�����ľ��룿Ӧ�þ���knnʱ��������ġ�
		//matchePoints��һ����һһ��Ӧ��һ��queryIdx���ܶ�Ӧ�ࣨk����trainIdx��Ҳ����һ��i���ж�����,[0]��[1]�ǲ�ͬ�������Ϣ��distance�Ѿ�������
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
	int start = MIN(corners.left_top.x, corners.left_bottom.x);//�ص��������߽磬ȡ��һ��С��(�϶�����Ҫ����ÿһ��ϸ�ڣ������б�ߣ���Ȼ�������ұ�Ϊ׼)
	double processWidth = img1.cols - start;//�ص������ȣ�img1����img1.cols���ص����ұ߽磿
	int rows = dst.rows;
	int cols = img1.cols;//����*ͨ������
	double alpha = 1;//img1�����ص�Ȩ��
	for (int i = 0; i < rows; i++)
	{
		uchar* p = img1.ptr<uchar>(i);
		uchar* t = trans.ptr<uchar>(i);//trans��˭��trans��
		uchar* d = dst.ptr<uchar>(i);
		for (int j = start; j < cols; j++)//���������ص�����ô
		{
			if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)//�����أ��ڵ㣬������ͨ�������̵ģ�
			{
				alpha = 1;//��ȫ��img1
			}
			else//�����أ�����Ȩ��
			{//��img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ�������ȣ�ʵ��֤�������ַ���ȷʵ�á�
				//��ǰ����j��j-start�Ǿ�����߽磬������ص�����ȣ��������ȼ��꣬���Ǻ��ұߵľ�����ô��������˵���ˣ�ֱ����Խ��Ȩ��Խ�󣬶����Ǿ�������ȣ�����
				//���Ҳ��һ��ȱ�㣬��������ȥ�����İɣ�parallax-tolerant image stitching��local�ġ�
				alpha = (processWidth - (j - start)) / processWidth;//����j�������ƶ���alpha�𽥱�С��Ҳ����tran�ṩ�ĸ�����
			}
			//����Ȩ�أ���p��tƴd��
			d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
			d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);

		}
	}
	

}

four_corners_t CalcCorners(const Mat& H, const Mat& src)//�任������ԭͼ
{
	four_corners_t corners;
	double v2[] = { 0, 0, 1 };//���Ͻ�
	double v1[3];//�任�������ֵ
	Mat V2 = Mat(3, 1, CV_64FC1, v2);  //������
	Mat V1 = Mat(3, 1, CV_64FC1, v1);  //������

	V1 = H * V2;
	//���Ͻ�(0,0,1)
	cout << "V2: " << V2 << endl;
	cout << "V1: " << V1 << endl;
	corners.left_top.x = v1[0] / v1[2];
	corners.left_top.y = v1[1] / v1[2];

	//���½�(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //������
	V1 = Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.left_bottom.x = v1[0] / v1[2];
	corners.left_bottom.y = v1[1] / v1[2];

	//���Ͻ�(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //������
	V1 = Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.right_top.x = v1[0] / v1[2];
	corners.right_top.y = v1[1] / v1[2];

	//���½�(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = Mat(3, 1, CV_64FC1, v2);  //������
	V1 = Mat(3, 1, CV_64FC1, v1);  //������
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

	//��ȡ������    
	SurfFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
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

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);





	//ת���������ͣ�������׼
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners= CalcCorners(homo,image01);//����ͼ��1�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2
	imshow("ֱ�Ӿ���͸�Ӿ���任", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//ͼ�񿽱���ǰ���Ǳ任�õ�imageTransform������Ҫƴ��ȥ�ˡ�
	int dst_width = imageTransform1.cols;//�����ҵ�ĳ�����Ϊƴ��ͼ�ĳ��ȡ�������ͼһŤ������������������ұߵ����ꣿ��
	int dst_height = image02.rows;//ͼ2��������Ϊ�߶�(��ʵimageTransform1Ҳһ����imageTransform1��ͼ1�ı任����ͼ2һ����rows��warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����

	imshow("b_dst", dst);

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
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

	//��ȡ������    
	SiftFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
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

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image02, keyPoint2, image01, keyPoint1, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);





	//ת���������ͣ�������׼
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners = CalcCorners(homo, image01);//����ͼ��1�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2
	imshow("ֱ�Ӿ���͸�Ӿ���任", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//ͼ�񿽱���ǰ���Ǳ任�õ�imageTransform������Ҫƴ��ȥ�ˡ�
	int dst_width = imageTransform1.cols;//�����ҵ�ĳ�����Ϊƴ��ͼ�ĳ��ȡ�������ͼһŤ������������������ұߵ����ꣿ��
	int dst_height = image02.rows;//ͼ2��������Ϊ�߶�(��ʵimageTransform1Ҳһ����imageTransform1��ͼ1�ı任����ͼ2һ����rows��warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����

	imshow("b_dst", dst);
// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
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

	//��ȡ������    
	OrbFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
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



	// Lowe's algorithm,��ȡ����ƥ���
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


	//ת���������ͣ�������׼
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners = CalcCorners(homo, image01);//����ͼ��1�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2
	imshow("ֱ�Ӿ���͸�Ӿ���任", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//ͼ�񿽱���ǰ���Ǳ任�õ�imageTransform������Ҫƴ��ȥ�ˡ�
	int dst_width = imageTransform1.cols;//�����ҵ�ĳ�����Ϊƴ��ͼ�ĳ��ȡ�������ͼһŤ������������������ұߵ����ꣿ��
	int dst_height = image02.rows;//ͼ2��������Ϊ�߶�(��ʵimageTransform1Ҳһ����imageTransform1��ͼ1�ı任����ͼ2һ����rows��warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����

	imshow("b_dst", dst);

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
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
	// ��ʾԴͼ�񣬺ͽ��ͼ��
	imshow("ȫ��ͼ��", pano);
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
// 		{//����ǵ�һ�ţ�
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

	//��ȡ������    
	SiftFeatureDetector Detector(accuracy);
	vector<KeyPoint> keyPoint1, keyPoint2;
	Detector.detect(image1, keyPoint1);
	Detector.detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
	SiftDescriptorExtractor Descriptor;
	Mat imageDesc1, imageDesc2;
	Descriptor.compute(image1, keyPoint1, imageDesc1);
	Descriptor.compute(image2, keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	//�������¼ӵ�,<int,int>�͵�set?
	//MatchesSet matches;
	std::set<std::pair<int, int>> matches;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	cout << "total match points: " << matchePoints.size() << endl;

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
			matches.insert(make_pair(matchePoints[i][0].queryIdx, matchePoints[i][0].trainIdx));
		}
	}
	cout << "\n1->2 matches: " << GoodMatchePoints.size() << endl;

	//�����ƥ��
	FlannBasedMatcher matcher2;
	matchePoints.clear();//�����������
	vector<Mat> train_desc2(1, imageDesc2);
	matcher2.add(train_desc2);
	matcher2.train();
	matcher2.knnMatch(imageDesc1, matchePoints, 2);
	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			//˫��ƥ�䲻��ɸѡƥ��㣬������ƥ��㣬�ǲ�����ע�⿴k-v��ѡ�񣬺͵�һ���Ƿ��ģ���ʵ�ǿ����ظ���
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





	//ת���������ͣ�������׼
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners = CalcCorners(homo, image01);//����ͼ��1�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	Mat imageTransform1, imageTransform2;
	warpPerspective(image01, imageTransform1, homo, Size(MAX(corners.right_top.x, corners.right_bottom.x), image02.rows));
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2
	imshow("ֱ�Ӿ���͸�Ӿ���任", imageTransform1);
	imwrite("trans1.jpg", imageTransform1);



	//ͼ�񿽱���ǰ���Ǳ任�õ�imageTransform������Ҫƴ��ȥ�ˡ�
	int dst_width = imageTransform1.cols;//�����ҵ�ĳ�����Ϊƴ��ͼ�ĳ��ȡ�������ͼһŤ������������������ұߵ����ꣿ��
	int dst_height = image02.rows;//ͼ2��������Ϊ�߶�(��ʵimageTransform1Ҳһ����imageTransform1��ͼ1�ı任����ͼ2һ����rows��warpPerspective)
	Mat dst(dst_height, dst_width, CV_8UC3);
	dst.setTo(0);

	imageTransform1.copyTo(dst(Rect(0, 0, imageTransform1.cols, imageTransform1.rows)));//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���
	image02.copyTo(dst(Rect(0, 0, image02.cols, image02.rows)));//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����

	imshow("b_dst", dst);
	//imwrite(strcat(dst_path, "before_seam_optimized"), dst);//����д��jpg�����

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
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

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image_new, keyPoint_new, image_main, keyPoint_main, GoodMatchePoints, first_match);
	imshow("first_match ", first_match);
	waitKey();

	//ת���������ͣ�������׼
	vector<Point2f> imagePoints_main, imagePoints_new;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints_new.push_back(keyPoint_new[GoodMatchePoints[i].queryIdx].pt);
		imagePoints_main.push_back(keyPoint_main[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	//Mat homo = findHomography(keyPoint_new, keyPoint_main, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
	//Mat homo = findHomography(keyPoint_main, keyPoint_new, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
	Mat homo = findHomography(imagePoints_new, imagePoints_main, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
	////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
	//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners = CalcCorners(homo, image_new);//����ͼ��new�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	//��һ�����ķ�������ͼ��ӳ������ڸ���������չ�����Կ�߶���������ͼΪ׼����Ҫ����������
	//�ȸ�������ȡmax��Ҳ����max����߽�min�����ǲ������ø��������Ҫ�����ܿ��
	int new_left, new_right, new_top, new_bottom;//��ͼ�Ƚϣ����ȡmin���ұ�ȡmax
	new_left = MIN(0, MIN(corners.left_top.x, corners.left_bottom.x));//��ͼ����ߣ�һ����0�ɣ�
	new_right = MAX(image_main.cols, MAX(corners.right_top.x, corners.right_bottom.x));//��ͼ���ұ�������
	new_top = MIN(0, MIN(corners.left_top.y, corners.right_top.y));
	new_bottom = MAX(0, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom�����
	int new_width = new_right - new_left;
	int new_height = new_bottom - new_top;
	
	//��һ������ʲô���ǲ��Ǹ����������⣿�������size��ô�㣿��������image_new��������imageTransform_new��ʲô��ʽ���任���ͼ��
	//��һ���ǹؼ���������һ����������λ�ã���λ�ò���corners��������ô����һ������ǰ����һ�ſհ��ϣ�
	//��֮����ͼ��ô���������˾�ͼ����dst�ģ���Ϊdst������ԭͼ�ӿ��ˣ���ͼҲ����һ�����ұ��ˣ�����ϵ���ˡ�
	//��󣬰�Ŀǰ�߼���������ô���������ȣ�������ԭͼ�ĸ��ƹ��̣���᲻���ʱ����ʱ�Ȳ��ܡ���Ҳû��ԭͼ������ֱ�������������ķ����ɣ��ֲ����ı�����Ӧ����opencv�ṩ��һ���Ż������ԣ�

//	Mat imagePerspectiveTransform_new;
// 	perspectiveTransform(image_new, image_new, homo);
// 	imshow("perspectiveTransform", image_new);
// 	waitKey();
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2


	//���sizeͬ����Ҫ�Ŵ������ǷŴ��ͼƬ���ڷŴ���ʲôλ���ˣ���ԭ����ϵ����������ϵΪ׼��
	//����ӿڸ������ˣ���������Զ��������ƫ
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	imshow("warpPerspective", imageTransform_new);
	imwrite("trans1.jpg", imageTransform_new);
	waitKey();

	//ͼ�񿽱�

	Mat dst(new_height, new_width, CV_8UC3);
	//�����ƴ��ȥ��ʱ��ҲҪƴ�����꣬������֮ǰ�ĸ���
	dst.setTo(0);

	//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���
	imageTransform_new.copyTo(dst(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
	//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����
	image_main.copyTo(dst(Rect(0, 0, image_main.cols, image_main.rows)));

	imshow("b_dst", dst);
	// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
	OptimizeSeam(image_main, imageTransform_new, dst, corners);
	imshow("dst", dst);
	imwrite(dst_path, dst);

	//waitKey();

}

void surfBased_extraction_match_new_resize(char* src_path_main, char* src_path_new, char* dst_path, int accuracy)
{
	//image_main.copyTo(dst(Rect(0, 0, image_main.cols, image_main.rows)));

	//��main   resize
	//todo:���Ҫ�������ͼ��������ޣ�����Сһ�㡣
	Mat image_main = imread(src_path_main, IMREAD_UNCHANGED);//D:\VS2015\Projects
	resize(image_main, image_main,Size(image_main.cols, image_main.rows));//todo:���������ţ���ʱ�Ȳ�����
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

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{//0.4~0.6��ϵ���Ƽ���0.4��׼ȷ�ȸߣ�0.6��ƥ��������࣬0.5��һ�㡣ע�⣬distanc����distance�����ǿ���Ŀռ�λ�ù�ϵ
		if (matchePoints[i][0].distance < 0.4 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
		}
	}

	Mat first_match;
	drawMatches(image_new, keyPoint_new, image_large, keyPoint_main, GoodMatchePoints, first_match);
	//imshow("first_match ", first_match);
	//waitKey();

	//ת���������ͣ�������׼
	vector<Point2f> imagePoints_main, imagePoints_new;
	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		//Goodֻ��һ����Ϣ�����ԣ������㼯���պ�ȡDMatch������Ա,����knn���㼯1��train������ͬһ��goodmatch����ͬ�ĺ�׺��ע��
		//��match�ַ�����������ˣ�GoodMatchȡ���ķֱ��Ӧ�����㼯���±꣬Ȼ��ȡ������pt��push��Point2f��
		imagePoints_new.push_back(keyPoint_new[GoodMatchePoints[i].queryIdx].pt);
		imagePoints_main.push_back(keyPoint_main[GoodMatchePoints[i].trainIdx].pt);
	}

	//��任��������׼
	//homography��������ó���������Ϊ�ο����������ͼ��ӳ���ϵ����1��2��ͶӰӳ�����3*3��
	//Mat homo = findHomography(keyPoint_new, keyPoint_main, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
	//Mat homo = findHomography(keyPoint_main, keyPoint_new, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
	Mat homo = findHomography(imagePoints_new, imagePoints_main, CV_RANSAC);//�����Ϊ����ͼ����ͼ��ӳ��
																			////Ҳ����ʹ��getPerspectiveTransform�������͸�ӱ任���󣬲���Ҫ��ֻ����4���㣬Ч���Բ�  
																			//Mat   homo=getPerspectiveTransform(imagePoints1,imagePoints2);  
	cout << "�任����\n" << homo << endl << endl;//3*3��

	four_corners_t  corners = CalcCorners(homo, image_new);//����ͼ��new�任����ĸ���Ӧ�õ����꣬��ͼ1����ͼ2���������
	cout << "left_top:" << corners.left_top << endl;
	cout << "left_bottom:" << corners.left_bottom << endl;
	cout << "right_top:" << corners.right_top << endl;
	cout << "right_bottom:" << corners.right_bottom << endl;

	//ͼ����׼
	//��һ�����ķ�������ͼ��ӳ������ڸ���������չ�����Կ�߶���������ͼΪ׼����Ҫ����������
	//�ȸ�������ȡmax��Ҳ����max����߽�min�����ǲ������ø��������Ҫ�����ܿ��
	int new_left, new_right, new_top, new_bottom, new_width, new_height;//��ͼ�Ƚϣ����ȡmin���ұ�ȡmax
	int reserved_left, reserved_right, reserved_top, reserved_bottom, reserved_width, reserved_height;//��νreserved������󱣴��ͼ��Ҫȥ�ڱ�

	if (1)//���ǰ�large��ֱ����new������꣬����һ�Ҫȥ�ڱߵĻ���x���ܴ�0��cols�����Ǻܶ��߼����ǿ����׿�����
	{
		new_left = MIN(0, MIN(corners.left_top.x, corners.left_bottom.x));//��ͼ����ߣ�һ����0�ɣ�
		new_right = MAX(image_large.cols, MAX(corners.right_top.x, corners.right_bottom.x));//��ͼ���ұ�������
		new_top = MIN(0, MIN(corners.left_top.y, corners.right_top.y));
		new_bottom = MAX(image_large.rows, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom�����
		new_width = new_right - new_left;
		new_height = new_bottom - new_top;

		reserved_left = MIN(image_main.cols, MIN(corners.left_top.x, corners.left_bottom.x));//�������־Ͳ���0��ʼ���ˣ�����ͼƯ�Ƶ��������
		reserved_right = MAX(image_main.cols*2, MAX(corners.right_top.x, corners.right_bottom.x));//��2���Ǽ���ԭͼ�ǷŴ����������Ҿ��з���
		reserved_top = MIN(image_main.rows, MIN(corners.left_top.y, corners.right_top.y));
		reserved_bottom = MAX(image_main.rows*2, MAX(corners.left_bottom.y, corners.right_bottom.y));//bottom�����
		reserved_width = reserved_right - reserved_left;
		reserved_height = reserved_bottom - reserved_top;


	}


	//��һ������ʲô���ǲ��Ǹ����������⣿�������size��ô�㣿��������image_new��������imageTransform_new��ʲô��ʽ���任���ͼ��
	//��һ���ǹؼ���������һ����������λ�ã���λ�ò���corners��������ô����һ������ǰ����һ�ſհ��ϣ�
	//��֮����ͼ��ô���������˾�ͼ����dst�ģ���Ϊdst������ԭͼ�ӿ��ˣ���ͼҲ����һ�����ұ��ˣ�����ϵ���ˡ�
	//��󣬰�Ŀǰ�߼���������ô���������ȣ�������ԭͼ�ĸ��ƹ��̣���᲻���ʱ����ʱ�Ȳ��ܡ���Ҳû��ԭͼ������ֱ�������������ķ����ɣ��ֲ����ı�����Ӧ����opencv�ṩ��һ���Ż������ԣ�

	//	Mat imagePerspectiveTransform_new;
	// 	perspectiveTransform(image_new, image_new, homo);
	// 	imshow("perspectiveTransform", image_new);
	// 	waitKey();
	//warpPerspective(image01, imageTransform2, adjustMat*homo, Size(image02.cols*1.3, image02.rows*1.8));//����Ȩ�ص�����imageTransform2






	
#if 1
	//���sizeͬ����Ҫ�Ŵ������ǷŴ��ͼƬ���ڷŴ���ʲôλ���ˣ���ԭ����ϵ����������ϵΪ׼��
	//����ӿڸ������ˣ���������Զ��������ƫ
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	//imshow("warpPerspective", imageTransform_new);
	imwrite("trans1.jpg", imageTransform_new);
	//waitKey();
	//ͼ�񿽱�
	//���ԣ������ͼ�㹻large����ʵ����Ҫ�½�һ��sdst��ֱ����image_large�ž�����
	cv::Mat dst = Mat(Size(new_width, new_height), CV_8UC3, Scalar(0, 0, 0));
	//Mat(Size(image_main.cols * 3, image_main.rows * 3), CV_8UC3, Scalar(0, 0, 0));
	//Mat dst(new_height, new_width, CV_8UC3);
	//�����ƴ��ȥ��ʱ��ҲҪƴ�����꣬������֮ǰ�ĸ���
	//dst.setTo(0);
	//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����
	//image_large.copyTo(dst(Rect(0, 0, image_large.cols, image_large.rows)));
	//imshow("image_main", image_main);
	
	//��Ϊ��ͼ�ǲ�����ģ��ж���ڱߵ�ʱ��Ҳ�ܻ����ڱߣ����Ա���Ѿ�ͼ������ȥ����ͼ������ȥ
	//������ô���˳����Ҳ�����ͼ�أ���Ϊ��ͼ��һ������ĺڱߣ�warpPerspective�������ģ�
	//waitKey();

	//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���



	Mat mask3 = image_main.clone();

	imageTransform_new.copyTo(dst(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
	//imwrite("dst1.jpg", dst);
	image_main.copyTo(dst(Rect(image_main.cols, image_main.rows, image_main.cols, image_main.rows)), mask3);//���ɰ�ԭͼ������ȥ


	//imshow("b_dst", dst);
	//OptimizeSeam(image_large, imageTransform_new, dst, corners);
	//imshow("dst", dst);
	imwrite(dst_path, dst(Rect(reserved_left, reserved_top, reserved_width, reserved_height)));
	//cv::Mat dst2 = Mat(Size(reserved_width, reserved_height), CV_8UC3, Scalar(0, 0, 0));
	//dst(Rect(reserved_left, reserved_top, reserved_width, reserved_height)).copyTo(dst2(Rect(0, 0, reserved_width, reserved_height)));
	//imwrite("test.jpg", dst2);
#else
	//����1��ֱ����large��ӳ��
// 	Mat imageTransform_new;
// 	warpPerspective(image_new, image_large, homo, Size(new_width, new_height));
// 	imshow("warpPerspective", image_large);
// 	bool ret = imwrite("trans2.jpg", image_large);
// 	std::cout << "ret is " << ret << endl;
	//waitKey();
	//����2������һ��ͼ����copyto��������Ҫmask
	Mat imageTransform_new;
	warpPerspective(image_new, imageTransform_new, homo, Size(new_width, new_height));
	//Mat mask(Size(imageTransform_new.cols, imageTransform_new.rows),);
	Mat mask = imageTransform_new.clone();
	imageTransform_new.copyTo(image_large(Rect(0, 0, imageTransform_new.cols, imageTransform_new.rows)));
#endif

	//������ϱߵ���⣬��Ҳ������0��0��㣿����ǰ�߾����任��������size����λ�ø�ƫ�����
	//image_large.copyTo(dst(Rect(0, 0, image_large.cols, image_large.rows)));
	//���Σ�roi����ʵ��trans���ͼ��ֱ������trans���λ�ã�rect��0,0��ô��⣿����ȫͼ��С��ֻ������ȥ��Ȼ���ұߣ����ݲ�͸�������ø���


	// 	imwrite(strcat(dst_path,"before_seam_optimized"), dst);

	//ͼ���ںϣ�ȥ�ѷ촦����ƴ�Ĳ���Ȼ������,����Ĵ���˼·�Ǽ�Ȩ�ںϣ����ص�������ǰһ��ͼ���������ɵ��ڶ���ͼ�񣬼���ͼ����ص����������ֵ��һ����Ȩֵ��Ӻϳ��µ�ͼ��
	//OptimizeSeam(image_large, imageTransform_new, dst, corners);
	//imshow("image_large", image_large);
	//imwrite(dst_path, image_large);

	//waitKey();

}


#endif