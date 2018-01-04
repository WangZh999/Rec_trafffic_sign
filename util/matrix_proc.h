
#ifndef _MATRIX_PROC_INCLUDE_H_
#define _MATRIX_PROC_INCLUDE_H_

#include "base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


#include <math.h>


//科学运算相关
#define EPS      (0.000001f)

//SVD分解的迭代次数
#define MAX_ITERA   100


//矩阵定义
typedef struct _MATRIX_
{
    double *matrix_data;
	int     matrix_rows;
	int     matrix_cols;
}MATRIX;



/******************************************************************************
* 功  能：对矩阵进行奇异值分解，参见《c 常用算法程序集》徐世良P169

* 参  数：IN:


* 返回值：
******************************************************************************/
DLL_API int  MATRIX_PROC_svd_decompose(MATRIX    *matrix_A,
							   MATRIX    *matrix_U,
							   MATRIX    *matrix_sigma,
							   MATRIX    *matrix_V,
							   double       *s,
							   double       *e,
							   double       *w,
							   double        eps);

/******************************************************************************
* 功  能：矩阵相乘

* 参  数：IN:
             matrix_C = matrix_A * matrix_B

* 返回值：
******************************************************************************/
DLL_API void MATRIX_PROC_multiply(MATRIX    *matrix_A,
                          MATRIX    *matrix_B,
						  MATRIX    *matrix_C);

/******************************************************************************
* 功  能：矩阵求逆运算

* 参  数：IN:
             matrix  -------------------------  输入矩阵
             row     -------------------------  行缓存
             col     -------------------------  列缓存

* 返回值：
******************************************************************************/
DLL_API void MATRIX_PROC_invert(MATRIX    *matrix,
				        int          *row, 
				        int          *col);

/******************************************************************************
* 功  能：矩阵转置运算

* 参  数：IN:
			matrix_A   -------------------------  输入矩阵
			matrix_At  -------------------------  转置后的矩阵

* 返回值：
******************************************************************************/
DLL_API void MATRIX_PROC_transpose(MATRIX *matrix_A, MATRIX *matrix_At);



#ifdef __cplusplus
}
#endif


#endif
