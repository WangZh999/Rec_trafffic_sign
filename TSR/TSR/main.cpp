#include<iostream>
#include"TSR.h"
#include"predict.h"
#include<opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	//Mat yuvimg;
	//
	//VideoCapture video("E:/video/1.mp4");
	//if (!video.isOpened())
	//{
	//	return -1;
	//}

	//// 获取图像帧率
	//double rate = video.get(CV_CAP_PROP_FPS);
	//bool stop(false);
	//cv::Mat frame; // 当前视频帧
	//cv::namedWindow("Extracted Frame",WINDOW_NORMAL);

	//// 每一帧之间的延迟
	//int delay = 1000 / rate;

	//// 遍历每一帧
	//while (1)
	//{
	//	// 尝试读取下一帧
	//	if (!video.read(frame))
	//		break;


	//	cv::imshow("Extracted Frame", frame);
	//	cvtColor(frame, yuvimg, CV_BGRA2YUV_I420);
	//	void * phandle = TSR_DET_Create(frame.cols, frame.rows);
	//	TSR_DET_run(phandle, yuvimg.data, frame.cols, frame.rows);
	//	// 引入延迟
	//	//if (cv::waitKey(delay) >= 0)
	//	//	stop = true;
	//}



	Mat yuvimg;
	Mat img = imread("5.jpg");
	resize(img, img, Size(500, 500));
	//bilateralFilter(img, img, 5, 100, 5);
	blur(img, img, Size(25,25));
	medianBlur(img, img, 9);
	imshow("img", img);
	waitKey();
	cvtColor(img, yuvimg, CV_BGRA2YUV_I420);
	void * phandle = TSR_DET_Create(img.cols, img.rows);
	TSR_DET_run2(phandle, yuvimg.data, img.cols, img.rows);
	waitKey();

	//ANN_Wz _ann = ANN_Wz();
	//Mat test = imread("test_0.jpg", 0);
	//int vv = _ann.predict(test);
	//cout << vv << endl;
	//waitKey();
	//Mat t = imread("503.jpg",0);
	//int vvv = _ann.predict(t);
	//cout << vvv << endl;
	//waitKey();
	//cout << "hhhhh" << endl;

	//FileStorage fs;
	//bool ll = fs.open("e:\\w.xml", FileStorage::READ);
	//cout << ll << endl;
	return 0;
}