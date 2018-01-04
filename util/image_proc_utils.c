
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "image_proc_utils.h"

// buffer size: imageWidth * (imageHeight - vanishLine + 1) * sizeof(int)
void jmMeanFilter(unsigned char* pSrcImage, unsigned char* pDstImage, int* sumBuffer,
	int imageWidth, int imageHeight, int vanishLine, int size)
{
	int row, col, i, value, *colSum, bufferRow;
	int scale = size * size, halfScale = scale >> 1;
#define get_row_index(i) ( ( (i) < 0 ) ? 0 : ( (i) >= imageHeight ) ? (imageHeight - 1) : (i) )
#define get_col_index(i) ( ( (i) < 0 ) ? 0 : ( (i) >= imageWidth ) ? (imageWidth - 1) : (i) )

	// ÐÐÂË²¨
	for (row = vanishLine - size / 2, bufferRow = 0; row < imageHeight + size / 2; row++, bufferRow++)
	{
		unsigned char* pSrcRow = pSrcImage + get_row_index(row) * imageWidth;
		int* pBuffer = sumBuffer + bufferRow * imageWidth;
		for (value = 0, i = -size / 2; i <= size / 2; i++)
			value += pSrcRow[get_col_index(i)];

		pBuffer[0] = value;
		for (col = 1; col < imageWidth; col++)
		{
			int colNext = col + size / 2, colPrevious = col - size / 2 - 1;
			value += pSrcRow[get_col_index(colNext)] - pSrcRow[get_col_index(colPrevious)];
			pBuffer[col] = value;
		}
	}

	// ÁÐÂË²¨
	colSum = sumBuffer + bufferRow * imageWidth;
	memset(colSum, 0, sizeof(int) * imageWidth);
	for (i = 0; i < size - 1; i++)
	{
		int* pBufferLine = sumBuffer + i * imageWidth;
		for (col = 0; col < imageWidth; col++)
			colSum[col] += pBufferLine[col];
	}

	for (row = vanishLine, bufferRow = size / 2; row < imageHeight; row++, bufferRow++)
	{
		unsigned char* pDstRow = pDstImage + row * imageWidth;
		int addRow = bufferRow + size / 2, subRow = bufferRow - size / 2;
		int* addLine = sumBuffer + addRow * imageWidth;
		int* subLine = sumBuffer + subRow * imageWidth;
		for (col = 0; col < imageWidth; col++)
		{
			value = colSum[col] + addLine[col];
			pDstRow[col] = (unsigned char)((value + halfScale) / (scale));
			colSum[col] = value - subLine[col];
		}
	}
#undef get_row_index
#undef get_col_index
}

// buffer size: imageWidth * (imageHeight - vanishLine + 1) * sizeof(int) + imageWidth * imageHeight * sizeof(unsigned char)
void jmAdaptiveThreshold(unsigned char* pSrcImage, unsigned char* pDstImage, unsigned char* buffer,
	int imageWidth, int imageHeight, int vanishLine, int blockSize, int C, int binaryType, unsigned char maxVal)
{
	unsigned char* pMeanImage = buffer;
	unsigned char* sumBuffer = buffer + imageWidth * imageHeight;
	unsigned char tab[768];
	int i, begin = vanishLine * imageWidth, end = imageHeight * imageWidth;
	unsigned char upVal = maxVal, downVal = 0;
	if (binaryType == THRESH_BINARY_INV) {
		upVal = 0;
		downVal = maxVal;
	}
		
	jmMeanFilter(pSrcImage, pMeanImage, (int*)sumBuffer, imageWidth, imageHeight, vanishLine, blockSize);
	for (i = 0; i < 768; i++) {
		tab[i] = i - 255 > -C ? upVal : downVal;
	}
	for (i = begin; i < end; i++)
		pDstImage[i] = tab[255 + pSrcImage[i] - pMeanImage[i]];
}

void jmEqualizeHist(unsigned char* pSrcImage, unsigned char* pDstImage,
	int imageWidth, int imageHeight, int vanishLine)
{
#define HIST_SIZE 256
	int hist[HIST_SIZE] = { 0, };
	int lut[HIST_SIZE] = { 0, };
	int begin = vanishLine * imageWidth, end = imageWidth * imageHeight;
	int i, length = end - begin, sum;
	float scale;

	for (i = begin; i < end; i++)
		hist[pSrcImage[i]]++;

	i = 0;
	while (!hist[i]) ++i;

	if (hist[i] == length)
	{
		memset(pDstImage + begin, (unsigned char)i, length);
		return;
	}

	scale = (HIST_SIZE - 1.f) / (length - hist[i]);
	sum = 0;
	for (lut[i++] = 0; i < HIST_SIZE; ++i)
	{
		sum += hist[i];
		lut[i] = (unsigned char)(sum * scale + 0.5);
	}

	for (i = begin; i < end; i++)
	{
		pDstImage[i] = (unsigned char)lut[pSrcImage[i]];
	}
#undef HIST_SIZE
}

void jmLdwBitwise_and(unsigned char* pSrcimage, unsigned char* pDstImage, int imageWidth,
	int imageHeight, int vanishLine)
{
	int begin = vanishLine * imageWidth, end = imageHeight * imageWidth;
	for (; begin < end; begin++)
		pDstImage[begin] &= pSrcimage[begin];
}

void jmAvgSdv(PBYTE image, PBYTE warpedMask, int width, int height, float* avg, float* std_dev)
{
	int i, j, count = 0;
	int size = width*height;

	*avg = 0;
	*std_dev = 0;
	if (warpedMask == NULL) 
	{
		for (i = 0; i < height; i++)
		{
			for (j = width*2/5; j <= width*3/5; j++)
			{
				*avg += image[i*width+j];
				count++;
			}
		}
		*avg /= count;
		for (i = 0; i < size; i++) {
			*std_dev += (image[i] - *avg) * (image[i] - *avg);
		}
	}
	else 
	{
		for (i = 0; i < size; i++) 
		{
			if (warpedMask[i]) 
			{
				count++;
				*avg += image[i];
			}
		}
		*avg /= count;

		for (i = 0; i < size; i++) 
		{
			if (warpedMask[i]) 
			{
				*std_dev += (image[i] - *avg) * (image[i] - *avg);
			}
		}
	}

	*std_dev /= count;
	*std_dev = sqrtf(*std_dev);
	return ;
}
