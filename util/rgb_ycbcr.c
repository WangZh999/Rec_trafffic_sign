

#include "rgb_ycbcr.h"
//#include "arch_def.h"


/******************************************************************************
* 功  能：将RGB图像存储方式为 BGR BGR BGR BGR 的数据帧转换为存储方式为 YCbCr YCbCr YCbCr YCbCr 的数据帧

* 要  求：输入RGB数据的存储方式为：   BGR BGR BGR BGR
*         输出YCbCr数据的存储方式为： YCbCr YCbCr YCbCr YCbCr

* 备  注：输入和输出的图像尺寸大小和占用内存相等，图像宽高的尺度没有必须为偶数或一定要为四字节对齐的要求         

* 返回值：
******************************************************************************/
void bgr2ycbcr(unsigned char *prgb, unsigned char *pycbcr, int image_w, int image_h)
{
    int i, j;
	unsigned char *ptr1, *ptr2;

    int residue = image_w & 0x3;

	ptr1 = prgb;
	ptr2 = pycbcr;

	for (i = 0; i < image_h; ++i)
	{
		switch(residue)
		{
		case 1:
			{
				int b = *ptr1++;
				int g = *ptr1++;
				int r = *ptr1++;

				int y, cb, cr;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;
			}
			break;

		case 2:
			{
				int b = *ptr1++;
				int g = *ptr1++;
				int r = *ptr1++;

				int y, cb, cr;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;

                //2
				b = *ptr1++;
				g = *ptr1++;
				r = *ptr1++;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;

			}
			break;

		case 3:
			{
				int b = *ptr1++;
				int g = *ptr1++;
				int r = *ptr1++;

				int y, cb, cr;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;

                //2
				b = *ptr1++;
				g = *ptr1++;
				r = *ptr1++;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;

                //3
				b = *ptr1++;
				g = *ptr1++;
				r = *ptr1++;

				RGB2YCbCr(r, g, b, y, cb, cr)

				*ptr2++ = y;
				*ptr2++ = cb;
				*ptr2++ = cr;
			}
			break;

		default:
			break;		    
		}

		for (j = residue; j < image_w; j += 4)
		{
			int b = *ptr1++;
			int g = *ptr1++;
			int r = *ptr1++;

			int y, cb, cr;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*ptr2++ = y;
			*ptr2++ = cb;
			*ptr2++ = cr;

            //2
			b = *ptr1++;
			g = *ptr1++;
			r = *ptr1++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*ptr2++ = y;
			*ptr2++ = cb;
			*ptr2++ = cr;

            //3
			b = *ptr1++;
			g = *ptr1++;
			r = *ptr1++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*ptr2++ = y;
			*ptr2++ = cb;
			*ptr2++ = cr;

            //4
			b = *ptr1++;
			g = *ptr1++;
			r = *ptr1++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*ptr2++ = y;
			*ptr2++ = cb;
			*ptr2++ = cr;
		}
	}

    return;
}

/******************************************************************************
* 功  能：将YCbCr图像存储方式为  YCbCr YCbCr YCbCr YCbCr 的数据帧转换为RGB图像

* 要  求：输入YCbCr数据的存储方式为： YCbCr YCbCr YCbCr YCbCr
*         输出RGB数据的存储方式为：   BGR BGR BGR BGR

* 备  注：输入和输出的图像尺寸大小和占用内存相等，图像宽高的尺度没有必须为偶数或一定要为四字节对齐的要求         

* 返回值：
******************************************************************************/
void ycbcr2bgr(unsigned char *pycbcr, unsigned char *prgb, int image_w, int image_h)
{
    int i, j;
	unsigned char *ptr1, *ptr2;

    int residue = image_w & 0x3;

	ptr1 = pycbcr;
	ptr2 = prgb;

	for (i = 0; i < image_h; ++i)
	{
		switch(residue)
		{
		case 1:
			{
				int y  = *ptr1++;
				int cb = *ptr1++;
				int cr = *ptr1++;

				int r, g, b;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;
			}
			break;

		case 2:
			{
				int y  = *ptr1++;
				int cb = *ptr1++;
				int cr = *ptr1++;

				int r, g, b;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;

                //2
				y  = *ptr1++;
				cb = *ptr1++;
				cr = *ptr1++;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;
			}
			break;

		case 3:
			{
				int y  = *ptr1++;
				int cb = *ptr1++;
				int cr = *ptr1++;

				int r, g, b;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;

                //2
				y  = *ptr1++;
				cb = *ptr1++;
				cr = *ptr1++;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;

                //3
				y  = *ptr1++;
				cb = *ptr1++;
				cr = *ptr1++;

				YCbCr2RGB(y, cb, cr, r, g, b)

				*ptr2++ = b;
				*ptr2++ = g;
				*ptr2++ = r;
			}
			break;

		default:
			break;		    
		}

		for (j = residue; j < image_w; j += 4)
		{
			int y  = *ptr1++;
			int cb = *ptr1++;
			int cr = *ptr1++;

			int r, g, b;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*ptr2++ = b;
			*ptr2++ = g;
			*ptr2++ = r;

            //2
			y  = *ptr1++;
			cb = *ptr1++;
			cr = *ptr1++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*ptr2++ = b;
			*ptr2++ = g;
			*ptr2++ = r;

            //3
			y  = *ptr1++;
			cb = *ptr1++;
			cr = *ptr1++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*ptr2++ = b;
			*ptr2++ = g;
			*ptr2++ = r;

            //4
			y  = *ptr1++;
			cb = *ptr1++;
			cr = *ptr1++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*ptr2++ = b;
			*ptr2++ = g;
			*ptr2++ = r;
		}
	}

    return;
}

/******************************************************************************
* 功  能：将RGB图像存储方式为 BGR BGR BGR BGR 的数据帧转换为存储方式为 YYYY YYYY CbCb CrCr 的数据帧

* 要  求：输入RGB数据的存储方式为：   BGR BGR BGR BGR
*         输出YCbCr数据的存储方式为： YYYY YYYY CbCb CrCr

* 备  注：输入图像宽高需要为偶数        

* 返回值：
******************************************************************************/
void bgr2ycbcr420(unsigned char *prgb, unsigned char *pycbcr, int image_w, int image_h)
{
	int i, j;
	unsigned char *ptr1, *ptr2, *py1, *py2, *pcb, *pcr;

	ptr1 = prgb;
	ptr2 = ptr1 + image_w * 3; //the next line

	py1  = pycbcr;
	py2  = py1 + image_w; //the next line
	pcb  = pycbcr + image_w * image_h;
    pcr  = pcb    + (image_w * image_h) / 4;

	for (i = 0; i < image_h; i += 2)
	{
		for (j = 0; j < image_w; j += 2)
		{
			int b = *ptr1++;
			int g = *ptr1++;
			int r = *ptr1++;

			int y, cb, cr;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*py1++ = y;

			*pcb++ = cb;
			*pcr++ = cr;

			b = *ptr1++;
			g = *ptr1++;
			r = *ptr1++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*py1++ = y;

			b = *ptr2++;
			g = *ptr2++;
			r = *ptr2++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*py2++ = y;

			b = *ptr2++;
			g = *ptr2++;
			r = *ptr2++;

			RGB2YCbCr(r, g, b, y, cb, cr)

			*py2++ = y;
		}

        py1  += image_w;
        py2  += image_w;

		ptr1 += 3 * image_w;
		ptr2 += 3 * image_w;
	}

	return;
}

/******************************************************************************
* 功  能：将YCbCr420图像存储方式为  YYYY YYYY CbCb CrCr 的数据帧转换为RGB图像

* 要  求：输入YCbCr420数据的存储方式为： YYYY YYYY CbCb CrCr
*         输出RGB数据的存储方式为：      BGR BGR BGR BGR

* 备  注：输入图像宽需要为偶数         

* 返回值：
******************************************************************************/
void ycbcr420_to_bgr(unsigned char *pycbcr, unsigned char *prgb, int image_w, int image_h)
{
    int i, j;
	unsigned char *py1, *py2, *pcb, *pcr, *pdst1, *pdst2;

	py1  = pycbcr;          
	py2  = pycbcr + image_w; //the next line
	pcb  = pycbcr + image_w * image_h;
	pcr  = pcb    + (image_w * image_h) / 4;

	pdst1 = prgb;
	pdst2 = prgb + 3 * image_w; //the next line

	for(i = 0; i < image_h; i += 2)
	{
		for (j = 0; j < image_w; j += 2) 
		{ 
			int y, cb, cr;
            int r, g, b;

			y   = *py1++;
			cb  = *pcb++;
			cr  = *pcr++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*pdst1++ = b;
			*pdst1++ = g;
			*pdst1++ = r;


			y   = *py1++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*pdst1++ = b;
			*pdst1++ = g;
			*pdst1++ = r;


			y   = *py2++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*pdst2++ = b;
			*pdst2++ = g;
			*pdst2++ = r;

			y   = *py2++;

			YCbCr2RGB(y, cb, cr, r, g, b)

			*pdst2++ = b;
			*pdst2++ = g;
			*pdst2++ = r;
		}

		py1  += image_w;
		py2  += image_w;

		pdst1 += 3 * image_w;
		pdst2 += 3 * image_w;
	}

    return;
}








long int crv_tab[256];
long int cbu_tab[256];
long int cgu_tab[256];

long int cgv_tab[256];
long int tab_76309[256];
unsigned char clp[1024];


void init_dither_tab()
{
	long int crv, cbu, cgu, cgv;
	int i, ind;

	crv = 104597; 
	cbu = 132201; 
	cgu = 25675;  
	cgv = 53279;

	for (i = 0; i < 256; i++)
	{
		crv_tab[i] = (i - 128) * crv;
		cbu_tab[i] = (i - 128) * cbu;
		cgu_tab[i] = (i - 128) * cgu;
		cgv_tab[i] = (i - 128) * cgv;
		
		tab_76309[i] = 76309 * (i - 16);
	}

	for (i=0; i<384; i++)
	{
		clp[i] = 0;
	}

	ind = 384;

	for (i = 0; i < 256; i++)
	{
		clp[ind++] = i;
	}

	ind = 640;

	for (i = 0; i < 384; i++)
	{
		clp[ind++] = 255;
	}

	return;
}






void YUV2RGB420(unsigned char *src0, unsigned char *src1, unsigned char *src2, unsigned char *dst_ori, int width, int height)
{    
	int y1, y2, u, v;
	unsigned char *py1, *py2;

	int i, j, c1, c2, c3, c4;
	unsigned char *d1, *d2;

	//src0=src;
	//src1=src+width*height;
	//src2=src+width*height+width*height/4;

	py1 = src0;
	py2 = py1 + width;

	d1  = dst_ori;
	d2  = d1 + 3 * width;

	for (j = 0; j < height; j += 2) 
	{ 
		for (i = 0; i < width; i += 2) 
		{
			u = *src1++;
			v = *src2++;

			c1 = crv_tab[v];
			c2 = cgu_tab[u];
			c3 = cgv_tab[v];
			c4 = cbu_tab[u];

			//up-left
			y1 = tab_76309[*py1++];    
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];

			//down-left
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];

			//up-right
			y1 = tab_76309[*py1++];
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];

			//down-right
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
			*d2++ = clp[384+((y2 + c4)>>16)];
		}

		d1 += 3*width;
		d2 += 3*width;
		py1+=   width;
		py2+=   width;
	}   

	return;
}




//How to use:
//YUV_TO_RGB24(pY, width, pU, pV, width >> 1, pRGBBuf, width, (int)0-height, width * 3);
typedef   unsigned char      uint8_t;
typedef   unsigned long long uint64_t;

#define MAXIMUM_Y_WIDTH 800
static uint64_t mmw_mult_Y    = 0x2568256825682568;
static uint64_t mmw_mult_U_G  = 0xf36ef36ef36ef36e;
static uint64_t mmw_mult_U_B  = 0x40cf40cf40cf40cf;
static uint64_t mmw_mult_V_R  = 0x3343334333433343;
static uint64_t mmw_mult_V_G  = 0xe5e2e5e2e5e2e5e2;


static uint64_t mmb_0x10      = 0x1010101010101010;
static uint64_t mmw_0x0080    = 0x0080008000800080;
static uint64_t mmw_0x00ff    = 0x00ff00ff00ff00ff;

static uint64_t mmw_cut_red   = 0x7c007c007c007c00;
static uint64_t mmw_cut_green = 0x03e003e003e003e0;
static uint64_t mmw_cut_blue  = 0x001f001f001f001f;

#if !IS_APPLE
void YUV_TO_RGB24(uint8_t *puc_y,      int stride_y, 
				  uint8_t *puc_u,      uint8_t *puc_v, int stride_uv, 
				  uint8_t *puc_out,    int width_y,    int height_y,  int stride_out) 
{
	int y, horiz_count;
	uint8_t *puc_out_remembered;
	//int stride_out = width_y * 3;
    
	if (height_y < 0) 
	{
		//we are flipping our output upside-down
		height_y  = -height_y;
		puc_y     += (height_y   - 1) * stride_y ;
		puc_u     += (height_y/2 - 1) * stride_uv;
		puc_v     += (height_y/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	horiz_count = -(width_y >> 3);

	for (y = 0; y < height_y; y++) 
	{
		if (y == height_y - 1) 
		{
			//this is the last output line - we need to be careful not to overrun the end of this line
			uint8_t temp_buff[3 * MAXIMUM_Y_WIDTH + 1];

			puc_out_remembered = puc_out;
			puc_out = temp_buff; //write the RGB to a temporary store
		}

		//_asm {
		//	push eax
		//	push ebx
		//	push ecx
		//	push edx
		//	push edi

		//	mov eax, puc_out       
		//	mov ebx, puc_y       
		//	mov ecx, puc_u       
		//	mov edx, puc_v
		//	mov edi, horiz_count

		//	horiz_loop:

		//	movd mm2, [ecx]
		//	pxor mm7, mm7

		//	movd mm3, [edx]
		//	punpcklbw mm2, mm7       

		//	movq mm0, [ebx]          
		//	punpcklbw mm3, mm7       

		//	movq mm1, mmw_0x00ff     

		//	psubusb mm0, mmb_0x10    

		//	psubw mm2, mmw_0x0080    
		//	pand mm1, mm0            

		//	psubw mm3, mmw_0x0080    
		//	psllw mm1, 3             

		//	psrlw mm0, 8             
		//	psllw mm2, 3             

		//	pmulhw mm1, mmw_mult_Y   
		//	psllw mm0, 3             

		//	psllw mm3, 3             
		//	movq mm5, mm3            

		//	pmulhw mm5, mmw_mult_V_R 
		//	movq mm4, mm2            

		//	pmulhw mm0, mmw_mult_Y   
		//	movq mm7, mm1            

		//	pmulhw mm2, mmw_mult_U_G 
		//	paddsw mm7, mm5

		//	pmulhw mm3, mmw_mult_V_G
		//	packuswb mm7, mm7

		//	pmulhw mm4, mmw_mult_U_B
		//	paddsw mm5, mm0      

		//	packuswb mm5, mm5
		//	paddsw mm2, mm3          

		//	movq mm3, mm1            
		//	movq mm6, mm1            

		//	paddsw mm3, mm4
		//	paddsw mm6, mm2

		//	punpcklbw mm7, mm5
		//	paddsw mm2, mm0

		//	packuswb mm6, mm6
		//	packuswb mm2, mm2

		//	packuswb mm3, mm3
		//	paddsw mm4, mm0

		//	packuswb mm4, mm4
		//	punpcklbw mm6, mm2

		//	punpcklbw mm3, mm4

		//	// 32-bit shuffle.
		//	pxor mm0, mm0

		//	movq mm1, mm6
		//	punpcklbw mm1, mm0

		//	movq mm0, mm3
		//	punpcklbw mm0, mm7

		//	movq mm2, mm0

		//	punpcklbw mm0, mm1
		//	punpckhbw mm2, mm1

		//	// 24-bit shuffle and sav
		//	movd   [eax], mm0
		//	psrlq mm0, 32

		//	movd  3[eax], mm0

		//	movd  6[eax], mm2


		//	psrlq mm2, 32            

		//	movd  9[eax], mm2        

		//	// 32-bit shuffle.
		//	pxor mm0, mm0            

		//	movq mm1, mm6            
		//	punpckhbw mm1, mm0       

		//	movq mm0, mm3            
		//	punpckhbw mm0, mm7       

		//	movq mm2, mm0            

		//	punpcklbw mm0, mm1       
		//	punpckhbw mm2, mm1       

		//	// 24-bit shuffle and sav
		//	movd 12[eax], mm0        
		//	psrlq mm0, 32            

		//	movd 15[eax], mm0        
		//	add ebx, 8               

		//	movd 18[eax], mm2        
		//	psrlq mm2, 32            

		//	add ecx, 4               
		//	add edx, 4               

		//	movd 21[eax], mm2        
		//	add eax, 24              

		//	inc edi
		//	jne horiz_loop

		//	pop edi
		//	pop edx
		//	pop ecx
		//	pop ebx
		//	pop eax

		//	emms
		//}

		if (y == height_y-1) 
		{
			//last line of output - we have used the temp_buff and need to copy
			int x = 3 * width_y;                  //interation counter
			uint8_t *ps = puc_out;                // source pointer (temporary line store)
			uint8_t *pd = puc_out_remembered;     // dest pointer
			while (x--) *(pd++) = *(ps++);        // copy the line
		}

		puc_y += stride_y;

		if (y % 2) 
		{
			puc_u += stride_uv;
			puc_v += stride_uv;
		}

		puc_out += stride_out; 
	}

	return;
}

#endif
