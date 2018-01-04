
#ifndef __MEMORY_INCLUDE_H__
#define __MEMORY_INCLUDE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "base_type.h"

//内存对齐方式
#define ALIGN_BYTE_32   ((unsigned long long)(32))  

//内存结构体
typedef struct _MEMORY_BUFFER_
{
	void*  start_pos;    //内存起始地址
	void*  end_pos;      //内存结束地址
	void*  cur_pos;      //剩余内存的起始位置
}MEMORY_BUFFER;

/******************************************************************************
* 功  能：在内存mem_buffer中分配一块大小为size的内存块

* 参  数：IN:
		  p_sw_mem_buffer -----------------------------内存管理结构体指针
          size            -----------------------------待分配的内存大小

* 返回值：分配得到的内存位置的指针
******************************************************************************/
DLL_API void *alloc_memory(MEMORY_BUFFER *p_mem_buffer, int size);

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