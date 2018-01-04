
#ifndef TSR_H_
#define TSR_H_

#include "base_type.h"



DLL_API void *TSR_DET_Create(int image_w, int image_h);

DLL_API void TSR_DET_Destroy(void *phandle);

DLL_API int TSR_DET_run(void *phandle, PBYTE yuv420, int image_w, int image_h);

#endif
