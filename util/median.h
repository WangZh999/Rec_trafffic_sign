

#ifndef MEDIAN_H_
#define MEDIAN_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "base_type.h"


//�����������ֵ��ͨ����������ĸ���ά���������鲻��
DLL_API int get_median_value_unchange(int arr[], int n, void *buff);


//��ȡ�������ֵ��������Ԫ�ص�˳�򱻸���
DLL_API int get_median_value_changed(int arr[], int n);




#ifdef __cplusplus
}
#endif



#endif
