#ifndef _COMPUTOR_GEOMETY_INCLUDE_H_
#define _COMPUTOR_GEOMETY_INCLUDE_H_

#include "base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif



//判断线段p1p2与线段p3p4是否相交
DLL_API int segments_intersert(CV_POINT *p1, CV_POINT *p2, CV_POINT *p3, CV_POINT *p4);


//点是否在多边形区域内
DLL_API int point_in_polygon(CV_POINT *pt, POLYGON *polygon);


//计算线段p1p2与线段p3p4的交点
DLL_API CV_POINT two_line_cross_pt(CV_POINT *pt1, CV_POINT *pt2, CV_POINT *pt3, CV_POINT *pt4);

//两点之间的距离平方
DLL_API int two_point_distance(CV_POINT *pt1, CV_POINT *pt2);

//点到直线的距离平方
DLL_API int point_to_line_distance(CV_POINT *pt, CV_POINT *pt1, CV_POINT *pt2);



//生成多边形mask,多边形定点坐标为归一化的
DLL_API void generate_det_mask(PBYTE pmask, POLYGON detect_region, int image_w, int image_h);







#ifdef __cplusplus
}
#endif



#endif
