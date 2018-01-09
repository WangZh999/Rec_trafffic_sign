#include <memory.h>
#include <stdlib.h>
#include "contours.h"


#ifndef CV_INIT_3X3_DELTAS
#define  CV_INIT_3X3_DELTAS( deltas, step, nch )            \
	((deltas)[0] =  (nch),  (deltas)[1] = -(step) + (nch),  \
	(deltas)[2] = -(step), (deltas)[3] = -(step) - (nch),  \
	(deltas)[4] = -(nch),  (deltas)[5] =  (step) - (nch),  \
	(deltas)[6] =  (step), (deltas)[7] =  (step) + (nch))
#endif



typedef struct TagContourInfo
{
	int flags;
	struct TagContourInfo *next;        /* next contour with the same mark value */
	struct TagContourInfo *parent;      /* information about parent contour */
	//CvSeq *contour;             /* corresponding contour (may be 0, if rejected) */
	CV_RECT rect;                /* bounding rectangle */
	CV_POINT origin;             /* origin point (where the contour was traced from) */
	CV_SEQ *ptSeq;
	int is_hole;                /* hole flag */
}TContourInfo;


int CodeDeltas[8][2] =
{ {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };



void *memory_malloc(int size, MEMORY_BUFFER *storage)
{
	void *ptData = NULL;

	if (storage == NULL)
	{
		ptData = malloc(size);
	}
	else
	{
		ptData = alloc_memory(storage, size);
//		if (ptData == NULL)
//		{
//			ptData = malloc(size);
//		}
	}
	return ptData;
}

void memory_free(void *ptData, MEMORY_BUFFER *storage)
{
	if (storage == NULL)
	{
		free(ptData);
	}
	else if (!(storage->start_pos <= ptData && storage->end_pos > ptData))
	{
		free(ptData);
	}
}


int WriteSeqElem(CV_SEQ *ptSeq, void *ptElem, int elem_size, MEMORY_BUFFER *mem)
{
	LIST *ptNode = NULL;
	void *ptData = NULL;

	if (ptSeq == NULL || ptElem == NULL)
	{
		return -1;
	}
	ptData = (void *)memory_malloc(elem_size, mem);
	if (ptData == NULL)
	{
		return -1;
	}
	memcpy(ptData, ptElem, elem_size);
	ptNode = (LIST *)memory_malloc(sizeof(LIST), mem);
	if (ptNode == NULL)
	{
		return -1;
	}
	ptNode->ptData = ptData;
	ptNode->ptNext = NULL;
	if (ptSeq->list.ptTail == NULL)
	{
		ptSeq->list.ptHead = ptNode;
	}
	else
	{
		ptSeq->list.ptTail->ptNext = ptNode;
	}
	ptSeq->list.ptTail = ptNode;
	ptSeq->list.total++;
	return 0;
}

CV_RECT BoundingRect(CV_SEQ *ptSeq)
{
	int  xmin = 0, ymin = 0, xmax = -1, ymax = -1;
	LIST *ptCurNode = NULL;
	CV_POINT *pt = NULL;
	CV_RECT rect;

	ptCurNode = ptSeq->list.ptHead;
	pt = (CV_POINT *)ptCurNode->ptData;
	xmin = xmax = pt->x;
	ymin = ymax = pt->y;
	ptCurNode = ptCurNode->ptNext;

	while (ptCurNode != NULL)
	{
		pt = (CV_POINT *)ptCurNode->ptData;
		if (xmin > pt->x)
		{
			xmin = pt->x;
		}
		if (xmax < pt->x)
		{
			xmax = pt->x;
		}
		if (ymin > pt->y)
		{
			ymin = pt->y;
		}
		if (ymax < pt->y)
		{
			ymax = pt->y;
		}
		ptCurNode = ptCurNode->ptNext;
	}

	rect.x = xmin;
	rect.width = xmax-xmin+1;
	rect.y = ymin;
	rect.height = ymax-ymin+1;
	ptSeq->rect = rect;
	return ptSeq->rect;
}



int FetchContour(signed char                   *ptr,
				 int                    step,
				 CV_POINT               *pt,
				 CV_SEQ*                contour,
				 MEMORY_BUFFER          *mem)
{
	signed char     nbd = 2;
	int   deltas[16];
	signed char           *i0 = ptr, *i1, *i3, *i4 = 0;
	int             prev_s = -1, s, s_end;


	CV_INIT_3X3_DELTAS( deltas, step, 1 );
	memcpy( deltas + 8, deltas, 8 * sizeof( deltas[0] ));

	s_end = s = (contour->flags == SEQ_FLAG_HOLE) ? 0 : 4;

	do
	{
		s = (s - 1) & 7;
		i1 = i0 + deltas[s];
		if( *i1 != 0 )
			break;
	}
	while( s != s_end );
	if( s == s_end )            /* single pixel domain */
	{
		*i0 = (signed char) (nbd | -128);
		WriteSeqElem(contour, pt, sizeof(*pt), mem);
	}
	else
	{
		i3 = i0;
		prev_s = s ^ 4;

		/* follow border */
		for( ;; )
		{
			s_end = s;

			for( ;; )
			{
				i4 = i3 + deltas[++s];
				if( *i4 != 0 )
					break;
			}
			s &= 7;

			/* check "right" bound */
			if( (unsigned) (s - 1) < (unsigned) s_end )
			{
				*i3 = (signed char) (nbd | -128);
			}
			else if( (BYTE)(*i3) == 1)
			{
				*i3 = nbd;
			}
			if( s != prev_s)
			{
				WriteSeqElem(contour, pt, sizeof(*pt), mem);
				prev_s = s;
			}
			pt->x += CodeDeltas[s][0];
			pt->y += CodeDeltas[s][1];
			if( i4 == i0 && i3 == i1 )
				break;

			i3 = i4;
			s = (s + 4) & 7;
		}                       /* end of border following loop */
	}
	//BoundingRect(contour);
	return 0;
}


CV_SEQ *CreateSeq(MEMORY_BUFFER *mem)
{
	CV_SEQ *ptSeq = (CV_SEQ *)memory_malloc(sizeof(CV_SEQ), mem);
	if (ptSeq == NULL)
	{
		return NULL;
	}
	memset(ptSeq, 0, sizeof(CV_SEQ));
	return ptSeq;
}


void DestroySeq(CV_SEQ *ptSeq, MEMORY_BUFFER *storage)
{
	CV_SEQ *ptTmp = (CV_SEQ *)ptSeq;
	if (ptSeq == NULL)
	{
		return ;
	}
	while (ptTmp != NULL)
	{
		ptSeq = ptTmp->h_next;
		if (ptTmp->list.ptHead != NULL)
		{
			LIST *ptNode = ptTmp->list.ptHead;
			while (ptNode != NULL)
			{
				ptTmp->list.ptHead = ptNode->ptNext;
				if (ptNode->ptData != NULL)
				{
					memory_free(ptNode->ptData, storage);
				}
				memory_free(ptNode, storage);
				ptNode = ptTmp->list.ptHead;
			}
		}
		memory_free(ptTmp, storage);
		ptTmp = ptSeq;
	}

	if (storage != NULL)
	{
		storage->cur_pos = storage->start_pos;
	}
	return ;
}


CV_SEQ *FindContours(BYTE *pu8Img, int image_w, int image_h, int mode, MEMORY_BUFFER *storage)
{
	int x = 1,y = 1;
	signed char *img1;
	signed char *img = (signed char *)pu8Img;
	int prev = 0;
	CV_POINT origin;
	CV_SEQ *ptSeq = NULL;
	CV_POINT lnbd;
	int new_mask = -2;

	if (img == NULL)
	{
		return NULL;
	}
	lnbd.x = 0;
	lnbd.y = 1;

	/*make border to zero*/
	memset(img, 0, image_w);
	memset(img+(image_h-1)*image_w, 0, image_w);
	img1 = img+image_w;
	for (y = 1; y < image_h-1; y++, img1 += image_w)
	{
		img1[0] = 0;
		img1[image_w - 1] = 0;
	}

	prev = img1[x-1];
	img1 = img + image_w;
	for(y = 1; y < image_h-1; y++,  img1 += image_w)
	{
		int p = 0;
		for (; x < image_w; x++)
		{
			for (; x < image_w; x++)
			{
				if (prev != (p = img1[x]))
				{
					break;
				}
			}
			if (x < image_w)
			{
				int is_hole = 0;
				/*ÅÐ¶ÏÊÇ·ñÎªHole*/
				if( prev != 0 || p != 1 )    /* if not external contour */
				{
					/* check hole */
					if( p != 0 || prev < 1 )
						goto resume_scan;
					if( prev & new_mask )
					{
						lnbd.x = x - 1;
					}
					is_hole = 1;
				}
				if (mode == CONTOURS_EXTERNAL && ( is_hole == 1  || img[lnbd.y * image_w + lnbd.x] > 0))
				{
					goto resume_scan;
				}
				else
				{
					CV_SEQ *ptTmp = CreateSeq(storage);
					if (ptTmp == NULL)
					{
						return ptSeq;
					}
					origin.y = y;
					origin.x = x - is_hole;

					lnbd.x = x - is_hole;
					ptTmp->flags =  is_hole ? SEQ_FLAG_HOLE:0;
					FetchContour(img1+x-is_hole, image_w, &origin, ptTmp, storage);
					if (ptSeq == NULL)
					{
						ptSeq = ptTmp;
					}
					else
					{
						ptTmp->h_next = ptSeq;
						ptSeq = ptTmp;
					}
					prev = img[x];
					continue;
				}
			}
resume_scan:
			prev = p;
			/* update lnbd */
			if( prev & -2 )
			{
				lnbd.x = x;
			}
		}
		prev = 0;
		lnbd.x = 0;
		lnbd.y = y + 1;
		x = 1;
	}
	return ptSeq;
}

