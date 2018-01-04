#ifndef _CONTOURS_H_
#define _CONTOURS_H_

#include "base_type.h"
#include "mem.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CONTOURS_EXTERNAL 0
#define CONTOURS_LIST     1

#define SEQ_FLAG_EXTERNAL   0
#define SEQ_FLAG_HOLE       1




DLL_API CV_SEQ *FindContours(BYTE *pu8Img, int image_w, int image_h, int mode, MEMORY_BUFFER *storage);

DLL_API CV_RECT BoundingRect(CV_SEQ *ptSeq);

DLL_API void DestroySeq(CV_SEQ *ptSeq, MEMORY_BUFFER *storage);


#ifdef __cplusplus
}
#endif



#endif
