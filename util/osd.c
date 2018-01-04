
#include <stdlib.h>
#include <math.h>
#include "sthz16.h"
#include "osd.h"
#include <string.h>

#include <memory.h> // added for iOS by Wangliang

unsigned short y_rgb2yuv_coef1[256] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 64, 65, 65};
unsigned short y_rgb2yuv_coef2[256] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127, 127, 128};
unsigned short y_rgb2yuv_coef3[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24};
unsigned short u_rgb2yuv_coef1[256] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37};
unsigned short u_rgb2yuv_coef2[256] = {0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 71, 72, 72, 72, 73, 73, 73, 73, 74};
unsigned short u_rgb2yuv_coef3[256] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99, 100, 100, 101, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 105, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 111, 111, 112};
unsigned short v_rgb2yuv_coef1[256] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99, 100, 100, 101, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 105, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 111, 111, 112};
unsigned short v_rgb2yuv_coef2[256] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 93};
unsigned short v_rgb2yuv_coef3[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18};


#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#define RGB2YCbCr(R, G, B, Y, Cb, Cr)                                     \
{                                                                         \
	Y  = MIN(255, MAX(0, (y_rgb2yuv_coef1[R] + y_rgb2yuv_coef2[G] + y_rgb2yuv_coef3[B] + 16)));   \
	Cb = MIN(255, MAX(0, (-u_rgb2yuv_coef1[R] - u_rgb2yuv_coef2[G] + u_rgb2yuv_coef3[B] + 128))); \
	Cr = MIN(255, MAX(0, (v_rgb2yuv_coef1[R] - v_rgb2yuv_coef2[G] - v_rgb2yuv_coef3[B] + 128)));  \
}

//#define RGB2YCbCr(R, G, B, Y, Cb, Cr)                                     \
//{                                                                         \
//	Y  = MIN(255, MAX(0, (((R * 263 + G * 516 + B * 100) >> 10) + 16)));  \
//	Cb = MIN(255, MAX(0, (((-152 * R - 298 * G + 450 * B) >> 10) + 128)));\
//	Cr = MIN(255, MAX(0, (((450 * R - 377 * G - 73 * B) >> 10) + 128)));  \
//}

void RGB_to_YCbCr(int R, int G, int B, int *Y, int *Cb, int *Cr)
{																			
	*Y  = MIN(255, MAX(0, (y_rgb2yuv_coef1[R] + y_rgb2yuv_coef2[G] + y_rgb2yuv_coef3[B] + 16)));   
	*Cb = MIN(255, MAX(0, (-u_rgb2yuv_coef1[R] - u_rgb2yuv_coef2[G] + u_rgb2yuv_coef3[B] + 128))); 
	*Cr = MIN(255, MAX(0, (v_rgb2yuv_coef1[R] - v_rgb2yuv_coef2[G] - v_rgb2yuv_coef3[B] + 128)));  
}


/**=============================================================================
* @name： MerageStringEx
* auhthor：
* 日期： 2015-12-30, @ldw_v2.0.6
* 功能：叠加字符串至图片, 从8*16点阵字体缩放, 字体宽度可连续设置
* 		1. 暂使用最临近缩放法 @2015-12-30
*		2. 最小fontWidth适合为8(点阵宽高8*16), 行间隔固定为8，列间固定隔：0
* 输入参数：
* @param pYUV YUV420p(I420), NV21, NV12格式图片数据
* @param nPicWidth  图像宽度
* @param nPicHeight 图像高度
* @param start_x 字符串目标左上角坐标x
* @param start_y 字符串目标左上角坐标y
* @param fontWidth 字符宽度，半角字母，汉字实际宽度为2*fontWidth, 行高为2*fontWidth
* @param R 字符颜色
* @param G
* @param B
* @param eColorType EColorType: 已支持YUV420p(I420), NV21, NV12
* @return 0
* 修改记录：2015-12-30 by Xsky :
* 		1. 原函数名MerageString
* 		2. 功能改变：改为字体宽度整连续设置
*============================================================================*/
int MerageStringEx(unsigned char *pYUV,
				  int            nPicWidth,
				  int            nPicHeight,
				  unsigned char *Str,
				  int            start_x,
				  int            start_y,
				  int            fontWidth,
				  int            R,
				  int            G,
				  int            B,
				  int	 		 format)
{

	int32 i;
	//int32 i, k, m;
	uint8 bCode, bFlag;

	//int32 nPos, uvPos;
	const unsigned char *psAdd;// *psEndAdd;

	//uint8 nLum;
	int32 nFontHeight, nFontWidth;
	int32 nAreaHeight, nAreaWidth;
	int32 nCharSpace, nRowSpace;
	int32 nRows, nRowCount, nColCount;
	//int32 nWidthRatio, nHeightRatio;

	//uint8 *py, *pu, *pv;
	int32    yu, cb, cr;
	int32 dx, dy;

	int32 nStartPixelX = start_x, nStartPixelY = start_y;
	/*
	int slen;
	int textAreaWidth, textAreaHeight;
	int maxCharsPerLine;
	int maxCols, maxRows;
	int lineChars, lineWidth;
	*/
	int32 cur_line_x, cur_line_y;
	uint8 *py0, *pu0 = NULL, *pv0 = NULL;
	int32 yu00, cb00, cr00;

	int32 nCharW, nCharH;
	int32 nScaleX, nScaleY;

	/*
	py = pYUV;
	pu = py + nPicWidth * nPicHeight;
	pv = pu + ((nPicWidth * nPicHeight) >> 2);

	py = tempdata;
	pu = py + nPicWidth * nPicHeight;
	pv = pu + ((nPicWidth * nPicHeight) >> 2);
	*/

	if(pYUV == NULL) return 1;

	RGB2YCbCr(R, G, B, yu, cb, cr);
	RGB2YCbCr(0, 0, 0, yu00, cb00, cr00);

	nAreaWidth  = nPicWidth;
	nAreaHeight = nPicHeight;
	//nFontWidth  = MAX(16, fontWidth);
	nFontWidth  = 8;		// 字库为8x16
	nFontHeight = nFontWidth * 2;
	nCharSpace  = 0;
	nRowSpace   = 8;
	//nLum        = 0;

	nCharW = fontWidth;
	nCharH = nCharW * 2;
	nScaleX = (nFontWidth  << 10) / nCharW;
	nScaleY = (nFontHeight << 10) / nCharH;

	if (nStartPixelY + nAreaHeight > nPicHeight)
	{
		nAreaHeight = nPicHeight - nStartPixelY;
	}

	if (nStartPixelX + nAreaWidth > nPicWidth)
	{
		nAreaWidth = nPicWidth - nStartPixelX;
	}

	if (nAreaHeight - nCharH < nRowSpace)
	{
		nRows = 1;
	}
	else
	{
		nRows = nAreaHeight / (nCharH + nRowSpace) + (nAreaHeight % (nCharH + nRowSpace)) / nCharH;
	}

	py0 = pYUV;
	if(format == FORMAT_I420P){ 					// YUV420P:  YYYYYYYY UU VV
		pu0 = pYUV +   nPicWidth * nPicHeight;
		pv0 = pu0  + ((nPicWidth * nPicHeight) / 4);
	}else if (format == FORMAT_NV21){				// NV21: YYYYYYYY VUVU
		pv0 = pYUV + nPicWidth * nPicHeight;
		pu0 = pv0 + 1;
	}else if (format == FORMAT_NV12){				// NV21: YYYYYYYY UVUV
		pu0 = pYUV + nPicWidth * nPicHeight;
		pv0 = pu0 + 1;
	}else if (format == FORMAT_YV12){				// YV12: :  YYYYYYYY VV UU
		pv0 = pYUV +   nPicWidth * nPicHeight;
		pu0 = pv0  + ((nPicWidth * nPicHeight) / 4);
	}else {
	}

	cur_line_x = start_x;
	cur_line_y = start_y;

	i         = 0;
	nRowCount = 0;
	nColCount = 0;

	do
	{
		int wordWidth = nCharW;
		int font_step = 1;
		bCode = *(Str + i);

		//printf("i: %d\r\n", i);

		if (bCode < 0xa0)
		{
			if (bCode == 0)
			{
				break;
			}

			if (bCode == '\n')
			{
				nRowCount++;

				if (nRowCount >= nRows)
				{
					break;
				}

				nColCount = 0;
				i++;

				continue;
			}
			else
			{
				psAdd = &(ASC[bCode << 4]); // 16*8 点阵字体

				cur_line_y = start_y + nRowCount * (nCharH + nRowSpace);
				cur_line_x = (start_x + nColCount);

				nColCount += nCharW + nCharSpace;
			}
		}
		else
		{
			i++;

			psAdd = &(sthz[((bCode - 0xa0 - 1) * 94 + (*(Str + i) - 0xa0 - 1))
					* 32]); // 宽高：16*16
			wordWidth = nCharW * 2;
			font_step = 2;

			cur_line_y = start_y + nRowCount * (nCharH + nRowSpace);
			cur_line_x = (start_x + nColCount);

			nColCount += 2 * nCharW + nCharSpace;
		}

		bFlag = 0;

		for (dy = 0; dy < nCharH; dy++)
		{
			int txt_x;
			int txt_y;
			int font_x, font_y;
			int ft_y, ft_y1;
			int u, v;
			unsigned char *pu8Y, *pu8U = NULL, *pu8V = NULL;

			txt_y = cur_line_y + dy;
			if (txt_y >= nPicHeight) break;

			//printf("y:%03d,dy:%02d: ", txt_y, dy);

			pu8Y = py0 + (nPicWidth * txt_y);
			if (format == FORMAT_I420P)		//YUV420p
			{			// YUV420p:  YYYYYYYY UU VV
				pu8U = pu0 + (nPicWidth >> 1) * (txt_y >> 1);
				pu8V = pv0 + (nPicWidth >> 1) * (txt_y >> 1);
			}
			else if (format == FORMAT_NV21)
			{			// NV21: YYYYYYYY VUVU
				pu8V = pv0 + (nPicWidth) * (txt_y >> 1);
				pu8U = pu8V + 1;
			}
			else if (format == FORMAT_NV12)
			{			// NV21: YYYYYYYY VUVU
				pu8U = pu0 + (nPicWidth) * (txt_y >> 1);
				pu8V = pu8U + 1;
			}
			else if (format == FORMAT_YV12)
			{			// YV12:  :  YYYYYYYY VV UU
				pu8V = pv0 + (nPicWidth >> 1) * (txt_y >> 1);
				pu8U = pu0 + (nPicWidth >> 1) * (txt_y >> 1);
			}
			else
			{
			}

			//font_y = ( ( ( ((dy << 6) + (1 << 6 >> 1) ) * nScaleY ) >> 10 ) - (1 << 6 >> 1) ) >> 6;
			font_y = (((dy << 6) + (1 << 6 >> 1)) * nScaleY) >> 6;
			//font_y = ( ((dy << 6)  ) * nScaleY ) >> 6;
			ft_y = font_y >> 10;
			ft_y1 = ft_y + 1;
			if (ft_y1 >= nFontHeight) ft_y1 = ft_y;
			v = font_y & ((1 << 10) - 1);
			//txt_x = cur_line_x;
			for (dx = 0; dx < wordWidth; dx++)
			{
				#define GET_FONT_PIX(xx, yy)			( psAdd[(yy) * font_step + (xx)/8] & ( 1 << (7-((xx) % 8)) ) )
				//#define GET_FONT_PIX_YUV(xx, yy, yuv)	GET_FONT_PIX((xx), (yy)) != 0 ? (yuv) : 0

				//int n;
				//unsigned char font_val;
				int ft_x, ft_x1;
				int fontPixVal;
				//int fxy, fxy1, fx1y, fx1y1;
				//int Yu[4] = {0, }, Cb[4] = {0, }, Cr[4] = {0, };
				//int fyuv[4][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
				/*int fyuv[4][3] =
								{
								{ yu00, cb00, cr00 },
								{ yu00, cb00, cr00 },
								{ yu00, cb00, cr00 },
								{ yu00, cb00, cr00 } };
				*/
				//int dstY, dstU, dstV;
				//uint32 coef1, coef2, coef3, coef4;
				//int32 flg;

				txt_x = cur_line_x + dx;
				if (txt_x >= nPicWidth) break;

				//font_x = ( ( ( ((dx << 6) + (1 << 6 >> 1) ) * nScaleX ) >> 10 ) - (1 << 6 >> 1) ) >> 6;
				font_x = (((dx << 6) + (1 << 6 >> 1)) * nScaleX) >> 6;
				//font_x = ( ((dx << 6)  ) * nScaleX ) >> 6;
				ft_x = font_x >> 10;
				ft_x1 = ft_x + 1;
				if (ft_x1 >= nFontWidth) ft_x1 = ft_x;
				u = font_x & ((1 << 10) - 1);

				/*font_val = psAdd[y1 * font_step + x1/8];
				 n = x1 % 8;
				 fontPixVal = font_val & (1<<(7-n));*/

				/*
				 flg = 0;
				 fontPixVal = GET_FONT_PIX(ft_x, ft_y);
				 if (fontPixVal != 0)
				 {
				 fyuv[0][0] = yu;
				 fyuv[0][1] = cb;
				 fyuv[0][2] = cr;
				 //printf("%02d,%03d,%03d; ", dx, txt_x, txt_x >> 1);
				 //printf("%02d; ", dx, txt_x, txt_x >> 1);
				 }

				 fontPixVal = GET_FONT_PIX(ft_x1, ft_y);
				 if (fontPixVal != 0)
				 {
				 fyuv[1][0] = yu;
				 fyuv[1][1] = cb;
				 fyuv[1][2] = cr;
				 flg = 1;
				 }
				 fontPixVal = GET_FONT_PIX(ft_x, ft_y1);
				 if (fontPixVal != 0)
				 {
				 fyuv[2][0] = yu;
				 fyuv[2][1] = cb;
				 fyuv[2][2] = cr;
				 flg = 1;
				 }
				 fontPixVal = GET_FONT_PIX(ft_x1, ft_y1);
				 if (fontPixVal != 0)
				 {
				 fyuv[0][0] = yu;
				 fyuv[0][1] = cb;
				 fyuv[0][2] = cr;
				 flg = 1;
				 }

				 // 1 << 10 = 1024
				 coef1 = ((1024 - u) * (1024 - v));
				 coef2 = ((1024 - u) * v);
				 coef3 = (u * (1024 - v));
				 coef4 = (u * v);

				 dstY = (coef1 * fyuv[0][0] + coef2 * fyuv[1][0]
				 + coef3 * fyuv[2][0] + coef4 * fyuv[3][0]) >> 20;
				 dstU = (coef1 * fyuv[0][1] + coef2 * fyuv[1][1]
				 + coef3 * fyuv[2][1] + coef4 * fyuv[3][1]) >> 20;
				 dstV = (coef1 * fyuv[0][2] + coef2 * fyuv[1][2]
				 + coef3 * fyuv[2][2] + coef4 * fyuv[3][2]) >> 20;
				 */
				/*
				 //if( dstY != 0 || dstU != 0 || dstV != 0)
				 if( flg == 1 )
				 {
				 pu8Y[txt_x]			= dstY;
				 if( (txt_y & 0x01) == 0  && (txt_x && 0x01) == 0 ){
				 pu8U[txt_x >> 1]	= dstU;
				 pu8V[txt_x >> 1]	= dstV;
				 }
				 //py[txt_x]		= fyuv[0][0];
				 //pu[txt_x >> 1]	= fyuv[0][1];
				 //pv[txt_x >> 1]	= fyuv[0][2];
				 }
				 */

				fontPixVal = GET_FONT_PIX(ft_x, ft_y);
				if (fontPixVal != 0)
				{
					pu8Y[txt_x] = yu; // fyuv[0][0];
					//if ( (txt_y % 1) == 0 && (txt_x % 1) == 0 )
					{
						if (format == FORMAT_I420P)	  // YUV420p
						{
								int uv_x = txt_x >> 1;
								pu8U[uv_x] 		= cb; // fyuv[0][1];
								pu8V[uv_x] 		= cr; // fyuv[0][2];
						}
						else if (format == FORMAT_NV21)
						{		// NV21: YYYYYYYY VUVU
							if ( (txt_x % 2) == 0 ) {
								pu8V[txt_x] 	 = cr;
								pu8U[txt_x] 	 = cb;		// 注意txt_x的值
							}
						}
						else if (format == FORMAT_NV12)
						{		// NV21: YYYYYYYY VUVU
							if ( (txt_x % 2) == 0 ) {
								pu8U[txt_x] 	 = cb;
								pu8V[txt_x] 	 = cr;		// 注意txt_x的值
							}
						}
						else if (format == FORMAT_YV12)	  // YV12
						{		// YV12:  :  YYYYYYYY VV UU
								int uv_x = txt_x >> 1;
								pu8V[uv_x] 		= cr;
								pu8U[uv_x] 		= cb;
						}
						else
						{
						}
					}
				}

				//txt_x++;
			}
			//printf(".\r\n");
		}
		//cur_line_x += wordWidth;

		if (nColCount >= nAreaWidth || nColCount >= nPicWidth)
		{
			nRowCount++;
			cur_line_y = start_y + nRowCount * (nCharH + nRowSpace);

			if (nRowCount >= nRows)
			{
				break;
			}

			nColCount = 0;
			cur_line_x = start_x;
		}

		i++;

	} while (1);

	return ERR_OK;

}

int MerageString(unsigned char *pYUV, 
				  int            nPicWidth, 
				  int            nPicHeight, 
				  unsigned char *Str, 
				  int            nStartPixelX, 
				  int            nStartPixelY,
				  int            fontWidth,
				  int            R,
				  int            G,
				  int            B)
{
     int32 i, k, m;
	 uint8 bCode, bFlag;
	
	 int32 nPos, uvPos;
	 const unsigned char *psAdd, *psEndAdd;
     
	 uint8 nLum;
	 int32 nFontHeight, nFontWidth;
	 int32 nAreaHeight, nAreaWidth;
	 int32 nCharSpace, nRowSpace;
	 int32 nRows, nRowCount, nColCount;
	 int32 nWidthRatio, nHeightRatio;

	 uint8 *py, *pu, *pv;
	 int    yu, cb, cr;

	 py = pYUV;
	 pu = py + nPicWidth * nPicHeight;
	 pv = pu + ((nPicWidth * nPicHeight) >> 2);

     RGB2YCbCr(R, G, B, yu, cb, cr);  

	 nAreaWidth  = nPicWidth;
	 nAreaHeight = nPicHeight;
	 nFontWidth  = MAX(16, fontWidth);
	 nFontHeight = nFontWidth * 2;
	 nCharSpace  = 0; 
	 nRowSpace   = 8; 
	 nLum        = 0; 

	 if (nStartPixelY + nAreaHeight > nPicHeight)
	 {
	     nAreaHeight = nPicHeight - nStartPixelY;
	 }

	 if (nStartPixelX + nAreaWidth > nPicWidth)
	 {
	     nAreaWidth = nPicWidth - nStartPixelX;
	 }
 
     if (nAreaHeight - nFontHeight < nRowSpace)
	 {
         nRows = 1;
	 }
	 else
	 {
		 nRows = nAreaHeight / (nFontHeight + nRowSpace) + (nAreaHeight % (nFontHeight + nRowSpace)) / nFontHeight;
	 }
 
	 i         = 0;
	 nRowCount = 0;
	 nColCount = 0;

	 do 
	 { 
		 bCode = *(Str + i);
		 
		 if (bCode < 0xa0)
		 {   
			 if(bCode == 0)  break;

			 if( bCode == '\n' ) 
			 {
                 nRowCount++;

				 if (nRowCount >= nRows)
				 {
				     break;
				 }

				 nColCount = 0;
				 i++;

				 continue;
			 }
			 else
			 {
                 psAdd = &(ASC[bCode << 4]);

				 psEndAdd     = psAdd + 16;
				 nWidthRatio  = nFontWidth / 8;
				 nHeightRatio = nFontHeight / 16;

				 nPos  = (nStartPixelY + nRowCount * (nFontHeight + nRowSpace)) * nPicWidth + (nStartPixelX + nColCount);
				 uvPos = ((nStartPixelY + nRowCount * (nFontHeight + nRowSpace))>>1) * (nPicWidth>>1) + ((nStartPixelX + nColCount)>>1); 
				 
				 nColCount += nFontWidth + nCharSpace;
			 }
		 }
		 else 
		 {    
			  i++;

			  psAdd = &(sthz[((bCode-0xa0-1)*94+(*(Str+i)-0xa0-1))*32]);

			  psEndAdd     = psAdd + 32;
			  nWidthRatio  = 2 * nFontWidth / 16;
			  nHeightRatio = nFontHeight / 16;

		      nPos  = (nStartPixelY + nRowCount * (nFontHeight + nRowSpace)) * nPicWidth + (nStartPixelX + nColCount);
			  uvPos = ((nStartPixelY + nRowCount * (nFontHeight + nRowSpace))>>1) * (nPicWidth>>1) + ((nStartPixelX + nColCount)>>1); 
		      
			  nColCount += 2 * nFontWidth + nCharSpace;
		 }

		 bFlag = 0;

		 while (psAdd < psEndAdd)
		 {
			 int n;

			 for(n = 0; n < 8; n++)
			 {
				 if( ((*psAdd) & (1<<(7-n))) !=0)
				 {
					 for(m = 0; m < nHeightRatio; m++)
					 {  
						 for(k = 0; k < nWidthRatio; k++)
						 {
							 py[m * nPicWidth + k + nPos]                 = yu;
							 pu[(m>>1) * (nPicWidth>>1) + (k>>1) + uvPos] = cb;
							 pv[(m>>1) * (nPicWidth>>1) + (k>>1) + uvPos] = cr;
						 }
					 }
				 }

				 nPos  += nWidthRatio;
				 uvPos += (nWidthRatio >> 1);
			 }
            
			 psAdd++;	
			 bFlag++;

             if (bCode < 0xa0)
			 {
			     nPos  = nPos - nFontWidth + nPicWidth * nHeightRatio;
                 uvPos = uvPos - (nFontWidth>>1) + ((nPicWidth * nHeightRatio)>>2); 
			 }
			 else if ((bCode > 0xa0) && (bFlag == 2))
			 {
			     nPos  = nPos - 2 * nFontWidth + nPicWidth * nHeightRatio;
                 uvPos = uvPos - nFontWidth + ((nPicWidth * nHeightRatio)>>2); 

				 bFlag=0;
			 }
		 }
       
		 if (nColCount >= nAreaWidth)
		 {
			 nRowCount++;

			 if (nRowCount >= nRows)
			 {
			    break;
			 }

			 nColCount = 0;
		 }

		 i++;
		 
	 } while(1);
	
     return ERR_OK;
}

void MerageRect(unsigned char *pImage, 
				int            nSrcWidth, 
				int            nSrcHeight, 
				int            nRectStartX,
				int            nRectStartY,
				int            nRectWidth, 
				int            nRectHeight,
				int            R,
				int            G,
				int            B)
{
	int i, w, h, imgsize;
	int yu, cb, cr;
	int x_min, x_max;
	int y_min, y_max;

	unsigned char *pty, *ptu, *ptv;

	w = nSrcWidth;
	h = nSrcHeight;
	imgsize = w * h;

	//矩形框的线条宽度为3，需要限制一下，否则可能越界
	x_min = MAX(nRectStartX, 5);
	x_max = MIN(nRectStartX + nRectWidth - 1, nSrcWidth - 5);
	y_min = MAX(nRectStartY, 5);
	y_max = MIN(nRectStartY + nRectHeight - 1, nSrcHeight - 5);

	if (x_min >= x_max || y_min >= y_max)
	{
		return ;
	}
    RGB2YCbCr(R, G, B, yu, cb, cr);  

	//Y分量	
	//y top
	pty = pImage + w * y_min + x_min;
    memset(pty, yu, x_max - x_min + 1);

	pty -= w;
    memset(pty, yu, x_max - x_min + 1);

	pty += 2 * w;
	memset(pty, yu, x_max - x_min + 1);

	//y left
	pty = pImage + w * (y_min - 1) + (x_min - 1);

	for (i = 0; i < y_max - y_min + 3; i++)
	{
        pty[0] = yu;
        pty[1] = yu;
        pty[2] = yu;

		pty += w;
	}

	//y bottom
	pty = pImage + w * y_max + x_min;
    memset(pty, yu, x_max - x_min + 1);

	pty -= w;
	memset(pty, yu, x_max - x_min + 1);

	pty += 2 * w;
	memset(pty, yu, x_max - x_min + 1);


	//y right
	pty = pImage + w * (y_min - 1) + (x_max - 1);

	for (i = 0; i < y_max - y_min + 3; i++)
	{
		pty[0] = yu;
		pty[1] = yu;
		pty[2] = yu;

		pty += w;
	}

	//U、V分量
	x_min = (x_min >> 1);
	x_max = (x_max >> 1);
	y_min = (y_min >> 1);
	y_max = (y_max >> 1);
	w     = (w >> 1);
	h     = (h >> 1);

	//u v top
	ptu = pImage + imgsize + w * y_min + x_min;
	ptv = pImage + imgsize * 5 / 4 + w * y_min + x_min;
	memset(ptu, cb, x_max - x_min + 1);
	memset(ptv, cr, x_max - x_min + 1);

	//u v left
	for (i = 0; i < y_max - y_min + 1; i++)
	{
        ptu[i] = cb;  ptu += w - 1;
        ptv[i] = cr;  ptv += w - 1;
	}

	//u v bottom
	ptu = pImage + imgsize + w * y_max + x_min;
	ptv = pImage + imgsize*5/4 + w * y_max + x_min;
	memset(ptu, cb, x_max - x_min + 1);
	memset(ptv, cr, x_max - x_min + 1);

	//u v right
	ptu = pImage + imgsize + w * y_min + x_max;
	ptv = pImage + imgsize*5/4 + w * y_min + x_max;

	for (i = 0; i < y_max - y_min + 1; i++)
	{
		ptu[i] = cb;  ptu += w - 1;
		ptv[i] = cr;  ptv += w - 1;
	}

	return;
}

void MerageRectYchannel(unsigned char *pImageY, 
						int            nWidth, 
						int            nHeight, 
						int            nRectStartX,
						int            nRectStartY,
						int            nRectWidth, 
						int            nRectHeight,
						int            yVal)
{
	int i, w, h, imgsize;
	int x_min, x_max;
	int y_min, y_max;
	unsigned char *pty;

	w = nWidth;
	h = nHeight;
	imgsize = w * h;

	////矩形框的线条宽度为3，需要限制一下，否则可能越界
	//x_min = MAX(nRectStartX, 3);
	//x_max = MIN(nRectStartX + nRectWidth - 1, nWidth - 4);
	//y_min = MAX(nRectStartY, 3);
	//y_max = MIN(nRectStartY + nRectHeight - 1, nHeight - 4);


	////Y分量	
	////y top
	//pty = pImageY + w * y_min+x_min;
	//for (i=-3; i<x_max-x_min+4; i++)
	//{
	//	pty[(i-0*w)] = yVal;
	//	pty[(i-1*w)] = yVal;
	//	pty[(i-2*w)] = yVal;
	//	pty[(i-3*w)] = yVal;
	//}

	////y left
	//for (i=0; i<y_max-y_min; i++)
	//{
	//	pty[i*w-0] = yVal;
	//	pty[i*w-1] = yVal;
	//	pty[i*w-2] = yVal;
	//	pty[i*w-3] = yVal;
	//}

	////y bottom
	//pty = pImageY + w * y_max + x_min;
	//for (i=-3; i<x_max-x_min+4; i++)
	//{
	//	pty[(i+0*w)] = yVal;
	//	pty[(i+1*w)] = yVal;
	//	pty[(i+2*w)] = yVal;
	//	pty[(i+3*w)] = yVal;
	//}    

	////y right
	//pty = pImageY + w * y_min + x_max;
	//for (i=0; i<y_max-y_min; i++)
	//{
	//	pty[i*w+0] = yVal;
	//	pty[i*w+1] = yVal;
	//	pty[i*w+2] = yVal;
	//	pty[i*w+3] = yVal;
	//}


	//矩形框的线条宽度为3，需要限制一下，否则可能越界
	x_min = MAX(nRectStartX, 0);
	x_max = MIN(nRectStartX + nRectWidth - 1, nWidth - 1);
	y_min = MAX(nRectStartY, 0);
	y_max = MIN(nRectStartY + nRectHeight - 1, nHeight - 1);


	//Y分量	
	//y top
	pty = pImageY + w * y_min+x_min;
	for (i=0; i<x_max-x_min; i++)
	{
		pty[i] = yVal;
	}

	//y left
	for (i=0; i<y_max-y_min; i++)
	{
		pty[i*w] = yVal;
	}

	//y bottom
	pty = pImageY + w * y_max + x_min;
	for (i=0; i<x_max-x_min; i++)
	{
		pty[i] = yVal;
	}    

	//y right
	pty = pImageY + w * y_min + x_max;
	for (i=0; i<y_max-y_min; i++)
	{
		pty[i*w] = yVal;
	}
}

void MarkPoint(unsigned char *pImage, 
			   int            nWidth, 
			   int            nHeight, 
			   int            X,
			   int            Y,
			   int            R,
			   int            G,
			   int            B)
{
	int i, w, h, imgsize;
	int x_min, x_max;
	int y_min, y_max;
	unsigned char *pty, *ptu, *ptv;
	int y,cb,cr;

	unsigned char *pImageY;
	unsigned char *pImageU;
	unsigned char *pImageV;

    RGB2YCbCr(R, G, B, y, cb, cr);  

	w = nWidth;
	h = nHeight;
	imgsize = w * h;

	//矩形框的线条宽度为3，需要限制一下，否则可能越界
	x_min = MIN(MAX(X-1, 3), nWidth-4);
	x_max = MIN(MAX(X+1, 3), nWidth-4);
	y_min = MIN(MAX(Y-1, 3), nWidth-4);
	y_max = MIN(MAX(Y+1, 3), nWidth-4);

	pImageY = pImage;
    pImageU = pImageY + nWidth * nHeight;
    pImageV = pImageU + ((nWidth * nHeight) >> 2);
  

	//Y分量	
	//y top
	pty = pImageY + w * y_min+x_min;
	for (i=-3; i<x_max-x_min+4; i++)
	{
		pty[(i-0*w)] = y;
		pty[(i-1*w)] = y;
		pty[(i-2*w)] = y;
		pty[(i-3*w)] = y;
	}

	//y left
	for (i=0; i<y_max-y_min; i++)
	{
		pty[i*w-0] = y;
		pty[i*w-1] = y;
		pty[i*w-2] = y;
		pty[i*w-3] = y;
	}

	//y bottom
	pty = pImageY + w * y_max + x_min;
	for (i=-3; i<x_max-x_min+4; i++)
	{
		pty[(i+0*w)] = y;
		pty[(i+1*w)] = y;
		pty[(i+2*w)] = y;
		pty[(i+3*w)] = y;
	}  

	//y right
	pty = pImageY + w * y_min + x_max;
	for (i=0; i<y_max-y_min; i++)
	{
		pty[i*w+0] = y;
		pty[i*w+1] = y;
		pty[i*w+2] = y;
		pty[i*w+3] = y;
	}

	//U、V分量
	x_min = (x_min >> 1);
	x_max = (x_max >> 1);
	y_min = (y_min >> 1);
	y_max = (y_max >> 1);
	w     = (w >> 1);
	h     = (h >> 1);

	//u v top
	ptu = pImageU + w * y_min + x_min;
	ptv = pImageV + w * y_min + x_min;
	for (i=-1; i<x_max-x_min+2; i++)
	{
		ptu[i-0*w] = cb;
		ptv[i-0*w] = cr;
		ptu[i-1*w] = cb;
		ptv[i-1*w] = cr;
	}

	//u v left
	for (i=0; i<y_max-y_min; i++)
	{
		ptu[i*w-0] = cb;
		ptv[i*w-0] = cr;
		ptu[i*w-1] = cb;
		ptv[i*w-1] = cr;
	}

	//u v bottom
	ptu = pImageU + w * y_max + x_min;
	ptv = pImageV + w * y_max + x_min;
	for (i=-1; i<x_max-x_min+2; i++)
	{
		ptu[i+0*w] = cb;
		ptv[i+0*w] = cr;
		ptu[i+1*w] = cb;
		ptv[i+1*w] = cr;
	} 

	//u v right
	ptu = pImageU + w * y_min + x_max;
	ptv = pImageV + w * y_min + x_max;
	for (i=0; i<y_max-y_min; i++)
	{
		ptu[i*w+0] = cb;
		ptv[i*w+0] = cr;
		ptu[i*w+1] = cb;
		ptv[i*w+1] = cr;
	}

	return;
}

void MerageLine(unsigned char *pyuv420, int image_w, int image_h, int x1, int y1, int x2, int y2, int thickness, unsigned char r, unsigned char g, unsigned char b) 
{    
	int x, x0, interchange;   
	int y, y0, temp;   
	int s1, s2, i;   
	int e;  

	unsigned char  yu, cb, cr;
	unsigned char *py, *pu, *pv;

	if (x1 == x2 && y1 == y2)
	{
		return;
	}

	thickness = MIN(10, MAX(1, thickness));

	py = pyuv420;
    pu = pyuv420 + image_w * image_h;
    pv = pu + image_w * image_h / 4;

	RGB2YCbCr(r, g, b, yu, cb, cr);  

	x  = x1;   
	y  = y1;    
	x0 = abs(x2 - x1);   
	y0 = abs(y2 - y1);     

	if ((x2 - x1) > 0)
	{
		s1 = 1;   
	}
	else
	{
		s1 = -1;  
	}

	if ((y2 - y1) > 0) 
	{
		s2 = 1; 
	} 
	else 
	{
		s2 = -1;  
	}

	if (y0 > x0)     
	{          
		temp = x0;         
		x0   = y0;         
		y0   = temp; 

		interchange = 1;     
	}    
	else 
	{
		interchange = 0;   
	}

	e = 2 * (y0 - x0);    

	for (i = 0; i <= x0; i++) 
	{
		int outside = 0;
		if(y < 0 || y >= image_h || x < 0 || x >= image_w)
		{
			// 防止越界，add by Xsky 2015-12-19
			outside = 1;
		}

		if(outside == 0)
		{
			if (thickness == 1)
			{
				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;
			}
			else if (thickness == 2)
			{
				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;

				if (x + 1 < image_w)
				{
					py[y * image_w + (x + 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cr;
				}
				else if (x - 1 >= 0)
				{
					py[y * image_w + (x - 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cr;
				}
			}
			else if (thickness == 3)
			{
				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;

				if (x + 1 < image_w)
				{
					py[y * image_w + (x + 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cr;
				}

				if(x - 1 >= 0)
				{
					py[y * image_w + (x - 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cr;
				}
			}
			else if (thickness == 4)
			{
				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;

				if (x + 1 < image_w)
				{
					py[y * image_w + (x + 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cr;
				}

				if(x - 1 >= 0)
				{
					py[y * image_w + (x - 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cr;
				}

				if(x - 2 >= 0)
				{
					py[y * image_w + (x - 2)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 2) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 2) >> 1)] = cr;
				}
			}
			else if (thickness == 5)
			{
				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;

				if (x + 1 < image_w)
				{
					py[y * image_w + (x + 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x + 1) >> 1)] = cr;
				}

				if (x + 2 < image_w)
				{
					py[y * image_w + (x + 2)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x + 2) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x + 2) >> 1)] = cr;
				}

				if(x - 1 >= 0)
				{
					py[y * image_w + (x - 1)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 1) >> 1)] = cr;
				}

				if(x - 2 >= 0)
				{
					py[y * image_w + (x - 2)]                      = yu;
					pu[(y >> 1) * (image_w >> 1) + ((x - 2) >> 1)] = cb;
					pv[(y >> 1) * (image_w >> 1) + ((x - 2) >> 1)] = cr;
				}
			}
			else
			{
				int kk = 0;

				py[y * image_w + x]                      = yu;
				pu[(y >> 1) * (image_w >> 1) + (x >> 1)] = cb;
				pv[(y >> 1) * (image_w >> 1) + (x >> 1)] = cr;

				for (kk = 1; kk < 10; ++kk)
				{
					if (x + kk < image_w)
					{
						py[y * image_w + (x + kk)]                      = yu;
						pu[(y >> 1) * (image_w >> 1) + ((x + kk) >> 1)] = cb;
						pv[(y >> 1) * (image_w >> 1) + ((x + kk) >> 1)] = cr;
					}
				}

				for (kk = -9; kk < 0; ++kk)
				{
					if (x + kk >= 0)
					{
						py[y * image_w + (x + kk)]                      = yu;
						pu[(y >> 1) * (image_w >> 1) + ((x + kk) >> 1)] = cb;
						pv[(y >> 1) * (image_w >> 1) + ((x + kk) >> 1)] = cr;
					}
				}
			}

		}

		while (e >= 0)            
		{         
			if (interchange == 1)
			{
				x = x + s1;
			}
			else
			{
				y = y + s2;  
			}

			e = e - 2 * x0;            
		}  

		if(interchange == 1)
		{
			y = y + s2;   
		}
		else 
		{
			x = x + s1;      
		}

		e = e + 2 * y0;        
	} 

	return;
}

