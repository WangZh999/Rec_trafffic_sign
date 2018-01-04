
#include <memory.h>
#include "mem.h"
#include <string.h>

/******************************************************************************
* ��  �ܣ����ڴ�mem_buffer�з���һ���СΪsize���ڴ��

* ��  ����IN:
		  p_mem_buffer    -----------------------------�ڴ����ṹ��ָ��
          size            -----------------------------��������ڴ��С

* ����ֵ������õ����ڴ�λ�õ�ָ��
******************************************************************************/
void *alloc_memory(MEMORY_BUFFER *p_mem_buffer, int size)
{
	void *buffer;
	unsigned long long free_size;

	//unsigned long long ab32 = (~(ALIGN_BYTE_32 - 1));

	// �ڴ���п����ڴ���ʼλ��
	buffer = (void*)(((unsigned long long)p_mem_buffer->cur_pos + (ALIGN_BYTE_32 - 1)) & (~(ALIGN_BYTE_32 - 1)));
	//buffer = (void*)(((unsigned long)p_mem_buffer->cur_pos + (ALIGN_BYTE_32 - 1)) & (~(ab32 - 1)));

	// �����ڴ��еĿ���ռ�Ĵ�С
	free_size = (unsigned long long)p_mem_buffer->end_pos - (unsigned long long)buffer;

	//���ʣ����ڴ�ռ䲻�������ؿ�ָ��
	if (free_size < size)
	{
		buffer = 0;
	}
	else
	{
		//�Է���õ��ڴ�����
 		memset(buffer, 0, size);

		//����ʣ���ڴ���ʼλ��
		p_mem_buffer->cur_pos = (void*)((unsigned long long)buffer + size);
	}

	return buffer;
}

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
void memory_copy2d(unsigned char *des,                  
				   unsigned char *src,                  
				   int            des_stride,          
				   int            src_stride,         
				   int            copy_bytes_once,    
				   int            copy_num)
{
	int  x;
	unsigned char  *ptr_src, *ptr_des;

	ptr_src = src;
	ptr_des = des;

	for (x = 0; x < copy_num; x++)
	{
		memcpy(ptr_des, ptr_src, copy_bytes_once);

		ptr_des += des_stride;
		ptr_src += src_stride;
	}

	return;
}