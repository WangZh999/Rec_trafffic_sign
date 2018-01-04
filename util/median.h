

#ifndef MEDIAN_H_
#define MEDIAN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "base_type.h"


//计算数组的中值，通过保留数组的副本维持输入数组不变
DLL_API int get_median_value_unchange(int arr[], int n, void *buff);


//获取数组的中值，数组中元素的顺序被更改
DLL_API int get_median_value_changed(int arr[], int n);




#ifdef __cplusplus
}
#endif



#endif
