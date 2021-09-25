
#include "perspectiveTransformDemo.h"
#if 0
void perspectiveTransformDemo1()
{

	Mat img = imread("data/1.jpg", IMREAD_UNCHANGED);//D:\VS2015\Projects
	int img_height = img.rows;
	int img_width = img.cols;
	vector<Point2f> corners(4);
	corners[0] = Point2f(0, 0);
	corners[1] = Point2f(img_width - 1, 0);
	corners[2] = Point2f(0, img_height - 1);
	corners[3] = Point2f(img_width - 1, img_height - 1);

	//todo�������Ҫ�����أ�Ҳ���ǰ첻����Ҳ����ȷ����Ӧ���ǣ��Ŵ󱳾����ѱ���ӳ�䵽����ģ���ǰ�ĵ����᣿������Ҫ���Ǹ�homo��һ��
	vector<Point2f> corners_trans(4);
	corners_trans[0] = Point2f(150, 250);
	corners_trans[1] = Point2f(771, 0);
	corners_trans[2] = Point2f(0, img_height - 1);
	corners_trans[3] = Point2f(650, img_height - 1);


	//����ԭͼ����x,y��findhomo�ҳ������ĸ���x',y'�����ɴ�ͼ��һ����x'',y''��ֱ����x,y��x'',y''�͵õ����µ�transform��
	//ԭͼ�������ˣ�homo������ˣ�ʣ���һ������������ʲô���ʣ��Լ����㣬
// 	large_width-origin_width=x_increment
// 	large_height- origin_width=y_increment
// 	��origin��x_increment��y_incrementΪ������large����ֻ��new�����ϵ������������new��corner���㣬
	//ֻҪȷ��new�ķ�λ���������originҪ����large��ʲôλ�á�Ҳ�����newӦ����large��ʲôλ�ã�x'',y''��



	Mat transform = getPerspectiveTransform(corners, corners_trans);//���׽������������trans�����ܸ�����ã���ʽ�͵�Ӧ��homo����������Ҳ���ܸ��᣿
	cout << transform << endl;//��������homo��һ������ʽ
	vector<Point2f> points, points_trans;
	for(int i = 0; i < img_height; i++)//�߶���y
	{
		for (int j = 0; j < img_width; j++)//�����x
		{
			points.push_back(Point2f(j, i));//���е㶼Ҫѹ����������������
		}
	}

	perspectiveTransform(points, points_trans, transform);
	Mat img_trans = Mat::zeros(img_height, img_width, CV_8UC3);
	int count = 0;
	for (int i = 0; i < img_height; i++) {
		uchar* p = img.ptr<uchar>(i);
		for (int j = 0; j < img_width; j++) {
			int y = points_trans[count].y;
			int x = points_trans[count].x;
			uchar* t = img_trans.ptr<uchar>(y);//tָ�����img_trans��ע�⿴��ߵ�����
			t[x * 3] = p[j * 3];//x�������꣬j�Ǿ�����y�أ����в���
			t[x * 3 + 1] = p[j * 3 + 1];
			t[x * 3 + 2] = p[j * 3 + 2];//+1��+2����ָ��ͨ���ɣ�����jһ��ֻ+1����ô������ͨ����
			count++;

		}
	}
	imshow("windows",img_trans);
	waitKey();


	//��������⣺���ֱ任�к�ɫС�㣬��Ҫ��ֵ��������һ�ַ�����ֱ������ͼ������ԭͼ�ĵ㡣https://blog.csdn.net/xiaowei_cqu/article/details/26471527
	//��һ�ַ�������findHomography��

}


#endif