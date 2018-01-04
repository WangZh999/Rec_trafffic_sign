
#include <memory.h>
#include "mem.h"
#include <string.h>

/******************************************************************************
* 功  能：在内存mem_buffer中分配一块大小为size的内存块

* 参  数：IN:
		  p_mem_buffer    -----------------------------内存管理结构体指针
          size            -----------------------------待分配的内存大小

* 返回值：分配得到的内存位置的指针
******************************************************************************/
void *alloc_memory(MEMORY_BUFFER *p_mem_buffer, int size)
{
	void *buffer;
	unsigned long long free_size;

	//unsigned long long ab32 = (~(ALIGN_BYTE_32 - 1));

	// 内存块中空余内存起始位置
	buffer = (void*)(((unsigned long long)p_mem_buffer->cur_pos + (ALIGN_BYTE_32 - 1)) & (~(ALIGN_BYTE_32 - 1)));
	//buffer = (void*)(((unsigned long)p_mem_buffer->cur_pos + (ALIGN_BYTE_32 - 1)) & (~(ab32 - 1)));

	// 计算内存中的空余空间的大小
	free_size = (unsigned long long)p_mem_buffer->end_pos - (unsigned long long)buffer;

	//如果剩余的内存空间不够，返回空指针
	if (free_size < size)
	{
		buffer = 0;
	}
	else
	{
		//对分配好的内存清零
 		memset(buffer, 0, size);

		//更新剩余内存起始位置
		p_mem_buffer->cur_pos = (void*)((unsigned long long)buffer + size);
	}

	return buffer;
}

/******************************************************************************
* 功  能：拷贝2D子区域的内存块

* 参  数：IN:
		  des        ----------------------------- 目标起始点
          src        ----------------------------- 指向拷贝起始点
		  des_stride ----------------------------- buffer宽度
          src_stride ----------------------------- frame宽度
          copy_bytes_once  ----------------------- 一次拷贝的字节数
          copy_num   ----------------------------- 拷贝次数

* 返回值：分配得到的内存位置的指针
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