#include "blog_demo2.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace std;
using namespace cv;

void blog_demo2_create_mat()
{
	//�½��˸�2*2�ģ�ÿһ������һ��scalar  0,0,255
	Mat img1(2, 2, CV_8UC3, Scalar(0, 0, 255));
	cout << img1 << endl;
	
	int sz[3] = { 2,2,2 };
	Mat img2(3, sz, CV_8UC1, Scalar(0, 0, 0));
	//cout << img2 << endl;//��ӡ������ԭ��Ҳ��ע��״̬������ʾ����
	Mat img2_2(4, 4, CV_8UC3, Scalar(45));//3ͨ������1άscalar�Ϳ��ԣ���֮���У�
	cout << img2_2 << endl;
	Mat img2_3(4, 4, CV_8UC3, Scalar(45, 11, 33));//3ͨ����Ӧ
	cout << img2_3 << endl;
	Mat img2_4(4, 4, CV_8UC1, Scalar(1));//1ͨ����Ӧ
	cout << img2_4 << endl;

	Mat img5;
	img5.create(4, 4, CV_8UC3);//8UC3��������ͨ��RGB������ΪʲôĬ��ֵ205��
	cout << "before:img5:" << img5 << endl;


	Mat img6 = Mat::zeros(4, 4, CV_8UC3);
	cout << "before:img6:" << img6 << endl;
	Mat img6_2 = Mat::zeros(4, 4, CV_8UC1);
	cout << img6_2 << endl;

	Mat img7 = img6.clone();
	cout << "before:img7:" << img7 << endl;

	Mat img8;
	img6.copyTo(img8);//������ǳ��
	cout << "before:img8:"<< img8 << endl;

	//����ǳ�������ĸ�ֵ���ԣ���θģ�
	Mat img9 = img8;
	Mat img10(img8);
	cout << "before:img9:" << img9 << endl;

	img8 = img5.clone();//�����ض��򣬲��Ǹ�ֵ���ⲻ������
	cout << "after:img8:" << img8 << endl;
	cout << "after:img9:" << img9 << endl;





	system("pause");
}


//ȫ�ֶ�ֵ���;ֲ�����Ӧ��ֵ��
void blog_demo2_threshold_binary()
{
	Mat image = imread("data/dota2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (image.empty())
	{
		cout << "read image failure" << endl;
		return;
	}

	imshow("ԭͼ", image);
	int th = 100;//100;//������ֵ
	Mat global;
	threshold(image, global, th, 255, CV_THRESH_BINARY_INV);
	imshow("ȫ�ֶ�ֵ��", global);



	//���������
	int blockSize = 25;//25//���С��
	int constValue = 10;
	Mat local;
	adaptiveThreshold(image, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	imshow("�ֲ���ֵ��", local);

	waitKey(0);
	return;

}

void blog_demo2_erode()
{
	Mat image = imread("data/dota2.jpg", CV_LOAD_IMAGE_COLOR);
	imshow("SrcPic", image);
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15)); //getStructuringElement�������ص���ָ����״�ͳߴ�ĽṹԪ��
	Mat DstPic;
	erode(image, DstPic, element); //��ʴ����
	imshow("��ʴЧ��ͼ", DstPic);
	waitKey(0);
	return;
}

void blog_demo2_blur()
{
	Mat srcPic = imread("data/dota2.jpg", CV_LOAD_IMAGE_COLOR);
	imshow("Src Pic", srcPic);
	
	Mat DstPic;
	blur(srcPic, DstPic, Size(7,7)); //��ʴ����
	imshow("��ֵģ��Ч��ͼ", DstPic);
	waitKey(0);
}

void blog_demo2_canny()
{
	Mat srcPic = imread("data/dota2.jpg", CV_LOAD_IMAGE_COLOR);
	imshow("Src Pic", srcPic);

	Mat DstPic,edge,grayImage,colorImage1,colorImage2;
	DstPic.create(srcPic.size(), srcPic.type());
	imshow("DstPic", DstPic);
	//cvtColor(srcPic, grayImage, COLOR_BGR2GRAY);//��������2gray������һ�������Ƿ���Ͳ�һ����,���ǣ�Ҳ�䲻��ȥ�ˣ������������˳����BGR��
	cvtColor(srcPic, grayImage, COLOR_RGB2GRAY);
	imshow("grayImage", grayImage);
	cvtColor(grayImage, colorImage1, COLOR_GRAY2BGR);
	imshow("colorImage1", colorImage1);
	cvtColor(grayImage, colorImage2, COLOR_GRAY2RGB);
	imshow("colorImage2", colorImage2);
	blur(grayImage, edge, Size(3, 3));
	imshow("edge", edge);
	Canny(edge, edge, 3, 9, 3);;
	imshow("��Ե��ȡЧ��", edge);



	waitKey(0);
	return;
}

#define PIC_MAX_NUM 5
int pic_num = 0;
void on_track(int, void*)
{
	char file[100];
	sprintf(file, "data/dota%d.jpg", pic_num);
	Mat img = imread(file);
	if (!img.data) {
		cout << "read pic failed" << endl;
		return;
	}
	imshow("չʾ���ͼƬ", img);
}
void blog_demo2_trackbar()
{
	namedWindow("չʾ���ͼƬ");//���name�ǹؼ��֣���Ҫ�����ģ���������
	createTrackbar("ͼƬ���", "չʾ���ͼƬ", &pic_num, PIC_MAX_NUM, on_track);
	on_track(pic_num, NULL);
	waitKey(0);

}

void blog_demo2_data_sctructure() 
{
	Mat m1(2, 2, CV_8UC3, Scalar(0, 0, 255));//CV_[λ��][���޷���][����ǰ׺]c[ͨ����]
	cout << m1 << endl;

	//��Ipl��ʼ����arrתmat��mat��ʼ��Ipl
	IplImage* image = cvLoadImage("data/dota1.jpg");
	Mat mat = cvarrToMat(image);
	IplImage img = IplImage(mat);
	Mat m2;
	m2.create(4, 5, CV_8UC(2));

	Point p;
	p.x = 1;
	p.y = 1;

	Point p2(1, 1);
	Scalar(1, 1, 1);//(b,g,r)˳����rgb

	Size(4, 4);

	//x,y,width,height
	Rect r1(0, 0, 100, 60);
	Rect r2(10, 10, 100, 60);
	Rect r3 = r1 | r2;//����������Ĳ�������ô��ӡ��������
	Rect r4 = r1&r2;
	cout << r1 << endl;
	cout << r2 << endl;
	cout << r3 << endl;//ʵ���ϣ����������ĸ����ˣ��ǲ��˺����������ļ�
	//Rect *pr3 = &r3;
	//cout << "area:"<< r3.area << endl;
	//imshow("rect", r3);
	cout << r4 << endl;


	waitKey(0);
}


void blog_demo2_access_to_pixel()
{
	Mat img = imread("data/bolt.png");
	for (int i = 0; i < img.rows; i++)
	{
		uchar* data = img.ptr<uchar>(i);//�����������Լ�������һ��ptr
		for (int j = 0; j < img.cols; j++)
			printf("%d\t", data[j]);
		printf("\n");

	}

	waitKey(0);
}


void blog_demo2_equalize_hist()
{
	Mat img = imread("data/dota1.jpg");
	imshow("origin pic", img);

	Mat dst;
	cvtColor(img, img, CV_RGB2GRAY);
	imshow("�Ҷ�ͼ", img);
	equalizeHist(img, dst);
	imshow("ֱ��ͼ���⻯", dst);
	
	waitKey(0);

}