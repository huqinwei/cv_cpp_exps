#pragma warning(disable:4996)

#include "video_frame_extraction.h"
//#include <cv.h>

#include <opencv2/highgui/highgui.hpp>
#define NUM_FRAME 30
#define START_FRAME 300
#define CAPTURE_LEAP 300//间隔30太大了？并没有
//todo:未完
//https://blog.csdn.net/u011035640/article/details/79880738
//Video_to_image
//Image_to_video
void video_frame_extraction_demo(char* filename, char* dst_path,int capture_interval)
{
	CvCapture* capture = cvCaptureFromAVI(filename);
	cvQueryFrame(capture);

	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int fps = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int numFrames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n\tframe numbers : %d\n", frameH, frameW, fps, numFrames);

	int i = START_FRAME;
	IplImage *img = 0;
	char image_name[130];
	cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
	while (1)
	{
		img = cvQueryFrame(capture);
		cvShowImage("mainWin", img);
		char key = cvWaitKey(20);
		if (i % capture_interval == 0)
		{
			sprintf(image_name, "%s\\frames%d\\%s%d%s",dst_path, capture_interval, "image", i / capture_interval, ".jpg");
			cvSaveImage(image_name, img);
		}
		++i;
		if (i == NUM_FRAME)
			break;
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("mainWin");
}



/*
void Image_to_video()
{
	int i = 0;
	IplImage* img = 0;
	char image_name[13];
	printf("------------- image to video ... ----------------\n");
	//初始化视频编写器，参数根据实际视频文件修改
	CvVideoWriter *writer = 0;
	int isColor = 1;
	int fps = 30; // or 25
	int frameW = 400; // 744 for firewire cameras
	int frameH = 240; // 480 for firewire cameras
	writer = cvCreateVideoWriter("out.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, cvSize(frameW, frameH), isColor);
	printf("\tvideo height : %d\n\tvideo width : %d\n\tfps : %d\n", frameH, frameW, fps);
	//创建窗口
	cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
	while (i < NUM_FRAME)
	{
		sprintf(image_name, "%s%d%s", "image", ++i, ".jpg");
		img = cvLoadImage(image_name);
		if (!img)
		{
			printf("Could not load image file...\n");
			exit(0);
		}
		cvShowImage("mainWin", img);
		char key = cvWaitKey(20);
		cvWriteFrame(writer, img);
	}
	cvReleaseVideoWriter(&writer);
	cvDestroyWindow("mainWin");
}

int main(int argc, char *argv[])
{
	char filename[13] = "infile.avi";
	Video_to_image(filename); //视频转图片
	Image_to_video();    //图片转视频
	return 0;
}

*/