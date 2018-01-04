

#include <memory.h>
#include "median.h"
#include <string.h>


#define ELEM_SWAP(a, b) { register int t = (a); (a) = (b); (b) = t; }


//获取数组的中值，数组中元素的顺序被更改
int get_median_value_changed(int arr[], int n)
{
	int low, high;
	int median;
	int middle, ll, hh;

	low  = 0;
	high = n - 1;
	median = (low + high) / 2;

	for (;;)
	{
		if (high <= low) /* One element only */
		{
		    return arr[median];
		}

		if (high == low + 1)
		{ 
			/* Two elements only */
			if (arr[low] > arr[high])
			{
			    ELEM_SWAP(arr[low], arr[high]);
			}

			return arr[median];
		}

		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;

		if (arr[middle] > arr[high])
		{
		    ELEM_SWAP(arr[middle], arr[high]);
		}

		if (arr[low] > arr[high])
		{
		    ELEM_SWAP(arr[low], arr[high]);
		}

		if (arr[middle] > arr[low])
		{
		    ELEM_SWAP(arr[middle], arr[low]);
		}

		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(arr[middle], arr[low+1]);

		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;

		for (;;)
		{
			do
			{
			  ll++;
			}
			while (arr[low] > arr[ll]);

			do
			{
			  hh--;
			}
			while (arr[hh] > arr[low]);

			if (hh < ll)
			{
			  break;
			}

			ELEM_SWAP(arr[ll], arr[hh]);
	    }

		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(arr[low], arr[hh]);

		/* Re-set active partition */
		if (hh <= median)
		{
		    low = ll;
		}

		if (hh >= median)
		{
			high = hh - 1;
		}
	}
}


//计算数组的中值，通过保留数组的副本维持输入数组不变
int get_median_value_unchange(int arr[], int n, void *buff)
{
	int *temP = (int*)buff;
	memcpy(temP, arr, sizeof(int) * n);

	return get_median_value_changed(temP, n);
}



#undef ELEM_SWAP


