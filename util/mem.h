
#ifndef __MEMORY_INCLUDE_H__
#define __MEMORY_INCLUDE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "base_type.h"

//�ڴ���뷽ʽ
#define ALIGN_BYTE_32   ((unsigned long long)(32))  

//�ڴ�ṹ��
typedef struct _MEMORY_BUFFER_
{
	void*  start_pos;    //�ڴ���ʼ��ַ
	void*  end_pos;      //�ڴ������ַ
	void*  cur_pos;      //ʣ���ڴ����ʼλ��
}MEMORY_BUFFER;

/******************************************************************************
* ��  �ܣ����ڴ�mem_buffer�з���һ���СΪsize���ڴ��

* ��  ����IN:
		  p_sw_mem_buffer -----------------------------�ڴ����ṹ��ָ��
          size            -----------------------------��������ڴ��С

* ����ֵ������õ����ڴ�λ�õ�ָ��
******************************************************************************/
DLL_API void *alloc_memory(MEMORY_BUFFER *p_mem_buffer, int size);

/******************************************************************************
* ��  �ܣ�����2D��������ڴ��

* ��  ����IN:
		  des        ----------------------------- Ŀ����ʼ��
          src        ----------------------------- ָ�򿽱���ʼ��
		  des_stride ----------------------------- buffer���
          src_stride ----------------------------- frame���
          copy_bytes_once  ----------------------- һ�ο������ֽ���
          copy_num   ----------------------------- ��������

* ����ֵ������õ����ڴ�λ�õ�ָ��
******************************************************************************/
DLL_API void memory_copy2d(unsigned char *des,                  
				   unsigned char *src,                  
				   int            des_stride,          
				   int            src_stride,         
				   int            copy_bytes_once,    
				   int            copy_num);


#ifdef __cplusplus
}
#endif


#endif