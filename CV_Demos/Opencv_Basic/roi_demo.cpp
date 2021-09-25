#include "roi_demo.h"
#if 0 

void roi_demo1()
{
	char* window = "img";
	int position_x = 1000, position_y = 10, fullscreen = 0;

	//读取待复制图片  
	cv::Mat roi = cv::imread("data/1.jpg", cv::IMREAD_COLOR);
	cv::Mat roi_2 = cv::imread("data/2.jpg", cv::IMREAD_COLOR);
	//cv::imshow("roi", roi);   

	//namedWindow(window, CV_WINDOW_AUTOSIZE);
	while (true)
	{
		//设置黑色背景画布
		cv::Mat image = cv::Mat::zeros(1024, 2048, CV_8UC3);//设置画布大小
		image.setTo(cv::Scalar(0, 0, 0));//设置背景颜色       

										 //设置画布绘制区域并复制  
		cv::Rect roi_rect = cv::Rect(10, 10, roi.cols, roi.rows);
		cv::Rect roi_rect_2 = cv::Rect(position_x, position_y, roi_2.cols, roi_2.rows);

		roi.copyTo(image(roi_rect));
		roi_2.copyTo(image(roi_rect_2));

		char key = cvWaitKey(30);
		switch (key)
		{
			//ROI平移操作           
		case 'a':
			position_x -= 1; break;
		case 's':
			position_y += 1; break;
		case 'd':
			position_x += 1; break;
		case 'w':
			position_y -= 1; break;
		case 'z':
			resize(roi_2, roi_2, Size(roi_2.cols*0.999, roi_2.rows*0.999), 0, 0, INTER_LINEAR);
			break;
		case 'x':
			resize(roi_2, roi_2, Size(roi_2.cols*1.005, roi_2.rows*1.005), 0, 0, INTER_LINEAR);
			break;
		case 'p':
			fullscreen = 1;
			break;
		case 'o':
			fullscreen = 0;
			break;
		}
		if (key == 27) break;

		namedWindow("LightField", CV_WINDOW_NORMAL);
		setWindowProperty("LightField", CV_WND_PROP_FULLSCREEN, fullscreen);
		cv::imshow("LightField", image);
	}
	return ;

}

#endif