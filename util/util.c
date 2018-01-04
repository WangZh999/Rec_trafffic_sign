
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "util.h"
#include <string.h>

#ifndef WIN32
#ifdef LOCAL_ARM_NEON
#include <arm_neon.h>
#endif
#include <omp.h>
#endif



//定点开方运算
//入口参数：被开方数，长整型; 出口参数：开方结果，整型 
unsigned int fix_sqrt_16(unsigned int M)
{
	unsigned int N, i;
	unsigned int tmp, ttp;   // 结果、循环计数

	if (M == 0)               // 被开方数，开方结果也为0
	{
		return 0;
	}

	N = 0;

	tmp = (M >> 30);          // 获取最高位：B[m-1]
	M <<= 2;

	if (tmp > 1)              // 最高位为1
	{
		N ++;                 // 结果当前位为1，否则为默认的0
		tmp -= N;
	}

	for (i = 15; i > 0; i--)  // 求剩余的15位
	{
		N <<= 1;              // 左移一位

		tmp <<= 2;
		tmp  += (M >> 30);     // 假设

		ttp = N;
		ttp = (ttp << 1) + 1;

		M <<= 2;

		if (tmp >= ttp)       // 假设成立
		{
			tmp -= ttp;
			N ++;
		}
	}

	return N;
}

//将RGB图像转换为灰度图
void rgb_to_gray(PBYTE p_rgb_img, PBYTE p_gray_img, int image_w, int image_h)
{
	int i, j;
	unsigned char *p_r, *p_g, *p_b, *p_gray;

	p_b    = p_rgb_img;
	p_g    = p_rgb_img + 1;
	p_r    = p_rgb_img + 2;
	p_gray = p_gray_img;

	for (i = 0; i < image_h; ++i)
	{
		for (j = 0; j < image_w; ++j)
		{
			*p_gray++ = (((*p_r) * 19595 + (*p_g) * 38469 + (*p_b) * 7472) >> 16);

			p_r += 3;
			p_g += 3;
			p_b += 3;
		}
	}

	return;
}

//获取子区域图像数据
void get_subimg_data(PBYTE p_gray_img, PBYTE p_subimg, CV_BLOB sub_rect, int image_w, int image_h)
{
	int i;
	int copy_num, copy_bytes_once;
	unsigned char *p_org, *p_dst;

	copy_num = MIN(sub_rect.bottom - sub_rect.top + 1, image_h);
    copy_bytes_once = sub_rect.right - sub_rect.left + 1;

	p_org = p_gray_img + sub_rect.top * image_w + sub_rect.left;
	p_dst = p_subimg;    

	for (i = 0; i < copy_num; ++i)
	{
		memcpy(p_dst, p_org, copy_bytes_once);

		p_org += image_w;
		p_dst += copy_bytes_once;
	}

	return;
}

#if 0
//对二值图像进行形态学膨胀腐蚀操作
void dilation_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height)
{
	int           i, j, m;
	int           sum;
	unsigned char *dst, *src;
	unsigned char *p1, *p2, *p3;

	memset(dst_img, 0, width * height);

	src  = src_img  + width;
	dst  = dst_img  + width;

	//分别指向5行图像数据
	p1 = src - width;
	p2 = p1 + width;
	p3 = p2 + width;

	for (i = 1; i < height - 1; i ++)
	{
		for (j = 1; j < width - 1; j ++)
		{
			//累加
			sum = 0;

			for (m = 0; m < 3; m++)
			{
				sum += (*p1++) + (*p2++) + (*p3++);
			}

			p1 -= 2;
			p2 -= 2;
			p3 -= 2;

			//与设定阈值进行比较
			dst[j] = (sum > 3 * 255) ? 255 : 0;
		}

		p1 += 2;
		p2 += 2;
		p3 += 2;

		src  += width;
		dst  += width;
	}

	return;
}
#endif

void dilation_filter_5x5(unsigned char *src_img, 
						 unsigned char *dst_img, 
						 int           width, 
						 int           height)
{
	int           i, j, m;
	int           sum;
	unsigned char *dst, *src;
	unsigned char *p1, *p2, *p3, *p4, *p5;

	memset(dst_img, 0, width * height);

	//计算起始点位置
	src = src_img + (width << 1);
	dst = dst_img + (width << 1);

	//分别指向5行图像数据
	p1 = src - (width << 1);
	p2 = p1 + width;
	p3 = p2 + width;
	p4 = p3 + width;
	p5 = p4 + width;

	//对每个像素点进行滤波处理
	for (i = 2; i < height-2; i ++)
	{
		for (j = 2; j < width - 2; j ++)
		{
			//累加
			sum = 0;

			for (m = 0; m < 5; m++)
			{
				sum += (*p1++) + (*p2++) + (*p3++) + (*p4++) + (*p5++);
			}

			p1 -= 4;
			p2 -= 4;
			p3 -= 4;
			p4 -= 4;
			p5 -= 4;

			//与设定阈值进行比较
			dst[j] = (sum >= 3 * 255) ? 255 : 0;
		}

		p1 += 4;
		p2 += 4;
		p3 += 4;
		p4 += 4;
		p5 += 4;

		src += width;
		dst += width;
	}

	return;
}

void erosion_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi)
{
	int begin_x, end_x, begin_y, end_y;
	int y;

	if (src_img == NULL || dst_img == NULL)
	{
		return ;
	}

	if (roi != NULL)
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(width-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(height-2, roi->y+roi->height);
	}
	else
	{
		begin_x = 1;
		end_x = width-2;
		begin_y = 1;
		end_y = height-2;
	}

#pragma omp parallel for  schedule(dynamic)	
	for (y = begin_y; y <= end_y; y++)
	{
		int sum = 0;
		PBYTE s_ptr1,s_ptr2,s_ptr3;
		PBYTE dst;
		int x;
		s_ptr1 = src_img+(y-1)*width+begin_x;
		s_ptr2 = src_img+y*width+begin_x;
		s_ptr3 = src_img+(y+1)*width+begin_x;
		dst = dst_img+y*width+begin_x;
		for (x = begin_x; x <= end_x; x++)
		{
			sum = (*s_ptr1+*s_ptr2+*s_ptr3);
			if (sum == 765)
			{
				sum = (*(s_ptr2-1)+*s_ptr2+*(s_ptr2+1));
				if (sum == 765)
				{
					*dst++ = 255;
				}
				else
				{
					*dst++ = 0;
				}
			}
			else
			{
				*dst++ = 0;
			}
			s_ptr1++;
			s_ptr2++;
			s_ptr3++;
		}
	}
	return ;
}

void dilation_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi)
{
	int begin_x, end_x, begin_y, end_y;
	int y;

	if (src_img == NULL || dst_img == NULL)
	{
		return ;
	}

	if (roi != NULL)
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(width-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(height-2, roi->y+roi->height);
	}
	else
	{
		begin_x = 1;
		end_x = width-2;
		begin_y = 1;
		end_y = height-2;
	}
#pragma omp parallel for  schedule(dynamic)	
	for (y = begin_y; y <= end_y; y++)
	{
		int sum = 0;
		PBYTE s_ptr1,s_ptr2,s_ptr3;
		PBYTE dst;
		int x;
		s_ptr1 = src_img+(y-1)*width+begin_x;
		s_ptr2 = src_img+y*width+begin_x;
		s_ptr3 = src_img+(y+1)*width+begin_x;
		dst = dst_img+y*width+begin_x;
		for (x = begin_x; x <= end_x; x++)
		{
			sum = (*s_ptr1+*s_ptr2+*s_ptr3);
			if (sum >= 255)
			{
				*dst++ = 255;
				
			}
			else
			{
				sum = (*(s_ptr2-1)+*s_ptr2+*(s_ptr2+1));
				if (sum >= 255)
				{
					*dst++ = 255;
				}
				else
				{
					*dst++ = 0;
				}
			}
			s_ptr1++;
			s_ptr2++;
			s_ptr3++;
		}
	}
	return ;
}

void horizontal_grad(unsigned char *p_src_img, 
					 unsigned char *p_grad_img,
					 int            image_w,
					 int            image_h,
					 int            top_pos,
					 int            bottom_pos,
					 unsigned char  threshold,
					 unsigned char  binary_val)
{
	// 按如下方式设置各个指针的指向

	//   P1
	//    |
	//    | 
	//  grad
	//    |
	//    |
	//   P6
	//初始化

//#if 0
#ifdef LOCAL_ARM_NEON
	int i = 0;
	unsigned char thresholds[16];
	memset(thresholds, threshold, sizeof(thresholds));
	uint8x16_t threshold_t = vld1q_u8(thresholds);
	uint8x16_t bin_val_t = vdupq_n_u8(binary_val);

	//memset(p_grad_img, 0, image_w * image_h);
	top_pos = MAX(top_pos, 0);
	bottom_pos = MIN(bottom_pos, image_h - 2);

#pragma omp parallel for
	for (i = top_pos; i < bottom_pos; i++)
	{
		unsigned char *p1 = p_src_img + i * image_w;
		unsigned char *p6 = p1 + 2 * image_w;
		unsigned char *p_grad = p_grad_img + (i + 1) * image_w;
		int j = 0;
		for (; j < image_w - 16; j += 16)
		{
			uint8x16_t upline_t = vld1q_u8(p1);
			uint8x16_t dwline_t = vld1q_u8(p6);

			uint8x16_t grad_t = vabdq_u8(upline_t, dwline_t);		// r[i] = | upline_t[i] - dwline_t[i] |;
			//grad_t = vcgeq_u8(grad_t, threshold_t);				// r[i] = grad_t[i] >= threshold_t[i] ? 1 : 0;
			grad_t = vcgtq_u8(grad_t, threshold_t);					// r[i] = grad_t[i] >  threshold_t[i] ? 1 : 0;
			grad_t = vandq_u8(grad_t, bin_val_t);					// r[i] = grad_t[i] &  threshold_t[i];
			vst1q_u8(p_grad, grad_t);

			p1 += 16;
			p6 += 16;
			p_grad += 16;
		}
		for (; j < image_w; j++)
		{
			*p_grad = ABS(*p1 - *p6);
			*p_grad = (*p_grad > threshold) ? binary_val : 0;
			p1++;
			p6++;
			p_grad++;
		}
	}

#else

	int i;

	memset(p_grad_img, 0, image_w * image_h);
	top_pos    = MAX(top_pos, 0);
	bottom_pos = MIN(bottom_pos, image_h-2);

#pragma omp parallel for
	for(i = top_pos; i < bottom_pos; i++)
	{
		unsigned char *p1 = p_src_img + i * image_w;
		unsigned char *p6 = p1 + 2 * image_w;
		unsigned char *p_grad = p_grad_img + (i + 1) * image_w;
		int j = 0;
		for(j = 0; j < image_w; j++)
		{
			*p_grad = ABS((*p1) - (*p6));
			*p_grad = (*p_grad > threshold) ? binary_val : 0;

			p1++;
			p6++;
			p_grad++;
		}
	}
#endif
	return;
}

void verticle_grad(unsigned char *p_src_img, 
				   unsigned char *p_grad_img,
				   int            image_w,
				   int            image_h,
				   int            top_pos,
				   int            bottom_pos,
				   unsigned char  threshold,
				   unsigned char  binary_val)
{
	// 按如下方式设置各个指针的指向
	//   P3----grad-----P4 
#ifdef LOCAL_ARM_NEON
	unsigned char thresholds[16];
	int i = 0;
	uint8x16_t bin_val_t = vdupq_n_u8(binary_val);

	memset(thresholds, threshold, sizeof(thresholds));
	uint8x16_t threshold_t = vld1q_u8(thresholds);

	top_pos    = MAX(top_pos, 1);
	bottom_pos = MIN(bottom_pos, image_h-1);
	//memset(p_grad_img, 0, image_w * image_h);

#pragma omp parallel for
	for(i = top_pos; i < bottom_pos; i++)
	{
		unsigned char *p3 = p_src_img + i * image_w;
		unsigned char *p4 = p3 + 2;
		unsigned char *p_grad = p_grad_img + i * image_w + 1;
		int j = 0;

		for (; j < image_w-18; j += 16)
		{
			uint8x16_t upline_t = vld1q_u8(p3);
			uint8x16_t dwline_t = vld1q_u8(p4);

			uint8x16_t grad_t = vabdq_u8(upline_t, dwline_t);
			grad_t = vcgeq_u8(grad_t, threshold_t);
			grad_t = vandq_u8(grad_t, bin_val_t);					// r[i] = grad_t[i] &  threshold_t[i];
			vst1q_u8(p_grad, grad_t);
			p3 += 16;
			p4 += 16;
			p_grad += 16;
		}
		for (; j < image_w-2; j++)
		{
			*p_grad = ABS(*p3 - *p4);
			*p_grad = (*p_grad > threshold) ? binary_val : 0;
			p3++;
			p4++;
			p_grad++;
		}
	}
#else
	int i = 0;

	top_pos    = MAX(top_pos, 1);
	bottom_pos = MIN(bottom_pos, image_h-1);
	memset(p_grad_img, 0, image_w * image_h);

#pragma omp parallel for
	for(i = top_pos; i < bottom_pos; i++) 
	{
		unsigned char *p3 = p_src_img + i * image_w;
		unsigned char *p4 = p3 + 2;
		unsigned char *p_grad = p_grad_img + i * image_w + 1;
		int j = 0;
		for(j = 0; j < image_w - 2; j++)
		{
			*p_grad = ABS((*p3) - (*p4));
			*p_grad = (*p_grad > threshold) ? binary_val : 0;

			p3++;
			p4++;
			p_grad++;
		}
	}
#endif
	return;
}

//sobel垂直方向图像边缘
void sobel_vertical_edge(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h, CV_RECT *roi)
{
	int i, begin_x, end_x, begin_y, end_y;

	// 按如下方式设置各个指针的指向

	//   P0-----P1-----P2 
	//    |      |      |
	//    |      |      | 
	//   P3------------P4 
	//    |      |      | 
	//    |      |      |  
	//   P5-----P6-----P7  

	if (roi != NULL)
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(image_w-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(image_h-2, roi->y+roi->height);
	}
	else
	{
		begin_x = 1;
		end_x = image_w-2;
		begin_y = 1;
		end_y = image_h-2;
	}

	for(i = begin_y; i <= end_y; i++)
	{
		int j = 0;
		for(j = begin_x; j <= end_x; j++)
		{
			unsigned char * p0 = p_src_img+(i-1)*image_w+(j-1);
			unsigned char * p1 = p0 + 1;
			unsigned char * p2 = p1 + 1;
			unsigned char * p3 = p0 + image_w;
			unsigned char * p4 = p2 + image_w;
			unsigned char * p5 = p3 + image_w;
			unsigned char * p6 = p5 + 1;
			unsigned char * p7 = p6 + 1;
			short  grad_y;
			//short grad_val;

			grad_y = (short)((*p2) + 2 * (*p4) + (*p7) - (*p5) - 2 * (*p3) - (*p0));
			p_sobel_img[i*image_w+j] = ABS(grad_y);
		}
	}
	return;
}


//sobel水平方向图像边缘
void sobel_horizontal_edge(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h, CV_RECT *roi)
{
	int i, begin_x, end_x, begin_y, end_y;

	// 按如下方式设置各个指针的指向

	//   P0-----P1-----P2 
	//    |      |      |
	//    |      |      | 
	//   P3------------P4 
	//    |      |      | 
	//    |      |      |  
	//   P5-----P6-----P7  

	if (roi != NULL)
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(image_w-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(image_h-2, roi->y+roi->height);
	}
	else
	{
		begin_x = 1;
		end_x = image_w-2;
		begin_y = 1;
		end_y = image_h-2;
	}

	for(i = begin_y; i <= end_y; i++)
	{
		int j = 0;
		for(j = begin_x; j <= end_x; j++)
		{
			unsigned char * p0 = p_src_img+(i-1)*image_w+(j-1);
			unsigned char * p1 = p0 + 1;
			unsigned char * p2 = p1 + 1;
			unsigned char * p3 = p0 + image_w;
			unsigned char * p4 = p2 + image_w;
			unsigned char * p5 = p3 + image_w;
			unsigned char * p6 = p5 + 1;
			unsigned char * p7 = p6 + 1;
			short grad_x;
			//short grad_val;

			grad_x = (short)((*p7) + 2 * (*p6) + (*p5) - (*p2) - 2 * (*p1) - (*p0));
			p_sobel_img[i*image_w+j] = ABS(grad_x);
		}
	}
	return;
}

//sobel图像边缘
void sobel_edge_ex(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h, int threshold, CV_RECT *roi)
{
	int i, begin_x, end_x, begin_y, end_y;

	// 按如下方式设置各个指针的指向

	//   P0-----P1-----P2 
	//    |      |      |
	//    |      |      | 
	//   P3------------P4 
	//    |      |      | 
	//    |      |      |  
	//   P5-----P6-----P7  

	if (roi != NULL)
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(image_w-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(image_h-2, roi->y+roi->height);
	}
	else
	{
		begin_x = 1;
		end_x = image_w-2;
		begin_y = 1;
		end_y = image_h-2;
	}

#pragma omp parallel for  schedule(dynamic)	
	for(i = begin_y; i <= end_y; i++)
	{
		int j = 0;
		for(j = begin_x; j <= end_x; j++)
		{
			unsigned char * p0 = p_src_img+(i-1)*image_w+(j-1);
			unsigned char * p1 = p0 + 1;
			unsigned char * p2 = p1 + 1;
			unsigned char * p3 = p0 + image_w;
			unsigned char * p4 = p2 + image_w;
			unsigned char * p5 = p3 + image_w;
			unsigned char * p6 = p5 + 1;
			unsigned char * p7 = p6 + 1;
			short grad_x, grad_y;
			//short grad_val;

			//grad_x = (short)((*p2) + 2 * (*p4) + (*p7) - (*p5) - 2 * (*p3) - (*p0));
			grad_x = (short)((*p1) - (*p6));
			if (ABS(grad_x) >= threshold)
			{
				p_sobel_img[i*image_w+j] = 255;
			}
			else
			{
				//grad_y = (short)((*p7) + 2 * (*p6) + (*p5) - (*p2) - 2 * (*p1) - (*p0));
				grad_y = (short)((*p3) - (*p4));
				if (ABS(grad_y) >= threshold)
				{
					p_sobel_img[i*image_w+j] = 255;
				}
				else
				{
					p_sobel_img[i*image_w+j] = 0;
				}
			}
			//grad_y = (short)((*p7) + 2 * (*p6) + (*p5) - (*p2) - 2 * (*p1) - (*p0));

			//grad_val = (short)(sqrt((float)(grad_x * grad_x + grad_y * grad_y)));
			//if (grad_val >= 30)
			//{
			//	p_sobel_img[i*image_w+j] = 255;
			//}
			//else
			//{
			//	p_sobel_img[i*image_w+j] = 0;
			//}
		}
	}

	return;
}

//sobel图像边缘
void sobel_edge(PBYTE p_src_img, PBYTE p_sobel_img, short *grad_img, int image_w,int image_h)
{
	int i, j;
	float grad_x, grad_y, scale_factor;
	short min_val, max_val, *p_grad, grad_val;
	unsigned char *p_sobel, *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;

	memset(p_sobel_img, 0, image_w * image_h);
	memset(grad_img, 0, image_w * image_h * sizeof(short));

	// 按如下方式设置各个指针的指向

	//   P0-----P1-----P2 
	//    |      |      |
	//    |      |      | 
	//   P3------------P4 
	//    |      |      | 
	//    |      |      |  
	//   P5-----P6-----P7  

	p0 = p_src_img;
	p1 = p0 + 1;
	p2 = p1 + 1;
	p3 = p0 + image_w;
	p4 = p2 + image_w;
	p5 = p3 + image_w;
	p6 = p5 + 1;
	p7 = p6 + 1;

	p_grad = grad_img + image_w + 1;

	min_val =  2048;
	max_val = -2048;

	for(i = 1; i < image_h - 1; i++)
	{
		for(j = 1; j < image_w - 1; j++)
		{
			grad_x = 0;
			grad_y = 0;

			grad_x = (float)((*p2) + 2 * (*p4) + (*p7) - (*p5) - 2 * (*p3) - (*p0));
			grad_y = (float)((*p7) + 2 * (*p6) + (*p5) - (*p2) - 2 * (*p1) - (*p0));

			grad_val = (short)(sqrt((float)(grad_x * grad_x + grad_y * grad_y)));

			*p_grad++ = grad_val;

			max_val = MAX(max_val, grad_val);
			min_val = MIN(min_val, grad_val);

			p0++;
			p1++;
			p2++;
			p3++;
			p4++;
			p5++;
			p6++;
			p7++;
		}

		p0 += 2;
		p1 += 2;
		p2 += 2;
		p3 += 2;
		p4 += 2;
		p5 += 2;
		p6 += 2;
		p7 += 2;

		p_grad += 2;
	}

	p_grad  = grad_img;
	p_sobel = p_sobel_img;

	if (max_val != min_val)
	{
		scale_factor = 255.0f / (max_val - min_val);
	}
	else
	{
		scale_factor = 0.0f;
	}

	for (i = 1; i < image_h - 1; ++i)
	{
		for (j = 1; j < image_w - 1; ++j)
		{
			int offset = i * image_w + j;
			BYTE val = (unsigned char)((short)(p_grad[offset] - min_val) * scale_factor);
			p_sobel[offset] = val;// > 30 ? 255 : 0;		
		}
	}
	return;
}




//图像滤波，并同时二值化
void image_filter(PBYTE p_src_img, PBYTE p_edge_img, BYTE threshold, int image_w, int image_h)
{
	int i, j;
	int grad;
	unsigned char *p_edge, *p1, *p2;

	p_edge = p_edge_img + 1;
	p1     = p_src_img;
	p2     = p_src_img + 2;

	memset(p_edge_img, 0, image_w * image_h);

	for(i = 0; i < image_h; i++)
	{
		for(j = 1; j < image_w - 1; j++)
		{
			grad = ((*p2) - (*p1));

			//*p_edge++ = grad >= threshold ? 255 : 0;
			*p_edge++ = ABS(grad) >= threshold ? 255 : 0;;

			p1++;
			p2++;
		}

		p1 += 2;
		p2 += 2;
		p_edge += 2;
	}

	return;
}

//二值图像中值滤波
void media_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h)
{
	int               i, j, sum;
	unsigned char    *pre_line, *cur_line, *nxt_line;
	unsigned char    *dst_line;

	pre_line = p_src;
	cur_line = pre_line + image_w;
	nxt_line = cur_line + image_w;
	dst_line = p_dst + image_w;

	memset(p_dst, 0, image_w * image_h);

	//图像平滑
	for(j = 1; j < image_h - 1; j++)
	{
		for(i = 1; i < image_w - 1; i++)
		{
			sum  = pre_line[i - 1] + pre_line[i] + pre_line[i + 1];
			sum += cur_line[i - 1] + cur_line[i] + cur_line[i + 1];
			sum += nxt_line[i - 1] + nxt_line[i] + nxt_line[i + 1];

			dst_line[i] = (unsigned char)(sum > 5 * 255) ? 255 : 0;
		}

		pre_line += image_w;
		cur_line += image_w;
		nxt_line += image_w;
		dst_line += image_w;
	}

	return;
}


 void avarage_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h)
 {
	 int               i, j, sum;
	 unsigned char    *pre_line, *cur_line, *nxt_line;
	 unsigned char    *dst_line;

	 pre_line = p_src;
	 cur_line = pre_line + image_w;
	 nxt_line = cur_line + image_w;
	 dst_line = p_dst + image_w;

	 memset(p_dst, 0, image_w * image_h);

	 //图像平滑
	 for(j = 1; j < image_h - 1; j++)
	 {
		 for(i = 1; i < image_w - 1; i++)
		 {
			 sum  = pre_line[i - 1] + pre_line[i] + pre_line[i + 1];
			 sum += cur_line[i - 1] + cur_line[i] + cur_line[i + 1];
			 sum += nxt_line[i - 1] + nxt_line[i] + nxt_line[i + 1];

			 dst_line[i] = (unsigned char)(sum/9.0+0.5);
		 }

		 pre_line += image_w;
		 cur_line += image_w;
		 nxt_line += image_w;
		 dst_line += image_w;
	 }

	 return;
 }


 void gauss_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h)
 {
	 int               i, j, sum;
	 unsigned char    *pre_line, *cur_line, *nxt_line;
	 unsigned char    *dst_line;

	 pre_line = p_src;
	 cur_line = pre_line + image_w;
	 nxt_line = cur_line + image_w;
	 dst_line = p_dst + image_w;

	 memset(p_dst, 0, image_w * image_h);

	 /*
	 1-2-1
	 2-4-2
	 1-2-1
	 */
	 //图像平滑
	 for(j = 1; j < image_h - 1; j++)
	 {
		 for(i = 1; i < image_w - 1; i++)
		 {
			 sum  = pre_line[i - 1] + 2*pre_line[i] + pre_line[i + 1];
			 sum += 2*cur_line[i - 1] + 4*cur_line[i] + 2*cur_line[i + 1];
			 sum += nxt_line[i - 1] + 2*nxt_line[i] + nxt_line[i + 1];

			 dst_line[i] = (unsigned char)(sum>>4);
		 }

		 pre_line += image_w;
		 cur_line += image_w;
		 nxt_line += image_w;
		 dst_line += image_w;
	 }

	 return;
 }


//对输入图像进行 2x2 下采样处理
void image_down_sample_2x2(unsigned char *pdst_img, unsigned char *psrc_img, int image_w, int image_h)
{
	int  i, j;
	int  dst_w = (image_w >> 1);
	int  dst_h = (image_h >> 1);
	unsigned char *p_src1, *p_src2, *p_dst;

	p_src1 = psrc_img;
    p_src2 = psrc_img + image_w;

	p_dst = pdst_img;

	for (i = 0; i < dst_h; i++)
	{
		for (j = 0; j < dst_w; j++)
		{
			short sum = 0;

			sum  = *p_src1++;
			sum += *p_src1++;
			sum += *p_src2++;
			sum += *p_src2++;

			*p_dst++ = (unsigned char)(sum >> 2);
		}

		p_src1 += image_w;
		p_src2 += image_w;
	}

	return;
}

//二值化
void image_binary(unsigned char *img, 
				  int            threshold,
				  int            image_w, 
				  int            image_h)
{
	int i, size;
	unsigned char *p_img;

	size = image_w * image_h;

	p_img = img;

	for (i = 0; i < size; ++i)
	{
		*p_img++ = (*p_img >= threshold) ? 255 : 0;
	}

	return;
}


//二值化
void image_binary_ex(unsigned char *img,  unsigned char *dst_img, int threshold,
				  int image_w, int image_h, CV_RECT *roi)
{
	int i, begin_x, end_x, begin_y, end_y;


	if (roi != NULL)
	{
		begin_x = MAX(0, roi->x);
		end_x = MIN(image_w-1, roi->x+roi->width);
		begin_y = MAX(0, roi->y);
		end_y = MIN(image_h-1, roi->y+roi->height);
	}
	else
	{
		begin_x = 0;
		end_x = image_w-1;
		begin_y = 0;
		end_y = image_h-1;
	}

#pragma omp parallel for  schedule(dynamic)	
	for(i = begin_y; i <= end_y; i++)
	{
		int j = 0;
		for(j = begin_x; j <= end_x; j++)
		{
			if (img[i*image_w+j] >= threshold)
			{
				dst_img[i*image_w+j] = 255;
			}
			else
			{
				dst_img[i*image_w+j] = 0;
			}
		}
	}
	return;
}


//二值化
void image_threshold(unsigned char *img,  unsigned char *dst_img, int threshold, BYTE value,
					 int image_w, int image_h, CV_RECT *roi)
{
	int i, begin_x, end_x, begin_y, end_y;


	if (roi != NULL)
	{
		begin_x = MAX(0, roi->x);
		end_x = MIN(image_w-1, roi->x+roi->width);
		begin_y = MAX(0, roi->y);
		end_y = MIN(image_h-1, roi->y+roi->height);
	}
	else
	{
		begin_x = 0;
		end_x = image_w-1;
		begin_y = 0;
		end_y = image_h-1;
	}

#pragma omp parallel for  schedule(dynamic)	
	for(i = begin_y; i <= end_y; i++)
	{
		int j = 0;
		for(j = begin_x; j <= end_x; j++)
		{
			if (img[i*image_w+j] >= threshold)
			{
				dst_img[i*image_w+j] = value;
			}
			else
			{
				dst_img[i*image_w+j] = 0;
			}
		}
	}
	return;
}

void integral_image(unsigned char *image, int *sum, int sum_col, int sum_row)
{
	int   x, y, i;
	int   image_w, image_h;
	unsigned char *p_img;
	int           *p_sum;
	int   s = 0;   
	int   residual = 0;

	image_w = sum_col - 1;
	image_h = sum_row - 1;

	residual = image_w & 0x3;

	memset(sum, 0, sum_col * sizeof(sum[0]));

	p_sum = sum + sum_col;
	p_img = image;

	//s = p_sum[0] = 0;

	//switch(residual)
	//{
	//case 1:
	//	{
	//		s += p_img[0];	p_sum[0] += s;		    
	//	}
	//	break;

	//case 2:
	//	{
	//		s += p_img[0];	p_sum[0] += s;	
	//		s += p_img[1];	p_sum[1] += s;	    
	//	}
	//	break;

	//case 3:
	//	{
	//		s += p_img[0];	p_sum[0] += s;	
	//		s += p_img[1];	p_sum[1] += s;
	//		s += p_img[2];	p_sum[2] += s;
	//	}
	//	break;

	//default:
	//	break;
	//}

	//for( x = residual; x < image_w; x += 4 )
	//{
	//	s += p_img[x];  	p_sum[x]   += s;
	//	s += p_img[x+1];	p_sum[x+1] += s;
	//	s += p_img[x+2];	p_sum[x+2] += s;
	//	s += p_img[x+3];	p_sum[x+3] += s;
	//}

	//p_img += image_w;
	//p_sum += sum_col;

	for( y = 0; y < image_h; y++)
	{
		int s = p_sum[0] = 0;

		memcpy(p_sum, p_sum - sum_col, sum_col * sizeof(int));

		for (i = 0; i < residual; i++)
		{
			s += p_img[i];	p_sum[i+1] += s;
		}
		//switch(residual)
		//{
		//case 1:
		//	{
		//		s += p_img[0];	p_sum[0] += s;		    
		//	}
		//	break;

		//case 2:
		//	{
		//		s += p_img[0];	p_sum[0] += s;	
		//		s += p_img[1];	p_sum[1] += s;	    
		//	}
		//	break;

		//case 3:
		//	{
		//		s += p_img[0];	p_sum[0] += s;	
		//		s += p_img[1];	p_sum[1] += s;
		//		s += p_img[2];	p_sum[2] += s;
		//	}
		//	break;

		//default:
		//	break;
		//}

		for( x = residual; x < image_w; x += 4 )
		{
			s += p_img[x];	    p_sum[x+1]   += s;
			s += p_img[x+1];	p_sum[x+2] += s;
			s += p_img[x+2];	p_sum[x+3] += s;
			s += p_img[x+3];	p_sum[x+4] += s;
		}
		//printf("[%3d][%3d]:%u %u %s"  , y, x - 4, p_img[x-4], p_sum[x-4], (y + 1) % 10 == 0 ? "\n\r" : "");

		p_img += image_w;
		p_sum += sum_col;
	}


	return;
}

//buffersize = 2 * dst_wid + 2 * dst_hei;
//void image_resize_bilinear(unsigned char *org_img, 
//						   unsigned char *dst_img,
//						   int            src_wid, 
//						   int            src_hei,
//						   int            dst_wid,
//						   int            dst_hei,
//						   short         *pbuffer)
//{
//	int   gx, gy;           
//	int   x1, x2, y1, y2;
//	int   scale_x, scale_y;
//	int   xx, yy;
//
//	int  shift_bit = 10;
//	int  shift_val = (1 << shift_bit);
//	int  mod_val   = shift_val - 1;
//
//	short *dx  = pbuffer;
//	short *dx1 = dx + dst_wid;
//
//	short *dy  = dx1 + dst_wid;
//	short *dy1 = dy  + dst_hei;
//
//	int residual;
//
//	scale_x = (src_wid << shift_bit) / dst_wid;
//	scale_y = (src_hei << shift_bit) / dst_hei;
//
//	//create x lut
//	residual = dst_wid & 0x3;
//
//	for(gx = dst_wid - 1; gx >= 3; gx -= 4)
//	{
//		//1
//		xx = gx * scale_x;	        
//		x1 = xx - (xx & mod_val); 
//
//		dx[gx]  = xx - x1; 
//		dx1[gx] = shift_val - dx[gx];
//
//		//2
//		xx = (gx - 1) * scale_x;	        
//		x1 = xx - (xx & mod_val); 
//
//		dx[gx - 1]  = xx - x1; 
//		dx1[gx - 1] = shift_val - dx[gx - 1];
//
//		//3
//		xx = (gx - 2) * scale_x;	        
//		x1 = xx - (xx & mod_val); 
//
//		dx[gx - 2]  = xx - x1; 
//		dx1[gx - 2] = shift_val - dx[gx - 2];
//
//		//4
//		xx = (gx - 3) * scale_x;	        
//		x1 = xx - (xx & mod_val); 
//
//		dx[gx - 3]  = xx - x1; 
//		dx1[gx - 3] = shift_val - dx[gx - 3];
//	}
//
//	gx = gx < 0 ? gx + 4 : gx;
//
//	switch(residual)
//	{
//	case 1:
//		{
//			xx = 0; //0 * scale_x;	        
//			x1 = 0; //xx - (xx & mod_val); 
//
//			dx[0]  = 0; //xx - x1; 
//			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];
//		}
//		break;
//
//	case 2:
//		{
//			//1
//			xx = 0; //0 * scale_x;	        
//			x1 = 0; //xx - (xx & mod_val); 
//
//			dx[0]  = 0; //xx - x1; 
//			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];
//
//			//2
//			xx = scale_x;//1 * scale_x;	        
//			x1 = xx - (xx & mod_val); 
//
//			dx[1]  = xx - x1; 
//			dx1[1] = shift_val - dx[1];
//		}
//		break;
//
//	case 3:
//		{
//			//1
//			xx = 0; //0 * scale_x;	        
//			x1 = 0; //xx - (xx & mod_val); 
//
//			dx[0]  = 0; //xx - x1; 
//			dx1[0] = shift_val; //(1 << shift_bit) - dx[0];
//
//			//2
//			xx = scale_x; //1 * scale_x;	        
//			x1 = xx - (xx & mod_val); 
//
//			dx[1]  = xx - x1; 
//			dx1[1] = shift_val - dx[1];
//
//			//3
//			xx = scale_x << 1; //2 * scale_x;	        
//			x1 = xx - (xx & mod_val); 
//
//			dx[2]  = xx - x1; 
//			dx1[2] = shift_val - dx[2];	
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	//create y lut
//	residual = dst_hei & 0x3;
//
//	for(gy = dst_hei - 1; gy >= 3; gy -= 4)
//	{
//		//1
//		yy = gy * scale_y;   
//		y1 = yy - (yy & mod_val); 
//
//		dy[gy]  = yy - y1;
//		dy1[gy] = shift_val - dy[gy];
//
//		//2
//		yy = (gy - 1) * scale_y;   
//		y1 = yy - (yy & mod_val); 
//
//		dy[gy - 1]  = yy - y1;
//		dy1[gy - 1] = shift_val - dy[gy - 1];
//
//		//3
//		yy = (gy - 2) * scale_y;   
//		y1 = yy - (yy & mod_val); 
//
//		dy[gy - 2]  = yy - y1;
//		dy1[gy - 2] = shift_val - dy[gy - 2];
//
//		//4
//		yy = (gy - 3) * scale_y;   
//		y1 = yy - (yy & mod_val); 
//
//		dy[gy - 3]  = yy - y1;
//		dy1[gy - 3] = shift_val - dy[gy - 3];
//	}
//
//	gy = gy < 0 ? gy + 4 : gy;
//
//	switch(residual)
//	{
//	case 1:
//		{
//			//1
//			yy = 0; //0 * scale_y;   
//			y1 = 0; //yy - (yy & mod_val); 
//
//			dy[0]  = 0; //yy - y1;
//			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		    
//		}
//		break;
//
//	case 2:
//		{
//			//1
//			yy = 0; //0 * scale_y;   
//			y1 = 0; //yy - (yy & mod_val); 
//
//			dy[0]  = 0; //yy - y1;
//			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		
//
//			//2
//			yy = scale_y; //1 * scale_y;   
//			y1 = yy - (yy & mod_val); 
//
//			dy[1]  = yy - y1;
//			dy1[1] = shift_val - dy[1];
//		}
//		break;
//
//	case 3:
//		{
//			//1
//			yy = 0; //0 * scale_y;   
//			y1 = 0; //yy - (yy & mod_val); 
//
//			dy[0]  = 0; //yy - y1;
//			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];
//
//			//2
//			yy = scale_y;//1 * scale_y;   
//			y1 = yy - (yy & mod_val); 
//
//			dy[1]  = yy - y1;
//			dy1[1] = shift_val - dy[1];	
//
//			//3
//			yy = scale_y << 1; //2 * scale_y;   
//			y1 = yy - (yy & mod_val); 
//
//			dy[2]  = yy - y1;
//			dy1[2] = shift_val - dy[2];	
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	//resize
//	residual = dst_wid & 0x3;
//
//	for(gy = 0; gy <= dst_hei - 1; gy++)
//	{
//		int dy1_val = dy1[gy];
//		int dy_val  = dy[gy];
//
//		unsigned char *p_line_y1, *p_line_y2, *p_line_dst;
//
//		p_line_dst = dst_img + gy * dst_wid;
//
//		y1 = (gy * scale_y) >> shift_bit;
//		y2 = y1 + 1;
//
//		p_line_y1 = org_img + y1 * src_wid;
//		p_line_y2 = org_img + y2 * src_wid;
//
//		for(gx = 0; gx < residual; gx++)
//		{
//			int val, val1, val2, val3, val4;
//			int dx1_val, dx_val;
//
//			x1 = (gx * scale_x) >> shift_bit;
//			x2 = x1 + 1;
//
//			dx1_val = dx1[gx];
//			dx_val  = dx[gx];
//
//			val1 = (int)p_line_y1[x1];
//			val2 = (int)p_line_y1[x2];
//			val3 = (int)p_line_y2[x1];
//			val4 = (int)p_line_y2[x2];
//
//			val = val1 * dx1_val * dy1_val	
//				+ val2 * dx_val  * dy1_val
//				+ val3 * dx1_val * dy_val
//				+ val4 * dx_val  * dy_val;
//
//			val = ((val >> shift_bit) >> shift_bit);
//
//			p_line_dst[gx] = (unsigned char)val;
//		}
//
//		for(; gx <= dst_wid - 1; gx += 4)
//		{
//			int val, val1, val2, val3, val4;
//			int dx1_val, dx_val;
//
//			//1
//			x1 = (gx * scale_x) >> shift_bit;
//			x2 = x1 + 1;
//
//			dx1_val = dx1[gx];
//			dx_val  = dx[gx];
//
//			val1 = (int)p_line_y1[x1];
//			val2 = (int)p_line_y1[x2];
//			val3 = (int)p_line_y2[x1];
//			val4 = (int)p_line_y2[x2];
//
//			val = val1 * dx1_val * dy1_val	
//				+ val2 * dx_val  * dy1_val
//				+ val3 * dx1_val * dy_val
//				+ val4 * dx_val  * dy_val;
//
//			val = ((val >> shift_bit) >> shift_bit);
//
//			p_line_dst[gx] = (unsigned char)val;
//
//			//2
//			x1 = ((gx + 1) * scale_x) >> shift_bit;
//			x2 = x1 + 1;
//
//			dx1_val = dx1[gx + 1];
//			dx_val  = dx[gx + 1];
//
//			val1 = (int)p_line_y1[x1];
//			val2 = (int)p_line_y1[x2];
//			val3 = (int)p_line_y2[x1];
//			val4 = (int)p_line_y2[x2];
//
//			val = val1 * dx1_val * dy1_val	
//				+ val2 * dx_val  * dy1_val
//				+ val3 * dx1_val * dy_val
//				+ val4 * dx_val  * dy_val;
//
//			val = ((val >> shift_bit) >> shift_bit);
//
//			p_line_dst[gx + 1] = (unsigned char)val;
//
//
//			//3
//			x1 = ((gx + 2) * scale_x) >> shift_bit;
//			x2 = x1 + 1;
//
//			dx1_val = dx1[gx + 2];
//			dx_val  = dx[gx + 2];
//
//			val1 = (int)p_line_y1[x1];
//			val2 = (int)p_line_y1[x2];
//			val3 = (int)p_line_y2[x1];
//			val4 = (int)p_line_y2[x2];
//
//			val = val1 * dx1_val * dy1_val	
//				+ val2 * dx_val  * dy1_val
//				+ val3 * dx1_val * dy_val
//				+ val4 * dx_val  * dy_val;
//
//			val = ((val >> shift_bit) >> shift_bit);
//
//			p_line_dst[gx + 2] = (unsigned char)val;
//
//			//4
//			x1 = ((gx + 3) * scale_x) >> shift_bit;
//			x2 = x1 + 1;
//
//			dx1_val = dx1[gx + 3];
//			dx_val  = dx[gx + 3];
//
//			val1 = (int)p_line_y1[x1];
//			val2 = (int)p_line_y1[x2];
//			val3 = (int)p_line_y2[x1];
//			val4 = (int)p_line_y2[x2];
//
//			val = val1 * dx1_val * dy1_val	
//				+ val2 * dx_val  * dy1_val
//				+ val3 * dx1_val * dy_val
//				+ val4 * dx_val  * dy_val;
//
//			val = ((val >> shift_bit) >> shift_bit);
//
//			p_line_dst[gx + 3] = (unsigned char)val;
//		}
//	}
//
//	return;
//}




void image_resize_bilinear(unsigned char *org_img, 
						   unsigned char *dst_img,
						   int            src_wid, 
						   int            src_hei,
						   int            dst_wid,
						   int            dst_hei,
						   short         *pbuffer)
{
	int   gx, gy;           
	int   x1, x2, y1, y2;
	int   scale_x, scale_y;
	int   xx, yy;

	int  shift_bit = 10;
	int  shift_val = (1 << shift_bit);
	int  mod_val   = shift_val - 1;

	short *dx  = pbuffer;
	short *dx1 = dx + dst_wid;

	short *dy  = dx1 + dst_wid;
	short *dy1 = dy  + dst_hei;

	int residual;

	scale_x = (src_wid << shift_bit) / dst_wid;
	scale_y = (src_hei << shift_bit) / dst_hei;

	//create x lut
	residual = dst_wid & 0x3;

	for(gx = dst_wid - 1; gx >= 3; gx -= 4)
	{
		//1
		xx = gx * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx]  = xx - x1; 
		dx1[gx] = shift_val - dx[gx];

		//2
		xx = (gx - 1) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 1]  = xx - x1; 
		dx1[gx - 1] = shift_val - dx[gx - 1];

		//3
		xx = (gx - 2) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 2]  = xx - x1; 
		dx1[gx - 2] = shift_val - dx[gx - 2];

		//4
		xx = (gx - 3) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 3]  = xx - x1; 
		dx1[gx - 3] = shift_val - dx[gx - 3];
	}

	gx = gx < 0 ? gx + 4 : gx;

	switch(residual)
	{
	case 1:
		{
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];
		}
		break;

	case 2:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];

			//2
			xx = scale_x;//1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];
		}
		break;

	case 3:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val; //(1 << shift_bit) - dx[0];

			//2
			xx = scale_x; //1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];

			//3
			xx = scale_x << 1; //2 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[2]  = xx - x1; 
			dx1[2] = shift_val - dx[2];	
		}
		break;

	default:
		break;
	}

	//create y lut
	residual = dst_hei & 0x3;

	for(gy = dst_hei - 1; gy >= 3; gy -= 4)
	{
		//1
		yy = gy * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy]  = yy - y1;
		dy1[gy] = shift_val - dy[gy];

		//2
		yy = (gy - 1) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 1]  = yy - y1;
		dy1[gy - 1] = shift_val - dy[gy - 1];

		//3
		yy = (gy - 2) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 2]  = yy - y1;
		dy1[gy - 2] = shift_val - dy[gy - 2];

		//4
		yy = (gy - 3) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 3]  = yy - y1;
		dy1[gy - 3] = shift_val - dy[gy - 3];
	}

	gy = gy < 0 ? gy + 4 : gy;

	switch(residual)
	{
	case 1:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		    
		}
		break;

	case 2:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		

			//2
			yy = scale_y; //1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];
		}
		break;

	case 3:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];

			//2
			yy = scale_y;//1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];	

			//3
			yy = scale_y << 1; //2 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[2]  = yy - y1;
			dy1[2] = shift_val - dy[2];	
		}
		break;

	default:
		break;
	}

	//resize
	residual = dst_wid & 0x3;

	for(gy = 0; gy <= dst_hei - 1; gy++)
	{
		int dy1_val = dy1[gy];
		int dy_val  = dy[gy];

		unsigned char *p_line_y1, *p_line_y2, *p_line_dst;

		p_line_dst = dst_img + gy * dst_wid;

		y1 = (gy * scale_y) >> shift_bit;
		y2 = y1 + 1;

		p_line_y1 = org_img + y1 * src_wid;
		p_line_y2 = org_img + y2 * src_wid;

		for(gx = 0; gx < residual; gx++)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			x1 = (gx * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;
		}

		for(; gx <= dst_wid - 1; gx += 4)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			//1
			x1 = (gx * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;

			//2
			x1 = ((gx + 1) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 1];
			dx_val  = dx[gx + 1];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3;

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 1] = (unsigned char)val;


			//3
			x1 = ((gx + 2) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 2];
			dx_val  = dx[gx + 2];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 2] = (unsigned char)val;

			//4
			x1 = ((gx + 3) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 3];
			dx_val  = dx[gx + 3];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 3] = (unsigned char)val;
		}
	}

	return;
}


//增加对UV的支持;
void image_resize_bilinear_ex(unsigned char *org_img, 
									  unsigned char *dst_img,
									  int            src_wid, 
									  int            src_hei,
									  int            dst_wid,
									  int            dst_hei,
									  short         *pbuffer)
{
	int   gx, gy;           
	int   x1, x2, y1, y2;
	int   scale_x, scale_y;
	int   xx, yy;

	int  shift_bit = 10;
	int  shift_val = (1 << shift_bit);
	int  mod_val   = shift_val - 1;

	short *dx  = pbuffer;
	short *dx1 = dx + dst_wid;

	short *dy  = dx1 + dst_wid;
	short *dy1 = dy  + dst_hei;
	PBYTE U = NULL;
	PBYTE V = NULL;
	PBYTE dU = NULL;
	PBYTE dV = NULL;

	int residual;

	scale_x = (src_wid << shift_bit) / dst_wid;
	scale_y = (src_hei << shift_bit) / dst_hei;

	//create x lut
	residual = dst_wid & 0x3;

	for(gx = dst_wid - 1; gx >= 3; gx -= 4)
	{
		//1
		xx = gx * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx]  = xx - x1; 
		dx1[gx] = shift_val - dx[gx];

		//2
		xx = (gx - 1) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 1]  = xx - x1; 
		dx1[gx - 1] = shift_val - dx[gx - 1];

		//3
		xx = (gx - 2) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 2]  = xx - x1; 
		dx1[gx - 2] = shift_val - dx[gx - 2];

		//4
		xx = (gx - 3) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 3]  = xx - x1; 
		dx1[gx - 3] = shift_val - dx[gx - 3];
	}

	gx = gx < 0 ? gx + 4 : gx;

	switch(residual)
	{
	case 1:
		{
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];
		}
		break;

	case 2:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];

			//2
			xx = scale_x;//1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];
		}
		break;

	case 3:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val; //(1 << shift_bit) - dx[0];

			//2
			xx = scale_x; //1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];

			//3
			xx = scale_x << 1; //2 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[2]  = xx - x1; 
			dx1[2] = shift_val - dx[2];	
		}
		break;

	default:
		break;
	}

	//create y lut
	residual = dst_hei & 0x3;

	for(gy = dst_hei - 1; gy >= 3; gy -= 4)
	{
		//1
		yy = gy * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy]  = yy - y1;
		dy1[gy] = shift_val - dy[gy];

		//2
		yy = (gy - 1) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 1]  = yy - y1;
		dy1[gy - 1] = shift_val - dy[gy - 1];

		//3
		yy = (gy - 2) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 2]  = yy - y1;
		dy1[gy - 2] = shift_val - dy[gy - 2];

		//4
		yy = (gy - 3) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 3]  = yy - y1;
		dy1[gy - 3] = shift_val - dy[gy - 3];
	}

	gy = gy < 0 ? gy + 4 : gy;

	switch(residual)
	{
	case 1:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		    
		}
		break;

	case 2:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		

			//2
			yy = scale_y; //1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];
		}
		break;

	case 3:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];

			//2
			yy = scale_y;//1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];	

			//3
			yy = scale_y << 1; //2 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[2]  = yy - y1;
			dy1[2] = shift_val - dy[2];	
		}
		break;

	default:
		break;
	}

	/*resize Y*/
	residual = dst_wid & 0x3;
	for(gy = 0; gy <= dst_hei - 1; gy++)
	{
		int dy1_val = dy1[gy];
		int dy_val  = dy[gy];

		unsigned char *p_line_y1, *p_line_y2, *p_line_dst;

		p_line_dst = dst_img + gy * dst_wid;

		y1 = (gy * scale_y) >> shift_bit;
		y2 = y1 + 1;

		p_line_y1 = org_img + y1 * src_wid;
		p_line_y2 = org_img + y2 * src_wid;

		for(gx = 0; gx < residual; gx++)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			x1 = (gx * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;
		}

		for(; gx <= dst_wid - 1; gx += 4)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			//1
			x1 = (gx * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;

			//2
			x1 = ((gx + 1) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 1];
			dx_val  = dx[gx + 1];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3;

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 1] = (unsigned char)val;


			//3
			x1 = ((gx + 2) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 2];
			dx_val  = dx[gx + 2];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 2] = (unsigned char)val;

			//4
			x1 = ((gx + 3) * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 3];
			dx_val  = dx[gx + 3];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 3] = (unsigned char)val;
		}
	}

	//resize UV
	U = org_img+src_wid*src_hei;
	V = org_img+src_wid*src_hei*5/4;
	dU = dst_img+dst_wid*dst_hei;
	dV = dst_img+dst_wid*dst_hei*5/4;
	src_wid >>= 1;
	src_hei >>= 1;
	dst_wid >>= 1;
	dst_hei >>= 1;
	residual = dst_wid & 0x3;

	for(gy = 0; gy <= dst_hei - 1; gy++)
	{
		int dy1_val = dy1[gy];
		int dy_val  = dy[gy];

		unsigned char *p_u_y1, *p_u_y2, *p_u_dst;
		unsigned char *p_v_y1, *p_v_y2, *p_v_dst;

		p_u_dst = dU + gy * dst_wid;
		p_v_dst = dV + gy*dst_wid;

		y1 = (gy * scale_y) >> shift_bit;
		y2 = y1 + 1;

		p_u_y1 = U + y1 * src_wid;
		p_u_y2 = U + y2 * src_wid;
		p_v_y1 = V + y1 * src_wid;
		p_v_y2 = V + y2 * src_wid;
		for(gx = 0; gx <= dst_wid - 1; gx++)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			x1 = (gx * scale_x) >> shift_bit;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_u_y1[x1];
			val2 = (int)p_u_y1[x2];
			val3 = (int)p_u_y2[x1];
			val4 = (int)p_u_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_u_dst[gx] = (unsigned char)val;

			val1 = (int)p_v_y1[x1];
			val2 = (int)p_v_y1[x2];
			val3 = (int)p_v_y2[x1];
			val4 = (int)p_v_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_v_dst[gx] = (unsigned char)val;
		}
	}

	return;
}

void image_resize_bilinear_roi(unsigned char *org_img, 
						   unsigned char *dst_img,
						   int            src_wid, 
						   int            src_hei,
						   int            dst_wid,
						   int            dst_hei,
						   CV_RECT       *roi,
						   short         *pbuffer)
{
	int   gx, gy;           
	int   x1, x2, y1, y2;
	int   scale_x, scale_y;
	int   xx, yy;

	int  shift_bit = 10;
	int  shift_val = (1 << shift_bit);
	int  mod_val   = shift_val - 1;

	short *dx  = pbuffer;
	short *dx1 = dx + dst_wid;

	short *dy  = dx1 + dst_wid;
	short *dy1 = dy  + dst_hei;

	int residual;
	int start_x, start_y;

	if (roi != NULL)
	{
		start_x = roi->x;
		start_y = roi->y;
		scale_x = (roi->width << shift_bit) / dst_wid;
		scale_y = (roi->height << shift_bit) / dst_hei;
	}
	else
	{
		start_x = 0;
		start_y = 0;
		scale_x = (src_wid << shift_bit) / dst_wid;
		scale_y = (src_hei << shift_bit) / dst_hei;
	}

	//create x lut
	residual = dst_wid & 0x3;

	for(gx = dst_wid - 1; gx >= 3; gx -= 4)
	{
		//1
		xx = gx * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx]  = xx - x1; 
		dx1[gx] = shift_val - dx[gx];

		//2
		xx = (gx - 1) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 1]  = xx - x1; 
		dx1[gx - 1] = shift_val - dx[gx - 1];

		//3
		xx = (gx - 2) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 2]  = xx - x1; 
		dx1[gx - 2] = shift_val - dx[gx - 2];

		//4
		xx = (gx - 3) * scale_x;	        
		x1 = xx - (xx & mod_val); 

		dx[gx - 3]  = xx - x1; 
		dx1[gx - 3] = shift_val - dx[gx - 3];
	}

	gx = gx < 0 ? gx + 4 : gx;

	switch(residual)
	{
	case 1:
		{
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];
		}
		break;

	case 2:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val;//(1 << shift_bit) - dx[0];

			//2
			xx = scale_x;//1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];
		}
		break;

	case 3:
		{
			//1
			xx = 0; //0 * scale_x;	        
			x1 = 0; //xx - (xx & mod_val); 

			dx[0]  = 0; //xx - x1; 
			dx1[0] = shift_val; //(1 << shift_bit) - dx[0];

			//2
			xx = scale_x; //1 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[1]  = xx - x1; 
			dx1[1] = shift_val - dx[1];

			//3
			xx = scale_x << 1; //2 * scale_x;	        
			x1 = xx - (xx & mod_val); 

			dx[2]  = xx - x1; 
			dx1[2] = shift_val - dx[2];	
		}
		break;

	default:
		break;
	}

	//create y lut
	residual = dst_hei & 0x3;

	for(gy = dst_hei - 1; gy >= 3; gy -= 4)
	{
		//1
		yy = gy * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy]  = yy - y1;
		dy1[gy] = shift_val - dy[gy];

		//2
		yy = (gy - 1) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 1]  = yy - y1;
		dy1[gy - 1] = shift_val - dy[gy - 1];

		//3
		yy = (gy - 2) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 2]  = yy - y1;
		dy1[gy - 2] = shift_val - dy[gy - 2];

		//4
		yy = (gy - 3) * scale_y;   
		y1 = yy - (yy & mod_val); 

		dy[gy - 3]  = yy - y1;
		dy1[gy - 3] = shift_val - dy[gy - 3];
	}

	gy = gy < 0 ? gy + 4 : gy;

	switch(residual)
	{
	case 1:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		    
		}
		break;

	case 2:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];		

			//2
			yy = scale_y; //1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];
		}
		break;

	case 3:
		{
			//1
			yy = 0; //0 * scale_y;   
			y1 = 0; //yy - (yy & mod_val); 

			dy[0]  = 0; //yy - y1;
			dy1[0] = shift_val; //(1 << shift_bit) - dy[0];

			//2
			yy = scale_y;//1 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[1]  = yy - y1;
			dy1[1] = shift_val - dy[1];	

			//3
			yy = scale_y << 1; //2 * scale_y;   
			y1 = yy - (yy & mod_val); 

			dy[2]  = yy - y1;
			dy1[2] = shift_val - dy[2];	
		}
		break;

	default:
		break;
	}

	//resize
	residual = dst_wid & 0x3;

	for(gy = 0; gy <= dst_hei - 1; gy++)
	{
		int dy1_val = dy1[gy];
		int dy_val  = dy[gy];

		unsigned char *p_line_y1, *p_line_y2, *p_line_dst;

		p_line_dst = dst_img + gy * dst_wid;

		y1 = ((gy * scale_y) >> shift_bit) + start_y;
		y2 = y1 + 1;

		p_line_y1 = org_img + y1 * src_wid;
		p_line_y2 = org_img + y2 * src_wid;

		for(gx = 0; gx < residual; gx++)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			x1 = ((gx * scale_x) >> shift_bit) + start_x;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;
		}

		for(; gx <= dst_wid - 1; gx += 4)
		{
			int val, val1, val2, val3, val4;
			int dx1_val, dx_val;
			int coef1, coef2, coef3, coef4;

			//1
			x1 = ((gx * scale_x) >> shift_bit) + start_x;
			x2 = x1 + 1;

			dx1_val = dx1[gx];
			dx_val  = dx[gx];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx] = (unsigned char)val;

			//2
			x1 = (((gx + 1) * scale_x) >> shift_bit) + start_x;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 1];
			dx_val  = dx[gx + 1];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3;

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 1] = (unsigned char)val;


			//3
			x1 = (((gx + 2) * scale_x) >> shift_bit) + start_x;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 2];
			dx_val  = dx[gx + 2];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 2] = (unsigned char)val;

			//4
			x1 = (((gx + 3) * scale_x) >> shift_bit) + start_x;
			x2 = x1 + 1;

			dx1_val = dx1[gx + 3];
			dx_val  = dx[gx + 3];

			val1 = (int)p_line_y1[x1];
			val2 = (int)p_line_y1[x2];
			val3 = (int)p_line_y2[x1];
			val4 = (int)p_line_y2[x2];

			coef1 = dx1_val * dy1_val;
			coef2 = (dy1_val << shift_bit) - coef1; 

			coef3 = dx1_val * dy_val;
			coef4 = (dy_val << shift_bit) - coef3; 

			val = (((val1 * coef1 + val2 * coef2 + val3 * coef3 + val4 * coef4) >> shift_bit) >> shift_bit);

			p_line_dst[gx + 3] = (unsigned char)val;
		}
	}

	return;
}



void image_down_sample_scale(unsigned char *psrc_img, unsigned char *pdst_img, int dst_w, int dst_h, int scale)
{
	int i,j;
	int src_w = dst_w*scale;
	int wid_step = (scale - 1) * src_w;
	BYTE *dst = pdst_img;
	BYTE *src = psrc_img;

	//#pragma omp parallel for  schedule(dynamic)
	for (i = 0; i < dst_h; i++)
	{
		//		BYTE *dst = pdst_img + i*dst_w;
		//		BYTE *src = psrc_img + i*scale*src_w;
		for (j = 0; j < dst_w; j++)
		{
			*dst = *src;
			dst++;
			src += scale;
		}
		src += wid_step;
	}
}



void dilation_filter_3x3_neon(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi)
{
	int begin_x, end_x, begin_y, end_y;
	int y, i;

	if (!src_img || !dst_img)
	{
		return;
	}

	if (roi) 
	{
		begin_x = MAX(1, roi->x);
		end_x = MIN(width-2, roi->x+roi->width);
		begin_y = MAX(1, roi->y);
		end_y = MIN(height-2, roi->y+roi->height);
	}
	else 
	{
		begin_x = 1;
		end_x = width-2;
		begin_y = 1;
		end_y = height-2;
	}

	for (y = begin_y; y <= end_y; y++) 
	{
		PBYTE s1 = src_img+(y-1)*width+begin_x;
		PBYTE s2 = s1 + width;
		PBYTE s3 = s2 + width;
		PBYTE d = dst_img+y*width+begin_x;
#ifdef LOCAL_ARM_NEON
		uint8x16_t c_255 = vdupq_n_u8(255);
		const PBYTE dEnd = d + end_x - begin_x + 1;
		for (; d <= dEnd - 16; d += 16) 
		{
			uint8x16_t s1_16 = vld1q_u8(s1);
			uint8x16_t s2_16 = vld1q_u8(s2);
			uint8x16_t s3_16 = vld1q_u8(s3);
			uint8x16_t s4_16= vld1q_u8(s2-1);
			uint8x16_t s5_16= vld1q_u8(s2+1);
			uint8x16_t sum1 = vqaddq_u8(vqaddq_u8(s1_16, s2_16), s3_16);
			uint8x16_t sum2 = vqaddq_u8(vqaddq_u8(s2_16, s4_16), s5_16);
			uint8x16_t tem1 = vceqq_u8(sum1, c_255);
			uint8x16_t tem2 = vceqq_u8(sum2, c_255);
			uint8x16_t tem = vorrq_u8(tem1, tem2);
			vst1q_u8(d, tem);

			s1 += 16;
			s2 += 16;
			s3 += 16;
		}
		for (; d < dEnd; d++) 
		{
			if (*s1 + *s2 + *s3 >= 255 || s2[-1] + s2[0] + s2[1] >= 255) 
			{
				*d = 255;
			}
			else 
			{
				*d = 0;
			}
			s1++;
			s2++;
			s3++;
		}
#else
		i = end_x - begin_x + 1;
		while (i--) 
		{
			if (*s1 + *s2 + *s3 >= 255 || s2[-1] + s2[0] + s2[1] >= 255) 
			{
				*d = 255;
			}
			else 
			{
				*d = 0;
			}
			d++;
			s1++;
			s2++;
			s3++;
		}
#endif
	}
}

void erosion_filter_3x3_neon(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi)
{
	int begin_x, end_x, begin_y, end_y;
	int y;

    if (!src_img || !dst_img) 
	{
        return;
    }

    if (roi) 
	{
        begin_x = MAX(1, roi->x);
        end_x = MIN(width-2, roi->x+roi->width);
        begin_y = MAX(1, roi->y);
        end_y = MIN(height-2, roi->y+roi->height);
    }
	else 
	{
        begin_x = 1;
        end_x = width-2;
        begin_y = 1;
        end_y = height-2;
    }

    for (y = begin_y; y <= end_y; y++) 
	{
        PBYTE s1 = src_img+(y-1)*width+begin_x;
        PBYTE s2 = s1 + width;
        PBYTE s3 = s2 + width;
        PBYTE d = dst_img+y*width+begin_x;
        const PBYTE dEnd = d + end_x - begin_x + 1;
#ifdef LOCAL_ARM_NEON
        uint8x16_t c_255 = vdupq_n_u8(255);
        for (; d <= dEnd - 16; d += 16) {
            uint8x16_t s1_16 = vld1q_u8(s1);
            uint8x16_t s2_16 = vld1q_u8(s2);
            uint8x16_t s3_16 = vld1q_u8(s3);
            uint8x16_t s4_16= vld1q_u8(s2-1);
            uint8x16_t s5_16= vld1q_u8(s2+1);
            uint8x16_t tem1 = vceqq_u8(s1_16, c_255);
            uint8x16_t tem2 = vceqq_u8(s2_16, c_255);
            uint8x16_t tem3 = vceqq_u8(s3_16, c_255);
            uint8x16_t tem4 = vceqq_u8(s4_16, c_255);
            uint8x16_t tem5 = vceqq_u8(s5_16, c_255);
            uint8x16_t tem = vandq_u8(vandq_u8(vandq_u8(vandq_u8(tem1, tem2), tem3), tem4), tem5);
            vst1q_u8(d, tem);

            s1 += 16;
            s2 += 16;
            s3 += 16;
        }
#endif
        for (; d < dEnd; d++) 
		{
            if (*s1 == 255 && *s2 == 255 && *s3 == 255 && s2[-1] == 255 && s2[1] == 255) 
			{
                *d = 255;
            }
			else 
			{
                *d = 0;
            }
            s1++;
            s2++;
            s3++;
        }
    }
}

/** 临近取样，画面有轻微锯齿，速度有明显提升
 */
void image_scale_2x2_to_1x1_down_sample(BYTE *src_data,
                                        BYTE *dst_data,
                                        int src_w,
                                        int src_h) 
{
    const int yMax = src_h >> 1;
    const int dst_w = src_w >> 1;
    int y;
    BYTE *d = dst_data;
    for (y = 0; y < yMax; y++)
	{
        const BYTE *s = src_data + src_w * 2 * y;
        const BYTE *dEnd = d + dst_w;
#ifdef LOCAL_ARM_NEON
        while (d <= dEnd - 16) {
            vst1q_u8(d, vld2q_u8(s).val[0]);
            d += 16;
            s += 32;
        }
#endif
        while (d < dEnd) 
		{
            *d++ = *s;
            s += 2;
        }
    }
}

/** 临近取样，画面有轻微锯齿，速度有明显提升
 */
void image_scale_4x4_to_1x1_down_sample(BYTE *src_data,
                                        BYTE *dst_data,
                                        int src_w,
                                        int src_h) 
{
    const int yMax = src_h >> 2;
    const int dst_w = src_w >> 2;
    int y;
    BYTE *d = dst_data;
    for (y = 0; y < yMax; y++) 
	{
        const BYTE *s = src_data + src_w * 4 * y + src_w;
        const BYTE *dEnd = d + dst_w;
#ifdef LOCAL_ARM_NEON
        while (d <= dEnd - 16)
		{
            vst1q_u8(d, vld4q_u8(s).val[1]);
            d += 16;
            s += 64;
        }
#endif
        while (d < dEnd) 
		{
            *d++ = s[1];
            s += 4;
        }
    }
}

/** 2:1缩小算法，建议使用down_sample
 */
void image_scale_2x2_to_1x1(BYTE *src_data,
                            BYTE *dst_data,
                            int src_w,
                            int src_h) 
{
    const int yMax = src_h >> 1;
    const int dst_w = src_w >> 1;
    int y;
    BYTE *d = dst_data;
    for (y = 0; y < yMax; y++)
	{
        const BYTE *s1 = src_data + src_w * 2 * y;
        const BYTE *s2 = s1 + src_w;
        const BYTE *dEnd = d + dst_w;
#ifdef LOCAL_ARM_NEON
        while (d <= dEnd - 16) 
		{
            uint8x16x2_t s_16x2_1 = vld2q_u8(s1);
            uint8x16x2_t s_16x2_2 = vld2q_u8(s2);
            uint8x16_t ave_16_1 = vhaddq_u8(s_16x2_1.val[0], s_16x2_1.val[1]);
            uint8x16_t ave_16_2 = vhaddq_u8(s_16x2_2.val[0], s_16x2_2.val[1]);
            uint8x16_t d_16 = vhaddq_u8(ave_16_1, ave_16_2);
            vst1q_u8(d, d_16);

            d += 16;
            s1 += 32;
            s2 += 32;
        }
#endif
        while (d < dEnd)
		{
            *d++ = (s1[0]+s1[1]+s2[0]+s2[1])>>2;
            s1 += 2;
            s2 += 2;
        }
    }
}

/** 3:2缩小算法
 */
void image_scale_3x3_to_2x2(BYTE *src_data,
                            BYTE *dst_data,
                            int src_w,
                            int src_h)
{
    const int yMax = src_h / 3;
    const int dst_w = src_w / 3 * 2;
    int y;
    for (y = 0; y < yMax; y++)
	{
        const BYTE *s1 = src_data + src_w * 3 * y;
        const BYTE *s2 = s1 + src_w;
        const BYTE *s3 = s2 + src_w;
        BYTE *d1 = dst_data + dst_w * 2 * y;
        BYTE *d2 = d1 + dst_w;
        const BYTE *dEnd = d2;
#ifdef LOCAL_ARM_NEON
        while (d1 <= dEnd - 32) 
		{
            uint8x16x3_t s1_16x3 = vld3q_u8(s1);
            uint8x16x3_t s2_16x3 = vld3q_u8(s2);
            uint8x16x3_t s3_16x3 = vld3q_u8(s3);
            uint8x16x2_t d1_16x2 = {
                vhaddq_u8(vhaddq_u8(s1_16x3.val[1], s2_16x3.val[0]), s1_16x3.val[0]),
                vhaddq_u8(vhaddq_u8(s1_16x3.val[1], s2_16x3.val[2]), s1_16x3.val[2])
            };
            uint8x16x2_t d2_16x2 = {
                vhaddq_u8(vhaddq_u8(s3_16x3.val[1], s2_16x3.val[0]), s3_16x3.val[0]),
                vhaddq_u8(vhaddq_u8(s3_16x3.val[1], s2_16x3.val[2]), s3_16x3.val[2])
            };

            vst2q_u8(d1, d1_16x2);
            vst2q_u8(d2, d2_16x2);

            d1 += 32;
            d2 += 32;
            s1 += 48;
            s2 += 48;
            s3 += 48;
        }
#endif
        while (d1 < dEnd) 
		{
            d1[0] = (s1[0]+s1[0]+s1[1]+s2[0])>>2;
            d1[1] = (s1[2]+s1[2]+s1[1]+s2[2])>>2;
            d2[0] = (s3[0]+s3[0]+s3[1]+s2[0])>>2;
            d2[1] = (s3[2]+s3[2]+s3[1]+s2[2])>>2;

            d1 += 2;
            d2 += 2;
            s1 += 3;
            s2 += 3;
            s3 += 3;
        }
    }
}

/** 4:3缩小算法
 */
void image_scale_4x4_to_3x3(BYTE *src_data,
                            BYTE *dst_data,
                            int src_w,
                            int src_h) 
{
    const int yMax = src_h >> 2;
    const int dst_w = src_w / 4 * 3;
    int y;
    for (y = 0; y < yMax; y++) 
	{
        const BYTE *s1 = src_data + src_w * 4 * y;
        const BYTE *s2 = s1 + src_w;
        const BYTE *s3 = s2 + src_w;
        const BYTE *s4 = s3 + src_w;
        BYTE *d1 = dst_data + dst_w * 3 * y;
        BYTE *d2 = d1 + dst_w;
        BYTE *d3 = d2 + dst_w;
        const BYTE *dEnd = d2;
#ifdef LOCAL_ARM_NEON
        while (d1 <= dEnd - 48)
		{
            uint8x16x4_t s1_16x4 = vld4q_u8(s1);
            uint8x16x4_t s2_16x4 = vld4q_u8(s2);
            uint8x16x4_t s3_16x4 = vld4q_u8(s3);
            uint8x16x4_t s4_16x4 = vld4q_u8(s4);
            uint8x16x3_t d1_16x3;
            uint8x16x3_t d2_16x3;
            uint8x16x3_t d3_16x3;
            uint8x16_t tem;

            d1_16x3.val[0] = vhaddq_u8(vhaddq_u8(s1_16x4.val[1], s2_16x4.val[0]), s1_16x4.val[0]);
            tem = vhaddq_u8(s1_16x4.val[1], s1_16x4.val[2]);
            d1_16x3.val[1] = vhaddq_u8(vhaddq_u8(vhaddq_u8(s2_16x4.val[1], s2_16x4.val[2]), tem), tem);
            d1_16x3.val[2] = vhaddq_u8(vhaddq_u8(s1_16x4.val[2], s2_16x4.val[3]), s1_16x4.val[3]);

            tem = vhaddq_u8(s2_16x4.val[0], s3_16x4.val[0]);
            d2_16x3.val[0] = vhaddq_u8(vhaddq_u8(vhaddq_u8(s2_16x4.val[1], s3_16x4.val[1]), tem), tem);
            d2_16x3.val[1] = vhaddq_u8(vhaddq_u8(s2_16x4.val[1], s2_16x4.val[2]), vhaddq_u8(s3_16x4.val[1], s3_16x4.val[2]));
            tem = vhaddq_u8(s2_16x4.val[3], s3_16x4.val[3]);
            d2_16x3.val[2] = vhaddq_u8(vhaddq_u8(vhaddq_u8(s2_16x4.val[2], s3_16x4.val[2]), tem), tem);

            d3_16x3.val[0] = vhaddq_u8(vhaddq_u8(s4_16x4.val[1], s3_16x4.val[0]), s4_16x4.val[0]);
            tem = vhaddq_u8(s4_16x4.val[1], s4_16x4.val[2]);
            d3_16x3.val[1] = vhaddq_u8(vhaddq_u8(vhaddq_u8(s3_16x4.val[1], s3_16x4.val[2]), tem), tem);
            d3_16x3.val[2] = vhaddq_u8(vhaddq_u8(s4_16x4.val[2], s3_16x4.val[3]), s4_16x4.val[3]);

            vst3q_u8(d1, d1_16x3);
            vst3q_u8(d2, d2_16x3);
            vst3q_u8(d3, d3_16x3);

            d1 += 48;
            d2 += 48;
            d3 += 48;
            s1 += 64;
            s2 += 64;
            s3 += 64;
            s4 += 64;
        }
#endif
        while (d1 < dEnd)
		{
            d1[0] = (s1[0]+s1[0]+s1[1]+s2[0])>>2;
            d1[1] = ((s1[1]+s1[2])*3+(s2[1]+s2[2]))>>3;
            d1[2] = (s1[3]+s1[3]+s1[2]+s2[3])>>2;
            d2[0] = ((s2[0]+s3[0])*3+(s2[1]+s3[1]))>>3;
            d2[1] = (s2[1]+s2[2]+s3[1]+s3[2])>>2;
            d2[2] = ((s2[3]+s3[3])*3+(s2[2]+s3[2]))>>3;
            d3[0] = (s4[0]+s4[0]+s4[1]+s3[0])>>2;
            d3[1] = ((s4[1]+s4[2])*3+(s3[1]+s3[2]))>>3;
            d3[2] = (s4[3]+s4[3]+s4[2]+s3[3])>>2;

            d1 += 3;
            d2 += 3;
            d3 += 3;
            s1 += 4;
            s2 += 4;
            s3 += 4;
            s4 += 4;
        }
    }
}


void gauss_filter_neon(PBYTE p_src, PBYTE p_dst, int image_w, int image_h)
{
	/*
	1-2-1
	2-4-2
	1-2-1
	*/
	//图像平滑

	int y;

	memset(p_dst, 0, image_w * image_h);
	for(y = 1; y < image_h - 1; y++) {
		PBYTE s1 = p_src + (y - 1) * image_w + 1;
		PBYTE s2 = s1 + image_w;
		PBYTE s3 = s2 + image_w;
		PBYTE d = p_dst + y * image_w + 1;
		const PBYTE dEnd = d + image_w - 2;
#ifdef ENABLE_NEON
		while (d <= dEnd - 16) {
			uint8x16_t s1_16 = vld1q_u8(s1);
			uint8x16_t s2_16 = vld1q_u8(s2);
			uint8x16_t s3_16 = vld1q_u8(s3);
			uint8x16_t s1_l_16 = vld1q_u8(s1-1);
			uint8x16_t s2_l_16 = vld1q_u8(s2-1);
			uint8x16_t s3_l_16 = vld1q_u8(s3-1);
			uint8x16_t s1_r_16 = vld1q_u8(s1+1);
			uint8x16_t s2_r_16 = vld1q_u8(s2+1);
			uint8x16_t s3_r_16 = vld1q_u8(s3+1);
			uint8x16_t sum1_16 = vhaddq_u8(vhaddq_u8(s2_l_16, s1_16), vhaddq_u8(vhaddq_u8(s1_l_16, s1_r_16), s2_16));
			uint8x16_t sum2_16 = vhaddq_u8(vhaddq_u8(s2_r_16, s3_16), vhaddq_u8(vhaddq_u8(s3_l_16, s3_r_16), s2_16));
			uint8x16_t tem = vhaddq_u8(sum1_16, sum2_16);
			vst1q_u8(d, tem);
			d += 16;
			s1 += 16;
			s2 += 16;
			s3 += 16;
		}
#endif
		while (d < dEnd) {
			*d++ = ((s1[-1] + s1[1] + s3[-1] + s3[1]) +
				(s1[0] + s2[-1] + s2[1] + s3[0]) +
				(s1[0] + s2[-1] + s2[1] + s3[0]) +
				(s2[0] + s2[0] + s2[0] + s2[0])) >> 4;
			s1++;
			s2++;
			s3++;
		}
	}
}



int GammaCorrection(PBYTE src, PBYTE dst, int width, int height, float fGamma)
{
	// build look up table
	unsigned char lut[256];
	int i = 0;

	for(i = 0; i < 256; i++ )
	{
		lut[i] = (unsigned char)(pow((float)(i/255.0), fGamma) * 255.0f);
	}

	for (i = 0; i < width*height; i++)
	{
		dst[i] = lut[src[i]];
	}

	return 0;
}


int image_copy(unsigned char *pimg, unsigned char *pout,int image_w, int image_h, CV_RECT *roi)
{
	int i, j;
	unsigned char *pdst = pout;


	for (j = roi->y; j < roi->y+roi->height; j++)
	{
		for (i = roi->x; i < roi->x+roi->width; i++)
		{
			*pdst++ = pimg[j*image_w+i];
		}
	}

	return 0;
}

