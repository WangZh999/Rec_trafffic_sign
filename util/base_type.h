

#ifndef _BASE_TYPE_INCLUDE_H_
#define _BASE_TYPE_INCLUDE_H_


#ifdef __cplusplus
extern "C"
{
#endif


#ifndef DLL_API_DEFAULT
#define DLL_API_DEFAULT				__attribute__((visibility("default")))
#endif
#ifndef DLL_API_HIDDEN
#define DLL_API_HIDDEN				/*__attribute__((visibility("hidden")))*/
#endif

#ifndef DLL_LOCAL
#ifdef WIN32
#define DLL_LOCAL _declspec(dllimport)
#else
#define DLL_LOCAL  	__attribute__ ((visibility("hidden")))
#endif
#else
//#pragma message("--------base_type.h already defined DLL_LOCAL" )
#endif

#undef DLL_API
#ifndef DLL_API
#ifdef WIN32
#define DLL_API  _declspec(dllexport)
#else
#define DLL_API 	__attribute__ ((visibility("default")))
#endif
#else
//#pragma message("--------base_type.h already defined DLL_API")
#endif


//常用宏定义
#ifndef PI
#define PI  3.14159265358979323846f
#endif

#ifndef RECT_EPS
#define RECT_EPS 308
#endif


#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)     (((a) > 0) ? (a) : (-(a)))
#endif

#ifndef SWAP
#define SWAP(a, b, t) ((t) = (a), (a) = (b), (b) = (t))
#endif

#ifndef CLIP
#define CLIP(x, minval, maxval)  (x) < (minval) ? (minval) : ((x) > (maxval) ? (maxval) : (x))
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif


#if !defined(DWORD) && !defined(_DWORD_)
#ifdef _WIN32
typedef unsigned long		DWORD;
#else
typedef unsigned int		DWORD;
#endif
#define _DWORD_				  DWORD
#endif

#ifndef BOOL
typedef int                 BOOL;
#endif

#ifndef BYTE
typedef unsigned char       BYTE;
#endif

#ifndef PBYTE
typedef unsigned char*      PBYTE;
#endif

#ifndef WORD
typedef unsigned short      WORD;
#endif

//错误校验
#define CHECK_ERROR(state, error_code) \
if (state)                             \
{                                      \
	return error_code;                 \
}


//常用结构体定义
//点[整型]
typedef struct _POINT_
{
	int x;
	int y;
}CV_POINT;

//点[浮点]
typedef struct _POINT_F_
{
	float x; 
	float y;
}POINT_F;

//块
typedef struct _BLOB_
{
	int left;
	int right;
	int top;
	int bottom;
}CV_BLOB;

//块[浮点]
typedef struct _BLOB_F_
{
	float left;
	float right;
	float top;
	float bottom;
}BLOB_F;

//LDW
typedef struct _LANE_INFO_
{
	CV_POINT left_lane[2];
	CV_POINT right_lane[2];
	int left_valid;
	int right_valid;
}LANE_INFO;

//获得区域块的宽度和高度
#define BLOB_WIDTH(_blob)  ( (_blob)->right - (_blob)->left + 1 )
#define BLOB_HEIGHT(_blob) ( (_blob)->bottom - (_blob)->top + 1 )

//矩形
typedef struct _RECT_
{
	int x;
	int y;
	int width;
	int height;
}CV_RECT;

//矩形[浮点]
typedef struct _RECT_F_
{
	float x;
	float y;
	float width;
	float height;
}RECT_F;

//SIZE
typedef struct _SIZE_
{
	int   width;
	int   height;
}CV_SIZE;

//多边形的顶点表示
//多边形
#define MAX_VERTEX  20    //多边形的最多顶点数

typedef struct _POLYGON_
{
	int       vertex_num;             //顶点数
	CV_POINT  point[MAX_VERTEX + 1];  //顶点    //加1是为了闭合多边形
}POLYGON;

//顶点坐标归一化的多边形
typedef struct _POLYGON_F_
{
	int      vertex_num;             //顶点数
	POINT_F  point[MAX_VERTEX + 1];  //顶点    //加1是为了闭合多边形
}POLYGON_F;

//线段结构
typedef struct _LINE_
{
	int      rho;          //极坐标形式下的线段参数
	int      angle;        //极坐标形式下的线段参数
	CV_POINT line_start;   //线段起始点      
	CV_POINT line_end;     //线段终点
	int      confidence;   //该直线的置信度
}LINE;


typedef struct LIST
{
	void *ptData;
	struct LIST *ptNext;
}LIST;

typedef struct SEQ_LIST
{
	LIST *ptHead;
	LIST *ptTail;
	int total;
}SEQ_LIST;



typedef struct _OBJ_RECT_
{
	CV_BLOB r;
	int     neighbors;
	int     light_on;
	float   scaleshift;
}OBJ_RECT;

//融合后的目标矩形区域队列
#ifndef MAX_OBJECT_RECT_NUM
#define MAX_OBJECT_RECT_NUM 200
#endif

typedef struct _OBJ_RECT_LIST_
{
	int      rect_num;
	OBJ_RECT rect[MAX_OBJECT_RECT_NUM];
}OBJ_RECT_LIST;

//单帧图像中最多检测到的矩形区域数目
#ifndef MAX_RECT_NUM
#define MAX_RECT_NUM	 200
#endif

typedef struct _RECT_LIST_
{
	int      rect_num;
	OBJ_RECT rect[MAX_RECT_NUM];
}RECT_LIST;


#define TREE_NODE_FIELDS(node_type)                               \
	int       flags;             /**< Miscellaneous flags.     */      \
	int       header_size;       /**< Size of sequence header. */      \
struct    node_type* h_prev; /**< Previous sequence.       */      \
struct    node_type* h_next; /**< Next sequence.           */      \
struct    node_type* v_prev; /**< 2nd previous sequence.   */      \
struct    node_type* v_next  /**< 2nd next sequence.       */

/**
Read/Write sequence.
Elements can be dynamically inserted to or deleted from the sequence.
*/
typedef struct CV_SEQ
{
	TREE_NODE_FIELDS(CV_SEQ);
	SEQ_LIST list;
	CV_RECT rect;
}CV_SEQ;

//图像数据存储格式定义
#define PIXEL_FORMAT_UNKNOWN       (0)  //reserved
#define PIXEL_FORMAT_BGR           (1)  //B0G0R0B1G0R0
#define PIXEL_FORMAT_GRAY          (2)  //G0G1G2G3
#define PIXEL_FORMAT_BLOCK_YUV     (3)  //Y0Y1Y2...,  U0U1U2..., V0V1V2...
#define PIXEL_FORMAT_YPUPVP_444    (4)  //pRaw[0] pointers to Y Block; pRaw[1] pointers to U Block; pRaw[2] pointers to V Block
#define PIXEL_FORMAT_BLOCK_YUV_422 (5)  //Y0Y1Y2..., U01U23..., V01V23... 
#define PIXEL_FORMAT_BLOCK_YUV_420 (6)  //Y0Y1Y2..., U0U1..., V0V1...  UV数据的宽高为Y的一半, YUV三个分量的首地址分别对应pRaw[3]的值


//图像数据结构体
typedef struct _IMAGE
{
	int    image_w;
	int    image_h;
	int    wideStep;
	int    pix_format;
	PBYTE  imageData[3]; //二维数组使用方式取决于像素格式
}IMAGE;



#ifdef __cplusplus
}
#endif


#endif

