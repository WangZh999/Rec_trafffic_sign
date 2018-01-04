
#ifndef _UTIL_INCLUDE_H_
#define _UTIL_INCLUDE_H_

#include "base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef     LOCAL_ARM_NEON
#include <arm_neon.h>
#endif



/*YUV到RGB转换中的运算*/
#define GET_R(sy,su,sv) ((297 * sy + 407 * sv - 56845) >> 8)                        
#define GET_G(sy,su,sv) ((297 * sy - 100 * su - 207 * sv + 34574) >> 8) 
#define GET_B(sy,su,sv) ((297 * sy + 514 * su - 70591) >> 8)  


//定点开方运算
//入口参数：被开方数，长整型; 出口参数：开方结果，整型 
DLL_API unsigned int fix_sqrt_16(unsigned int M);

//将RGB图像转换为灰度图
DLL_API void rgb_to_gray(PBYTE p_rgb_img, PBYTE p_gray_img, int image_w, int image_h);

//获取子区域图像数据
DLL_API void get_subimg_data(PBYTE p_gray_img, PBYTE p_subimg, CV_BLOB sub_rect, int image_w, int image_h);

//对二值图像进行形态学膨胀腐蚀操作
//void dilation_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height);
DLL_API void dilation_filter_5x5(PBYTE src_img, PBYTE dst_img, int width, int height);


DLL_API void erosion_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi);

DLL_API void dilation_filter_3x3(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi);

DLL_API void sobel_edge_ex(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h,  int threshold, CV_RECT *roi);


//垂直梯度
DLL_API void verticle_grad(unsigned char *p_src_img, 
				   unsigned char *p_grad_img,
				   int            image_w,
				   int            image_h,
				   int            top_pos,
				   int            bottom_pos,
				   unsigned char  threshold,
				   unsigned char  binary_val);

//水平梯度
DLL_API void horizontal_grad(unsigned char *p_src_img, 
					 unsigned char *p_grad_img,
					 int            image_w,
					 int            image_h,
					 int            top_pos,
					 int            bottom_pos,
					 unsigned char  threshold,
					 unsigned char  binary_val);

//sobel水平方向图像边缘
DLL_API void sobel_vertical_edge(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h, CV_RECT *roi);

//sobel图像边缘
DLL_API void sobel_horizontal_edge(PBYTE p_src_img, PBYTE p_sobel_img, int image_w,int image_h, CV_RECT *roi);


DLL_API void sobel_edge(PBYTE p_src_img, PBYTE p_sobel_img, short *grad_img, int image_w,int image_h);

//图像滤波，并同时二值化
DLL_API void image_filter(PBYTE p_src_img, PBYTE p_edge_img, BYTE threshold, int image_w, int image_h);

//二值图像中值滤波
DLL_API void media_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h);

//¾ùÖµÂË²¨
DLL_API void avarage_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h);

DLL_API void gauss_filter(PBYTE p_src, PBYTE p_dst, int image_w, int image_h);


DLL_API void image_down_sample_2x2(PBYTE dst_img, PBYTE src_img, int image_w, int image_h);

//二值化
DLL_API void image_binary(unsigned char *img, int threshold, int image_w, int image_h);

DLL_API void image_binary_ex(unsigned char *img,  unsigned char *dst_img, int threshold,
					 int image_w, int mage_h, CV_RECT *roi);


DLL_API void image_threshold(unsigned char *img,  unsigned char *dst_img, int threshold, BYTE value,
					 int image_w, int image_h, CV_RECT *roi);

DLL_API void integral_image(unsigned char *image, int *sum, int sum_col, int sum_row);

//buffersize = 2 * dst_wid + 2 * dst_hei;
DLL_API void image_resize_bilinear(unsigned char *org_img, 
						   unsigned char *dst_img,
						   int            src_wid, 
						   int            src_hei,
						   int            dst_wid,
						   int            dst_hei,
						   short         *pbuffer);

DLL_API void image_resize_bilinear_ex(unsigned char *org_img, 
								   unsigned char *dst_img,
								   int            src_wid, 
								   int            src_hei,
								   int            dst_wid,
								   int            dst_hei,
								   short          *pbuffer);

DLL_API void image_resize_bilinear_roi(unsigned char *org_img, 
							   unsigned char *dst_img,
							   int            src_wid, 
							   int            src_hei,
							   int            dst_wid,
							   int            dst_hei,
							   CV_RECT        *roi,
							   short          *pbuffer);

								   
DLL_API void image_down_sample_scale(unsigned char *psrc_img, unsigned char *pdst_img, int dst_w, int dst_h, int scale);


DLL_API void erosion_filter_3x3_neon(PBYTE src_img, PBYTE dst_img, int width, int height, CV_RECT *roi);

DLL_API void image_scale_2x2_to_1x1_down_sample(BYTE *src_data, BYTE *dst_data, int src_w, int src_h);

DLL_API void image_scale_4x4_to_1x1_down_sample(BYTE *src_data, BYTE *dst_data, int src_w, int src_h);

DLL_API void image_scale_2x2_to_1x1(BYTE *src_data, BYTE *dst_data, int src_w, int src_h);

DLL_API void image_scale_3x3_to_2x2(BYTE *src_data, BYTE *dst_data, int src_w, int src_h);

DLL_API void image_scale_4x4_to_3x3(BYTE *src_data, BYTE *dst_data, int src_w, int src_h);

DLL_API void gauss_filter_neon(PBYTE p_src, PBYTE p_dst, int image_w, int image_h);

DLL_API int GammaCorrection(PBYTE src, PBYTE dst, int width, int height, float fGamma);

DLL_API int image_copy(unsigned char *pimg, unsigned char *pout,int image_w, int image_h, CV_RECT *roi);

#ifdef __cplusplus
}
#endif



#endif

