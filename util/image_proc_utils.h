
#ifndef _IMAGE_PROC_UTILS_INCLUDE_H_
#define _IMAGE_PROC_UTILS_INCLUDE_H_

#include "base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define THRESH_BINARY 0
#define THRESH_BINARY_INV 1


// buffer size: imageWidth * (imageHeight - vanishLine + 1) * sizeof(int)
DLL_API void jmMeanFilter(unsigned char* pSrcImage, unsigned char* pDstImage, int* sumBuffer,
	int imageWidth, int imageHeight, int vanishLine, int size);

// buffer size: imageWidth*(imageHeight-vanishLine+1)*sizeof(int)+imageWidth*imageHeight*sizeof(unsigned char)
// binaryType should be THRESH_BINARY or THRESH_BINARY_INV
DLL_API void jmAdaptiveThreshold(unsigned char* pSrcImage, unsigned char* pDstImage, unsigned char* buffer,
	int imageWidth, int imageHeight, int vanishLine, int blockSize, int C, int binaryType, unsigned char maxVal);

DLL_API void jmEqualizeHist(unsigned char* pSrcImage, unsigned char* pDstImage,
	int imageWidth, int imageHeight, int vanishLine);

DLL_API void jmLdwBitwise_and(unsigned char* pSrcimage, unsigned char* pDstImage, 
	int imageWidth, int imageHeight, int vanishLine);

DLL_API void jmAvgSdv(PBYTE image, PBYTE warpedMask, int width, int height, float* avg, float* std_dev);

#ifdef __cplusplus
}
#endif


#endif