#include <malloc.h>
#include <string.h>
#include "TSR.h"
#include "util.h"
#include "osd.h"
#include "contours.h"
#include "predict.h"


#include<iostream>

#ifdef WIN32
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#endif
#include "image_proc_utils.h"

using namespace cv;
using namespace std;

#define BITMAP_LEN (256*256*32)

//单帧图像中最多检测到的矩形区域数目
#ifndef MAX_RECT_NUM
#define MAX_RECT_NUM	 200
#endif



typedef struct _TSR_HANDLE_
{
	int       rect_num;
	int       mem_len;
	PBYTE     temp_image;
	PBYTE     binary_image;
	PBYTE     bitmap;
	PBYTE     memory;
	CV_RECT   rect_list[MAX_RECT_NUM];
}TSR_HANDLE;

int CreateRedBitMap(unsigned char *bitmap)
{
	int hsv_shift = 16;
	int sdiv_table[256];
	int hdiv_table180[256];
	int i = 0;
	short y, u, v;
	short r, g, b;
	short c, d, e;
	int h, s;
	int min, max, diff;
	int vr, vg;
	int hr = 180;
	int pos = 0;
	unsigned char bit = 0x01;
	int shift = 0;

	if (bitmap == NULL)
	{
		return -1;
	}

	sdiv_table[0] = hdiv_table180[0] = 0;
	for( i = 1; i < 256; i++ )
	{
		sdiv_table[i] = (int)((255 << hsv_shift)/(1.*i));
		hdiv_table180[i] = (int)((180 << hsv_shift)/(6.*i));
	}

	for (y = 0; y < 256; y++)
	{
		for (u = 0; u < 256; u++)
		{
			for (v = 0; v < 256; v++)
			{
				pos = (y << 16) + (u << 8) + v;
				shift = pos%8;
				pos >>= 3;

				c = y - 16;
				d = u - 128;
				e = v - 128;

				r = ( 298*c         + 409*e + 128) >> 8;
				g = ( 298*c - 100*d - 208*e + 128) >> 8;
				b = ( 298*c + 516*d         + 128) >> 8;
				r = MAX(0, MIN(r, 255));
				g = MAX(0, MIN(g, 255));
				b = MAX(0, MIN(b, 255));

				max = MAX( b, g );
				max = MAX( max, r );
				min = MIN( b, g );
				min = MIN( min, r );

				diff = max - min;
				vr = max == r ? -1 : 0;
				vg = max == g ? -1 : 0;
				s = (diff * sdiv_table[max] + (1 << (hsv_shift-1))) >> hsv_shift;
				h = (vr & (g - b)) +
					(~vr & ((vg & (b - r + 2 * diff)) + ((~vg) & (r - g + 4 * diff))));
				h = (h * hdiv_table180[diff] + (1 << (hsv_shift-1))) >> hsv_shift;
				h += h < 0 ? hr : 0;
				if ((((h >= 120 && h <= 180 ) || h <= 10) && (s >= 43 && s <= 255) && (max >= 46 && max <= 255)) 
					/*|| (max >= 0 && max <= 60) */)
				{
					bitmap[pos] |= (bit<<shift);
				}
				else
				{
					bitmap[pos] &= ~(bit<<shift);
				}
			}
		}
	}
	return 0;
}


int CreateWBBitMap(unsigned char *bitmap)
{
	int hsv_shift = 16;
	int sdiv_table[256];
	int hdiv_table180[256];
	int i = 0;
	short y, u, v;
	short r, g, b;
	short c, d, e;
	int h, s;
	int min, max, diff;
	int vr, vg;
	int hr = 180;
	int pos = 0;
	unsigned char bit = 0x01;
	int shift = 0;

	if (bitmap == NULL)
	{
		return -1;
	}

	sdiv_table[0] = hdiv_table180[0] = 0;
	for( i = 1; i < 256; i++ )
	{
		sdiv_table[i] = (int)((255 << hsv_shift)/(1.*i));
		hdiv_table180[i] = (int)((180 << hsv_shift)/(6.*i));
	}

	for (y = 0; y < 256; y++)
	{
		for (u = 0; u < 256; u++)
		{
			for (v = 0; v < 256; v++)
			{
				pos = (y << 16) + (u << 8) + v;
				shift = pos%8;
				pos >>= 3;

				c = y - 16;
				d = u - 128;
				e = v - 128;

				r = ( 298*c         + 409*e + 128) >> 8;
				g = ( 298*c - 100*d - 208*e + 128) >> 8;
				b = ( 298*c + 516*d         + 128) >> 8;
				r = MAX(0, MIN(r, 255));
				g = MAX(0, MIN(g, 255));
				b = MAX(0, MIN(b, 255));

				max = MAX( b, g );
				max = MAX( max, r );
				min = MIN( b, g );
				min = MIN( min, r );

				diff = max - min;
				vr = max == r ? -1 : 0;
				vg = max == g ? -1 : 0;
				s = (diff * sdiv_table[max] + (1 << (hsv_shift-1))) >> hsv_shift;
				h = (vr & (g - b)) +
					(~vr & ((vg & (b - r + 2 * diff)) + ((~vg) & (r - g + 4 * diff))));
				h = (h * hdiv_table180[diff] + (1 << (hsv_shift-1))) >> hsv_shift;
				h += h < 0 ? hr : 0;
				if ((max >= 0 && max <= 46) || (s >= 0 && s <= 30 && max >= 221 && max <= 255))
				{
					bitmap[pos] |= (bit<<shift);
				}
				else
				{
					bitmap[pos] &= ~(bit<<shift);
				}
			}
		}
	}
	return 0;
}

int ConvertHsvMask(BYTE *bitmap, BYTE *mask, unsigned char *src, int image_w, int image_h, CV_RECT *roi)
{
	int i = 0;
	int begin_x, end_x, begin_y, end_y;
	unsigned char *pu8Y = NULL;
	unsigned char *pu8U = NULL;
	unsigned char *pu8V = NULL;
	unsigned char bit = 0x01;

	if (roi == NULL)
	{
		begin_x = 0;
		end_x = image_w-1;
		begin_y = 0;
		end_y = image_h-1;
	}
	else
	{
		begin_x = roi->x;
		end_x = roi->x+roi->width-1;
		begin_y = roi->y;
		end_y = roi->y+roi->height-1;
	}

	pu8Y = src;
	pu8U = src + image_w*image_h;
	pu8V = pu8U   + image_w*image_h/4;
#pragma omp parallel for  schedule(dynamic)	
	for (i = begin_y; i <= end_y; i+= 2)
	{
		unsigned char y = 0;
		unsigned char u = 0;
		unsigned char v = 0;
		int pos = 0;
		int shift = 0;
		int j = 0;

		for (j = begin_x; j <= end_x; j += 2)
		{
			y = *(pu8Y + i*image_w + j);
			u = *(pu8U + i*image_w/4 + j/2);
			v = *(pu8V + i*image_w/4 + j/2);
			pos = (y << 16) + (u << 8) + v;
			shift = pos%8;
			pos >>= 3;
			//mask[i*image_w+j] = (bitmap[pos]&(bit<<shift))?1:0;
			mask[i*image_w+j] = (bitmap[pos]>>shift)&bit;


			/*下一列*/
			y = *(pu8Y + i*image_w + j + 1);
			pos = (y << 16) + (u << 8) + v;
			shift = pos%8;
			pos >>= 3;
			//mask[i*image_w+j+1] = (bitmap[pos]&(bit<<shift))?1:0;
			mask[i*image_w+j+1] = (bitmap[pos]>>shift)&bit;


			/*下一排*/
			y = *(pu8Y + (i+1)*image_w + j);
			pos = (y << 16) + (u << 8) + v;
			shift = pos%8;
			pos >>= 3;
			//mask[(i+1)*image_w+j] = (bitmap[pos]&(bit<<shift))?1:0;
			mask[(i+1)*image_w+j] = (bitmap[pos]>>shift)&bit;


			y = *(pu8Y + (i+1)*image_w + j + 1);
			pos = (y << 16) + (u << 8) + v;
			shift = pos%8;
			pos >>= 3;
			//mask[(i+1)*image_w+j+1] = (bitmap[pos]&(bit<<shift))?1:0;
			mask[(i+1)*image_w+j+1] = (bitmap[pos]>>shift)&bit;
		}
	}

	return 0;
}



void *TSR_DET_Create(int image_w, int image_h)
{
	TSR_HANDLE *phandle = NULL;

	phandle = (TSR_HANDLE *)malloc(sizeof(TSR_HANDLE));
	if (phandle == NULL)
	{
		return NULL;
	}
	memset(phandle, 0, sizeof(TSR_HANDLE));
	phandle->binary_image = (PBYTE)malloc(image_w*image_h);
	if (phandle->binary_image == NULL)
	{
		TSR_DET_Destroy(phandle);
		return NULL;
	}
	memset(phandle->binary_image, 0, image_w*image_h);
	phandle->temp_image = (PBYTE)malloc(image_w*image_h);
	if (phandle->temp_image == NULL)
	{
		TSR_DET_Destroy(phandle);
		return NULL;
	}
	memset(phandle->temp_image, 0, image_w*image_h);

	phandle->mem_len = image_w*image_h*4;
	phandle->memory = (PBYTE)malloc(phandle->mem_len);
	if (phandle->memory == NULL)
	{
		TSR_DET_Destroy(phandle);
		return NULL;
	}

	phandle->bitmap = (PBYTE)malloc(BITMAP_LEN);
	if (phandle->bitmap == NULL)
	{
		TSR_DET_Destroy(phandle);
		return NULL;
	}
	memset(phandle->bitmap, 0, BITMAP_LEN);
	CreateRedBitMap(phandle->bitmap);
	
	return phandle;
}

void TSR_DET_Destroy(void *phandle)
{
	TSR_HANDLE *ptTSR_HANDLE = NULL;

	if (phandle == NULL)
	{
		return ;
	}

	ptTSR_HANDLE = (TSR_HANDLE *)phandle;
	if (ptTSR_HANDLE->binary_image != NULL)
	{
		free(ptTSR_HANDLE->binary_image);
	}
	if (ptTSR_HANDLE->temp_image != NULL)
	{
		free(ptTSR_HANDLE->temp_image);
	}
	if (ptTSR_HANDLE->memory != NULL)
	{
		free(ptTSR_HANDLE->memory);
	}
	if (ptTSR_HANDLE->bitmap != NULL)
	{
		free(ptTSR_HANDLE->bitmap);
	}

	free(ptTSR_HANDLE);
	return ;
}

#ifdef WIN32
int ShowImageOpencv(unsigned char *edge, int image_w, int image_h, int depth, int nchannels, char *name)
{
	if (nchannels == 1)
	{
		IplImage *ptImg = cvCreateImageHeader(cvSize(image_w, image_h), depth, nchannels);
		if (ptImg == NULL)         
		{
			return -1;
		}
		ptImg->imageData = (char *)edge;
		cvShowImage(name, ptImg);
		cvReleaseImageHeader(&ptImg);
	}
	else
	{
		int i, j;
		IplImage *ptImg = cvCreateImage(cvSize(image_w, image_h), 8, 3);
		if (ptImg == NULL)
		{
			return -1;
		}
		for (i = 0; i < image_h; i++)
		{
			for (j = 0; j < image_w; j++)
			{
				ptImg->imageData[i*ptImg->widthStep+nchannels*j] = edge[i*image_w+j];
				ptImg->imageData[i*ptImg->widthStep+nchannels*j+1] = edge[i*image_w+j];
				ptImg->imageData[i*ptImg->widthStep+nchannels*j+2] = edge[i*image_w+j];
			}
		}
		cvShowImage(name, ptImg);
		cvReleaseImage(&ptImg);
	}
	cvWaitKey(1);
	return 0;
}
#endif

int Save_Image(PBYTE temp, int width, int height, CV_RECT *roi)
{
	static int count = 0;
	char filename[256];
	Mat image = Mat(Size(width, height), CV_8UC1, temp);

	if (roi != NULL)
	{
		Mat sub_img(image, Rect(roi->x,roi->y,roi->width,roi->height));  
		image = sub_img.clone();
	}
	//sprintf_s(filename, sizeof(filename), "E:\\VideoCheckAlgorithm\\Osd\\Debug\\pic\\%d.jpg", count++);
	sprintf_s(filename, sizeof(filename), "%d.jpg", count++);
	//imwrite(filename, image);
	try {
		imwrite(filename, image);
	}
	catch (cv::Exception& ex) {
		//fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
		printf_s("Exception converting image to PNG format: %s\n", ex.what());
		return 1;
	}
	return 0;
}




#define DST_WID 128
#define DST_HEI 128


int image_binary_otsu(PBYTE src, PBYTE dst, int image_w, int image_h)
{
	Mat src_img = Mat(Size(image_w, image_h), CV_8UC1, src);
	Mat dst_img = Mat(Size(image_w, image_h), CV_8UC1, dst);

	threshold(src_img, dst_img, 100, 255, CV_THRESH_OTSU);
	return 0;
}


int TSR_DET_run(void *phandle, PBYTE yuv420, int image_w, int image_h)
{
	ANN_Wz _ann = ANN_Wz();
	//test();

	TSR_HANDLE *ptTSR_HANDLE = NULL;
	CV_SEQ *ptSeq = NULL;
	CV_SEQ *ptTmp = NULL;
	CV_RECT rect;
	PBYTE  binary, temp, buffer;
	int i = 0;
	MEMORY_BUFFER storage;

	CHECK_ERROR((phandle == NULL), -1);

	ptTSR_HANDLE = (TSR_HANDLE *)phandle;

	rect.x = 8;
	rect.y = 8;
	rect.width = image_w-8;
	rect.height = image_h/2;

	binary = ptTSR_HANDLE->binary_image;
	temp = ptTSR_HANDLE->temp_image;
	buffer = ptTSR_HANDLE->memory;
	memset(temp, 0, image_w*image_h);
	memset(binary, 0, image_w*image_h);

	ConvertHsvMask(ptTSR_HANDLE->bitmap, binary, yuv420, image_w, image_h, &rect);
	//{
	//	Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3), Point(-1,-1));
	//	Mat image = Mat(Size(image_w, image_h), CV_8UC1, binary);

	//	erode(image, image, kernel);
	//}
	image_threshold(binary, temp, 1, 255,  image_w, image_h, NULL);
	//image_binary_otsu(yuv420, temp, image_w, image_h);

	//ShowImageOpencv(temp, image_w, image_h, 8, 1, "mask");	//***********************

	storage.start_pos = ptTSR_HANDLE->memory;
	storage.cur_pos = ptTSR_HANDLE->memory;
	storage.end_pos = ptTSR_HANDLE->memory+ptTSR_HANDLE->mem_len;

	ptTSR_HANDLE->rect_num = 0;
	ptSeq = FindContours(binary, image_w, image_h, CONTOURS_LIST, &storage);
	for (ptTmp = ptSeq; ptTmp != NULL; ptTmp = ptTmp->h_next) 
	{
		CV_RECT sub_rect = BoundingRect(ptTmp);

		if (sub_rect.width *sub_rect.height >= 200 && sub_rect.width *sub_rect.height <= 100000\
			&& sub_rect.width/(float)sub_rect.height >= 0.3 && sub_rect.width/(float)sub_rect.height <= 3.3)
		{
			ptTSR_HANDLE->rect_list[ptTSR_HANDLE->rect_num] = sub_rect;
			ptTSR_HANDLE->rect_num++;

		}
	}

	
	/***************************************
	for (i = 0; i < ptTSR_HANDLE->rect_num; i++)
	{
		CV_RECT sub_rect = ptTSR_HANDLE->rect_list[i];

		if (sub_rect.height >= DST_HEI / 3 && sub_rect.width / (float)sub_rect.height >= 0.05 && sub_rect.width / (float)sub_rect.height <= 0.8)
		{
			//Save_Image(temp, DST_WID, DST_HEI, &sub_rect);
			MerageRectYchannel(temp, DST_WID, DST_HEI, sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height, 255);
		}
		else
		{
			MerageRectYchannel(temp, DST_WID, DST_HEI, sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height, 128);

		}
	}

	ShowImageOpencv(temp, image_w, image_h, 8, 1, "mask");
	//***************************************/


	DestroySeq(ptSeq, &storage);

	for (i = 0; i < ptTSR_HANDLE->rect_num; i++)
	{
		BYTE *buffer = temp+DST_HEI*DST_WID;
		CV_RECT rect = ptTSR_HANDLE->rect_list[i];
		int rect_num = 0;
		//image_copy(yuv420, temp, image_w, image_h, &sub_rect);
		//int cx = rect.x+rect.width/2;
		//int cy = rect.y+rect.height/2;
		//rect.width = rect.width*5/4;
		//rect.height = rect.height*5/4;
		//rect.x = MAX(cx-rect.width/2, 0);
		//rect.y = MAX(cy-rect.height/2, 0);
		//rect.width = MIN(cx+rect.width/2-rect.x+1, image_w-rect.x+1);	
		//rect.height = MIN(cy+rect.height/2-rect.y+1, image_h-rect.y+1);
		image_resize_bilinear_roi(yuv420, temp, image_w, image_h, DST_WID, DST_HEI, &rect, (short *)buffer);
		//image_binary_otsu(temp, binary, DST_WID, DST_HEI);
		jmAdaptiveThreshold(temp, binary, buffer, DST_WID,
			DST_HEI, 0, 21, 3, THRESH_BINARY_INV, 1); 
		image_threshold(binary, temp, 1, 255, DST_WID, DST_HEI, NULL);
		//{
		//	Mat image = Mat(Size(DST_WID, DST_HEI), CV_8UC1, temp);
		//	vector<Vec3f> circles;

		//	HoughCircles( image, circles, CV_HOUGH_GRADIENT, 1, 10, 10, 25, DST_WID/3, DST_WID/2);

		//	for( size_t i = 0; i < circles.size(); i++ )
		//	{
		//		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));                                 
		//		int radius = cvRound(circles[i][2]);

		//		// circle outline
		//		circle( image, center, radius, Scalar(255,255,255), 3, 8, 0 );

		//	}
		//	imshow("hough", image);
		//	waitKey(1);
		//}
		ptSeq = FindContours(binary, DST_WID, DST_HEI, CONTOURS_LIST, &storage);
		for (ptTmp = ptSeq; ptTmp != NULL; ptTmp = ptTmp->h_next) 
		{
			CV_RECT sub_rect = BoundingRect(ptTmp);

			if ((sub_rect.height >= DST_HEI / 4) && (sub_rect.width >= DST_WID / 5) && (sub_rect.width <= DST_WID / 1.2) && \
				(sub_rect.width / (float)sub_rect.height >= 0.3) && (sub_rect.height / (float)sub_rect.width >= 0.5))  //****************
			//if (sub_rect.height >= DST_HEI/3 && sub_rect.width/(float)sub_rect.height >= 0.05 && sub_rect.width/(float)sub_rect.height <= 0.8)
			{

				//***************************************************************
				Mat _temp_img = Mat(sub_rect.height, sub_rect.width, CV_8UC1);
				for (int i = 0; i < _temp_img.rows; i++)
				{
					unsigned char *ptr = _temp_img.ptr(i);
					for (int j = 0; j < _temp_img.cols; j++)
					{
						ptr[j] = temp[(sub_rect.y + i)*DST_WID + sub_rect.x + j];
					}
				}

				Mat element = getStructuringElement(MORPH_RECT,
					Size(5,5),
					Point(2, 2));
				erode(_temp_img, _temp_img, element);

				namedWindow("temp", WINDOW_NORMAL);
				imshow("temp", _temp_img);
				int __V = _ann.predict(_temp_img);
				cout << "value:  " << __V << endl;
				waitKey();
				destroyAllWindows();
				//***************************************************************

				MerageRectYchannel(temp, DST_WID, DST_HEI, sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height, 255);
				rect_num++;
			}
			else
			{
				//MerageRectYchannel(temp, DST_WID, DST_HEI, sub_rect.x, sub_rect.y, sub_rect.width, sub_rect.height, 128);  //****************

			}
		}
		//namedWindow("mask", WINDOW_NORMAL);
		//ShowImageOpencv(temp, DST_WID, DST_HEI, 8, 1, "mask");	  waitKey(0);//***********************
		DestroySeq(ptSeq, &storage);
		if (rect_num > 0)
		{
			MerageRect(yuv420, image_w, image_h, rect.x, rect.y, rect.width, rect.height, 0, 255, 0);
		}
		//Save_Image(binary, DST_WID, DST_HEI, NULL);
	}
	return 0;
}