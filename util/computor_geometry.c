
#include <memory.h>
#include "computor_geometry.h"
#include <string.h>

//利用叉积计算点相对线段的方位
int is_left( CV_POINT *P0, CV_POINT *P1, CV_POINT *P2 )
{
	return ( (P1->x - P0->x) * (P2->y - P0->y)	- (P2->x - P0->x) * (P1->y - P0->y) );
}

//点是否在多边形区域内
int point_in_polygon(CV_POINT *pt, POLYGON *polygon)
{
	int i, wn;    //wn: the winding number counter
	CV_POINT *polygom_pt1, *polygom_pt2;

	wn = 0; 

	// loop through all edges of the polygon
	for (i = 0; i < polygon->vertex_num - 1; ++i)
	{
		polygom_pt1 = &polygon->point[i];
		polygom_pt2 = &polygon->point[i + 1];

		if (polygom_pt1->y <= pt->y)    // start y <= pt->y
		{        
			if (polygom_pt2->y > pt->y)      // an upward crossing
			{
				if (is_left( polygom_pt1, polygom_pt2, pt) > 0)  // P left of edge
				{
					++wn;            // have a valid up intersect
				}
			}
		}
		else    // start y > P.y (no test needed)
		{                      
			if (polygom_pt2->y <= pt->y)     // a downward crossing
			{
				if (is_left( polygom_pt1, polygom_pt2, pt) < 0)  // P right of edge
				{
					--wn;            // have a valid down intersect
				}
			}
		}
	}

	return ((wn == 0)) ? 0 : 1;
}

//确定与线段p1p2共线的点p是否在线段p1p2上
int on_segment( CV_POINT *p1, CV_POINT *p2, CV_POINT *p )
{
	int max_x, min_x, max_y, min_y;

	max_x = MAX(p1->x, p2->x);
	min_x = MIN(p1->x, p2->x);
	max_y = MAX(p1->y, p2->y);
	min_y = MIN(p1->y, p2->y);

	if( p->x  >= min_x && p->x <= max_x && p->y >= min_y && p->y <= max_y)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//判断线段p1p2与线段p3p4是否相交
int segments_intersert(CV_POINT *p1,
					   CV_POINT *p2,
					   CV_POINT *p3,
					   CV_POINT *p4)
{
	int d1, d2, d3, d4;

	d1 = is_left (p3, p4, p1);
	d2 = is_left (p3, p4, p2);
	d3 = is_left (p1, p2, p3);
	d4 = is_left (p1, p2, p4);

	//int型两个数相乘有可能越界
	if( ((float)(d1) * (float)(d2) < 0) && ((float)(d3) * (float)(d4) < 0))
	{
		return 1;
	}
	else if(d1== 0 && on_segment(p3, p4, p1) == 1)
	{
		return 1;
	}
	else if(d2== 0 && on_segment(p3, p4, p2) == 1)
	{
		return 1;
	}
	else if(d3== 0 && on_segment(p1, p2, p3) == 1)
	{
		return 1;
	}
	else if(d4 == 0 && on_segment(p1, p2, p4) == 1)
	{
		return 1;
	}

	return 0;
}

//计算直线p1p2与直线p3p4的交点
CV_POINT two_line_cross_pt(CV_POINT *pt1, 
						   CV_POINT *pt2, 
						   CV_POINT *pt3, 
						   CV_POINT *pt4)
{
	CV_POINT cross_pt;

	int x1 = pt1->x;
	int y1 = pt1->y;
	int x2 = pt2->x;
	int y2 = pt2->y;   

	int x3 = pt3->x;
	int y3 = pt3->y;
	int x4 = pt4->x;
	int y4 = pt4->y;  

	float val1, val2, val3, val4;

	cross_pt.x = 0;
	cross_pt.y = 0;

	val1 = (float)((x1 - x2) * (x3 * y4 - x4 * y3) - (x3 - x4) * (x1 * y2 - x2 * y1));
	val2 = (float)((x3 - x4) * (y1 - y2) - (x1 - x2) * (y3 - y4));  

	val3 = (float)((y1 - y2) * (x3 * y4 - x4 * y3) - (x1 * y2 - x2 * y1) * (y3 - y4));
	val4 = (float)((y1 - y2) * (x3 - x4) - (x1 - x2) * (y3 - y4)); 

	if (val2 != 0 && val4 != 0)
	{
		cross_pt.x = (int)(val1 / val2);  
		cross_pt.y = (int)(val3 / val4); 
	}
	else
	{
		cross_pt.x = 0;  
		cross_pt.y = 0; 
	}

	return  cross_pt;
}


//两点之间的距离平方
int two_point_distance(CV_POINT *pt1, CV_POINT *pt2)
{
    int dist_x, dist_y, dist;

	dist_x = pt1->x - pt2->x;
	dist_y = pt1->y - pt2->y;

	dist = dist_x * dist_x + dist_y * dist_y;

    return dist;
}

//点到直线的距离平方
int   point_to_line_distance(CV_POINT *pt,
						     CV_POINT *pt1,
						     CV_POINT *pt2)
{
	if (pt1->x != pt2->x)
	{
		float k    = (float)(pt1->y - pt2->y) / (float)(pt1->x - pt2->x);
		float bias = 0;
		int dist   = 0;

		float val1, val2;

		CV_POINT tmp_pt, tmp_pt1, tmp_pt2;

		tmp_pt1.x = 0; tmp_pt1.y = 0;
		tmp_pt2.x = 0; tmp_pt2.y = 300;

		tmp_pt = two_line_cross_pt(&tmp_pt1, &tmp_pt2, pt1, pt2);
		bias   = tmp_pt.y;

		val1 = k * pt->x + bias - pt->y;
		val2 = (1.0 + k * k);

		dist = (int)(val1 * val1 / val2);

		return dist;
	}
	else
	{
		int  val = (pt->x - pt1->x);
	    return val * val;
	}
}


//生成多边形mask,多边形定点坐标为归一化的
void generate_det_mask(PBYTE pmask, POLYGON detect_region, int image_w, int image_h)
{
	int i, j;

	memset(pmask, 0, image_w * image_h);

	if (detect_region.vertex_num > 0)
	{
        int left = image_w, right = 0, top = image_h, bottom = 0;

		for (i = 0; i < detect_region.vertex_num; ++i)
		{
			detect_region.point[i].x = (detect_region.point[i].x * image_w) >> 10;
			detect_region.point[i].y = (detect_region.point[i].y * image_h) >> 10;

			left   = MIN(left, detect_region.point[i].x);
			right  = MAX(right, detect_region.point[i].x);
			top    = MIN(top, detect_region.point[i].y);
			bottom = MAX(bottom, detect_region.point[i].y);
		}

		for (i = top; i < bottom; ++i)
		{
            int   start = 0;
            int   end = 0;
			PBYTE ptr;

			for (j = left; j < right; ++j)
			{
				CV_POINT pt;

				pt.x = j;
				pt.y = i;

				if (point_in_polygon(&pt, &detect_region))
				{
					start = j;
					break;
				}
			}

			if (j == right)
			{
				continue;
			}

			for (j = right; j > start; --j)
			{
				CV_POINT pt;

				pt.x = j;
				pt.y = i;

				if (point_in_polygon(&pt, &detect_region))
				{
					end = j;
					break;
				}
			}

			if (j == start)
			{
				continue;
			}

			ptr = pmask + i * image_w + start;

			for (j = start; j <= end; ++j)
			{
                *ptr++ = 255;
			}
		}
	} 

	return;
}
