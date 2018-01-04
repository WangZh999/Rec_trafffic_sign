

#ifndef _OSD_H_
#define _OSD_H_

#include "base_type.h"

#define ERR_OK    0
#define ERR_FAIL -1

typedef char			    int8;
typedef unsigned char	    uint8;
typedef short int 		    int16;
typedef unsigned short int 	uint16;
typedef int 			    int32;
typedef unsigned int 		uint32;


#ifdef __cplusplus
extern "C" {
#endif 


#define FORMAT_YV12  0
#define FORMAT_NV21  1
#define FORMAT_I420P 2
#define FORMAT_NV12  3

DLL_API void RGB_to_YCbCr(int R, int G, int B, int *Y, int *Cb, int *Cr);

DLL_API int MerageStringEx(unsigned char *pYUV,
				  int            nPicWidth,
				  int            nPicHeight,
				  unsigned char *Str,
				  int            start_x,
				  int            start_y,
				  int            fontWidth,
				  int            R,
				  int            G,
				  int            B,
				  int			 format);

DLL_API int MerageString(unsigned char *pYUV, 
				  int            nPicWidth, 
				  int            nPicHeight, 
				  unsigned char *Str, 
				  int            nStartPixelX, 
				  int            nStartPixelY,
				  int            fontWidth,
				  int            R,
				  int            G,
				  int            B);

DLL_API void MerageRect(unsigned char *pImage, 
				int            nSrcWidth, 
				int            nSrcHeight, 
				int            nRectStartX,
				int            nRectStartY,
				int            nRectWidth, 
				int            nRectHeight,
				int            R,
				int            G,
				int            B);

DLL_API void MerageRectYchannel(unsigned char *pImageY, 
						int            nWidth, 
						int            nHeight, 
						int            nRectStartX,
						int            nRectStartY,
						int            nRectWidth, 
						int            nRectHeight,
						int            yVal);

DLL_API void MarkPoint(unsigned char *pImage, 
			   int            nWidth, 
			   int            nHeight, 
			   int            X,
			   int            Y,
			   int            R,
			   int            G,
			   int            B);

DLL_API void MerageLine(unsigned char *pyuv420, 
				int            image_w, 
				int            image_h, 
				int            x1, 
				int            y1, 
				int            x2, 
				int            y2,
				int            thickness,
				unsigned char  r, 
				unsigned char  g, 
				unsigned char  b);


#ifdef __cplusplus
};
#endif 


#endif