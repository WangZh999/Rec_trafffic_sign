
#include <memory.h>
#include <math.h>
#include "matrix_proc.h"
#include "base_type.h"
#include <string.h>

#define MIN_DOUBLE  (1e-30)


void ppp(double a[], 
		 double e[], 
		 double s[], 
		 double v[], 
		 int    m, 
		 int    n)
{
	int i, j, p, q;
	double d;

	if(m >= n)
	{
		i = n;
	}
	else
	{
		i = m;
	}

	for(j = 1; j <= i-1; j++)
	{
		a[(j-1) * n + j-1] = s[j-1];
		a[(j-1) * n + j]   = e[j-1];
	}

	a[(i-1) * n + i-1] = s[i-1];

	if(m < n)
	{
		a[(i-1) * n + i] = e[i-1];	
	}

	for(i = 1; i <= n-1; i++)
	{
		for(j = i+1; j <= n; j++)
		{
			p = (i-1) * n + j-1;
			q = (j-1) * n + i-1;

			d    = v[p];
			v[p] = v[q];
			v[q] = d;
		}
	}

	return;
}

void sss(double fg[2], double cs[2])
{
	double r, d;

	//if((fabs(fg[0])+fabs(fg[1]))==0.0)
	if((fabs(fg[0]) + fabs(fg[1])) < MIN_DOUBLE)
	{
		cs[0] = 1.0;
		cs[1] = 0.0;
		d     = 0.0;
	}
	else
	{
		d = sqrt(fg[0] * fg[0] + fg[1] * fg[1]);

		if(fabs(fg[0]) > fabs(fg[1]))
		{
			d = fabs(d);

			if(fg[0] < 0.0)
			{
				d = -d;			
			}
		}

		if(fabs(fg[1]) >= fabs(fg[0]))
		{
			d = fabs(d);

			if(fg[1] < 0.0)
			{
				d = -d;
			}
		}

		cs[0] = fg[0] / d;
		cs[1] = fg[1] / d;
	}

	r = 1.0;

	if(fabs(fg[0]) > fabs(fg[1]))
	{
		r = cs[1];
	}
	else
	{
		//if(cs[0]!=0.0)
		if(fabs(cs[0]) > MIN_DOUBLE)
		{
			r = 1.0 / cs[0];
		}
	}

	fg[0] = d;
	fg[1] = r;

	return;
}


/******************************************************************************
* 功  能：对矩阵进行奇异值分解，参见《c 常用算法程序集》徐世良P169

* 参  数：IN:


* 返回值：
******************************************************************************/
int  MATRIX_PROC_svd_decompose(MATRIX    *matrix_A, 
							   MATRIX    *matrix_U,
							   MATRIX    *matrix_sigma,
							   MATRIX    *matrix_V,
							   double       *s,
							   double       *e,
							   double       *w,
							   double        eps)
{
	int    i, j, k, l, m, n;
	int    it, ll, kk, ix, iy, mm, nn, iz, ml, ks;
	double d, dd, t, sm, sml, eml, sk, ek, b, c, shh, fg[2], cs[2];
	double *a, *u, *v;

    memcpy(matrix_sigma->matrix_data, matrix_A->matrix_data, matrix_A->matrix_cols * matrix_A->matrix_rows * sizeof(double));
    memset(matrix_U->matrix_data, 0, matrix_U->matrix_cols * matrix_U->matrix_rows * sizeof(double));
    memset(matrix_V->matrix_data, 0, matrix_V->matrix_cols * matrix_V->matrix_rows * sizeof(double));

    memset(s, 0, (MAX(matrix_V->matrix_cols, matrix_V->matrix_rows) + 1) * sizeof(double));
    memset(e, 0, (MAX(matrix_V->matrix_cols, matrix_V->matrix_rows) + 1) * sizeof(double));
    memset(w, 0, (MAX(matrix_V->matrix_cols, matrix_V->matrix_rows) + 1) * sizeof(double));

    a = matrix_sigma->matrix_data;
	u = matrix_U->matrix_data;
	v = matrix_V->matrix_data;

	m = matrix_A->matrix_rows;
	n = matrix_A->matrix_cols;

	//矩阵对角线元素置零
	for(i = 1; i <= m; i++)
	{
		ix    = (i - 1) * m + (i - 1);
		u[ix] = 0;
	}

	//矩阵对角线元素置零
	for(i = 1; i <= n; i++)
	{
		iy    = (i - 1) * n + (i - 1);
		v[iy] = 0;
	}

	it = MAX_ITERA;
	k  = n;

	if(m - 1 < n)
	{
		k = m - 1;
	}

	l = m;
	if(n - 2 < m) 
	{
		l = n - 2;
	}

	if(l < 0) 
	{
		l = 0;
	}

	ll = k;
	if(l > k) 
	{
		ll = l;
	}

	if(ll >= 1)
	{
		for(kk = 1; kk <= ll; kk++)
		{
			if(kk <= k)
			{
				d = 0.0;

				for(i = kk; i <= m; i++)
				{
					ix = (i - 1) * n + (kk - 1);
					d  = d + a[ix] * a[ix];
				}

				s[kk-1] = sqrt(d);

				//if(s[kk-1]!=0.0)
				if(fabs(s[kk - 1]) > MIN_DOUBLE)
				{
					ix = (kk - 1) * n + (kk - 1);

					//if(a[ix]!=0.0)
					if(fabs(a[ix]) > MIN_DOUBLE)
					{
						s[kk - 1] = fabs(s[kk - 1]);

						if(a[ix] < 0.0) 
						{
							s[kk-1] = -s[kk-1];
						}
					}

					for(i = kk; i <= m; i++)
					{
						iy    = (i-1) * n + (kk-1);
						a[iy] = a[iy] / s[kk-1];
					}

					a[ix] = 1.0 + a[ix];
				}

				s[kk-1] = -s[kk-1];
			}

			if(n >= kk + 1)
			{
				for(j = kk + 1; j <= n; j++)
				{
					//if((kk<=k)&&(s[kk-1]!=0.0))
					if((kk <= k) && (fabs(s[kk-1]) > MIN_DOUBLE))
					{
						d = 0.0;

						for(i = kk; i <= m; i++)
						{
							ix = (i-1) * n + (kk-1);
							iy = (i-1) * n + (j-1);
							d  = d + a[ix] * a[iy];
						}

						d = -d / a[(kk-1) * n + (kk-1)];

						for(i = kk; i <= m; i++)
						{
							ix    = (i-1) * n + (j-1);
							iy    = (i-1) * n + (kk-1);
							a[ix] = a[ix] + d * a[iy];
						}
					}

					e[j-1] = a[(kk-1) * n + (j-1)];
				}
			}

			if(kk <= k)
			{
				for(i = kk; i <= m; i++)
				{
					ix = (i-1) * m + (kk-1);
					iy = (i-1) * n + (kk-1);

					u[ix] = a[iy];
				}
			}

			if(kk <= l)
			{
				d = 0.0;

				for(i = kk+1;i <= n; i++)
				{
					d = d + e[i-1] * e[i-1];
				}

				e[kk-1] = sqrt(d);

				//if(e[kk-1]!=0.0)
				if(fabs(e[kk-1]) > MIN_DOUBLE)
				{
					//if(e[kk]!=0.0)
					if(fabs(e[kk]) > MIN_DOUBLE)
					{
						e[kk-1] = fabs(e[kk-1]);

						if(e[kk] < 0.0)
						{
							e[kk-1] = -e[kk-1];
						}
					}

					for(i = kk+1; i <= n; i++)
					{
						e[i-1] = e[i-1] / e[kk-1];
					}

					e[kk] = 1.0 + e[kk];
				}

				e[kk-1]=-e[kk-1];

				//if((kk+1<=m)&&(e[kk-1]!=0.0))
				if((kk + 1 <= m) && (fabs(e[kk-1]) > MIN_DOUBLE))
				{
					for(i = kk + 1; i <= m; i++) 
					{
						w[i-1] = 0.0;
					}

					for(j = kk + 1; j <= n; j++)
					{
						for(i = kk + 1; i <= m; i++)
						{
							w[i-1]=w[i-1]+e[j-1]*a[(i-1)*n+j-1];
						}
					}

					for(j = kk + 1; j <= n; j++)
					{
						for(i = kk+1; i <= m; i++)
						{
							ix = (i-1) * n + (j-1);

							a[ix] = a[ix] - w[i-1] * e[j-1] / e[kk];
						}
					}
				}

				for(i = kk + 1; i <= n; i++)
				{
					v[(i-1) * n +(kk-1)] = e[i-1];
				}
			}

		}
	}

	mm = n;

	if(m + 1 < n) 
	{
		mm = m + 1;
	}

	if(k < n)
	{
		s[k] = a[k * n + k];
	}

	if(m < mm) 
	{
		s[mm-1] = 0.0;
	}

	if(l+1 < mm) 
	{
		e[l] = a[l * n + mm - 1];
	}

	e[mm-1] = 0.0;

	nn = m;
	if(m > n) 
	{
		nn = n;
	}

	if(nn >= k+1)
	{
		for(j = k+1; j  <= nn; j++)
		{
			for(i=1;i<=m;i++)
			{
				u[(i-1) * m + (j-1)] = 0.0;
			}

			u[(j-1) * m + (j-1)] = 1.0;
		}
	}

	if(k >= 1)/////////////////////////////////
	{
		for(ll = 1; ll <= k; ll++)
		{
			kk = k - ll + 1;
			iz = (kk - 1) * m + (kk - 1);

			//if(s[kk-1]!=0.0)
			if(fabs(s[kk-1]) > MIN_DOUBLE)
			{
				if(nn>=kk+1)
				{
					for(j = kk+1; j <= nn; j++)
					{
						d = 0.0;
						for(i = kk; i <= m; i++)
						{
							ix = (i-1) * m + (kk-1);
							iy = (i-1) * m + (j-1);
							d  = d + u[ix] * u[iy] / u[iz];
						}

						d = -d;

						for(i = kk; i <= m; i++)
						{
							ix = (i-1) * m + (j-1);
							iy = (i-1) * m + (kk-1);

							u[ix] = u[ix] + d * u[iy];
						}
					}
				}

				for(i = kk; i <= m; i++)
				{
					ix = (i-1) * m + (kk-1);

					u[ix] = -u[ix];
				}

				u[iz] = 1.0 + u[iz];

				if(kk-1 >= 1)
				{
					for(i = 1; i <= kk-1; i++)
					{
						u[(i-1) * m + (kk-1)] = 0.0;
					}
				}
			}
			else
			{
				for(i=1;i<=m;i++)
					u[(i-1)*m+kk-1]=0.0;
				u[(kk-1)*m+kk-1]=1.0;
			}
		}
	}

	for(ll = 1; ll <= n; ll++)
	{
		kk = n - ll + 1;
		iz = kk * n + kk-1;

		//if((kk<=l)&&(e[kk-1]!=0.0))
		if((kk <= l) && (fabs(e[kk-1]) > MIN_DOUBLE))
		{
			for(j = kk+1; j <= n; j++)
			{
				d = 0.0;

				for(i = kk+1; i <= n; i++)
				{
					ix = (i-1) * n + (kk - 1);
					iy = (i-1) * n + (j - 1);

					d = d + v[ix] * v[iy] / v[iz];
				}

				d = -d;

				for(i = kk+1; i <= n; i++)
				{
					ix = (i-1) * n + (j-1);
					iy = (i-1) * n + (kk-1);

					v[ix] = v[ix] + d * v[iy];
				}
			}
		}

		for(i = 1; i <= n; i++)
		{
			v[(i-1) * n + kk-1] = 0.0;
		}

		v[iz-n] = 1.0;
	}

	for(i = 1; i <= m; i++)
	{
		for(j = 1; j <= n; j++)
		{
			a[(i-1) * n + (j-1)] = 0.0;
		}
	}

	ml = mm;

	it = MAX_ITERA;

	while(1 == 1)//////////////////////////////////
	{
		if(mm == 0)
		{
			ppp(a, e, s, v, m, n);

			return l;
		}

		if(it == 0)
		{
			ppp(a, e, s, v, m, n);

			return -1;
		}

		kk = mm - 1;

		//while((kk!=0)&&(fabs(e[kk-1])!=0.0))
		while((kk != 0) && (fabs(e[kk-1]) > MIN_DOUBLE))
		{
			d  = fabs(s[kk-1]) + fabs(s[kk]);
			dd = fabs(e[kk-1]);

			if(dd > eps * d)
			{
				kk = kk - 1;
			}
			else
			{
				e[kk-1] = 0.0;
			}
		}

		if(kk == mm - 1)
		{
			kk = kk + 1;

			if(s[kk-1] < 0.0)
			{
				s[kk-1] = -s[kk-1];

				for(i = 1; i <= n; i++)
				{
					ix = (i - 1) * n + (kk - 1);

					v[ix] = -v[ix];
				}
			}

			while((kk != ml) && (s[kk-1] < s[kk]))
			{
				d       = s[kk-1];
				s[kk-1] = s[kk];
				s[kk]   = d;

				if(kk < n)
				{
					for(i = 1; i <= n; i++)
					{
						ix = (i-1) * n + (kk-1);
						iy = (i-1) * n + kk;

						d     = v[ix];
						v[ix] = v[iy];
						v[iy] = d;
					}
				}

				if(kk < m)
				{
					for(i = 1; i <= m; i++)
					{
						ix = (i-1) * m + (kk-1);
						iy = (i-1) * m + kk;

						d     = u[ix];
						u[ix] = u[iy];
						u[iy] = d;
					}
				}

				kk = kk+ 1;
			}

			it = MAX_ITERA;
			mm = mm - 1;
		}
		else
		{
			ks = mm;

			//while((ks>kk)&&(fabs(s[ks-1])!=0.0))
			while((ks > kk) && (fabs(s[ks-1]) > MIN_DOUBLE))
			{
				d = 0.0;

				if(ks != mm)
				{
					d = d + fabs(e[ks-1]);				
				}

				if(ks != kk+1) 
				{
					d = d + fabs(e[ks-2]);
				}

				dd = fabs(s[ks-1]);

				if(dd > eps * d)
				{
					ks = ks - 1;
				}
				else
				{
					s[ks-1] = 0.0;
				}
			}

			if(ks == kk)
			{
				kk = kk + 1;

				d = fabs(s[mm-1]);
				t = fabs(s[mm-2]);

				if(t > d)
				{
					d = t;				
				}

				t = fabs(e[mm - 2]);
				if(t > d)
				{
					d = t;
				}

				t = fabs(s[kk-1]);
				if(t > d)
				{
					d = t;
				}

				t=fabs(e[kk-1]);
				if(t > d)
				{
					d = t;
				}

				sm  = s[mm-1] / d;
				sml = s[mm-2] / d;
				eml = e[mm-2] / d;

				sk = s[kk-1] / d;
				ek = e[kk-1] / d;

				b = ((sml + sm) * (sml - sm) + eml * eml) / 2.0;
				c = sm * eml;
				c = c * c;

				shh = 0.0;

				//if((b!=0.0)||(c!=0.0))
				if((fabs(b) > MIN_DOUBLE) || (fabs(c) > MIN_DOUBLE))
				{
					shh = sqrt(b * b + c);

					if(b < 0.0)
					{
						shh = -shh;
					}

					shh = c / (b + shh);
				}

				fg[0] = (sk + sm) * (sk - sm) - shh;
				fg[1] = sk * ek;

				for(i = kk; i <= mm - 1; i++)
				{
					sss(fg, cs);

					if(i != kk)
					{
						e[i-2] = fg[0];
					}

					fg[0]  = cs[0] * s[i-1] + cs[1] * e[i-1];
					e[i-1] = cs[0] * e[i-1] - cs[1] * s[i-1];
					fg[1]  = cs[1] * s[i];
					s[i]   = cs[0] * s[i];

					//if((cs[0]!=1.0)||(cs[1]!=0.0))
					if((fabs(cs[0]-1.0) > MIN_DOUBLE) || (fabs(cs[1]) > MIN_DOUBLE))
					{
						for(j = 1; j <= n; j++)
						{
							ix = (j-1) * n + (i-1);
							iy = (j-1) * n + i;

							d     = cs[0] * v[ix] + cs[1] * v[iy];
							v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
							v[ix] = d;
						}
					}

					sss(fg, cs);

					s[i-1] =  fg[0];
					fg[0]  =  cs[0] * e[i-1] + cs[1] * s[i];
					s[i]   = -cs[1] * e[i-1] + cs[0] * s[i];
					fg[1]  =  cs[1] * e[i];
					e[i]   =  cs[0] * e[i];

					if(i < m)
					{
						//if((cs[0]!=1.0)||(cs[1]!=0.0))
						if((fabs(cs[0] - 1.0) > MIN_DOUBLE) || (fabs(cs[1]) > MIN_DOUBLE))
						{
							for(j = 1; j <= m; j++)
							{
								ix = (j-1) * m + (i-1);
								iy = (j-1) * m + i;

								d     =  cs[0] * u[ix] + cs[1] * u[iy];
								u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
								u[ix] =  d;
							}
						}
					}
				}

				e[mm-2] = fg[0];
				it      = it - 1;
			}
			else
			{
				if(ks == mm)
				{
					kk = kk + 1;

					fg[1]   = e[mm-2];
					e[mm-2] = 0.0;

					for(ll = kk; ll <= mm - 1; ll++)
					{
						i     = mm + kk - ll - 1;
						fg[0] = s[i-1];

						sss(fg, cs);

						s[i-1] = fg[0];

						if(i != kk)
						{
							fg[1]  = -cs[1] * e[i-2];
							e[i-2] =  cs[0] * e[i-2];
						}

						//if((cs[0]!=1.0)||(cs[1]!=0.0))
						if((fabs(cs[0]-1.0) > MIN_DOUBLE) || (fabs(cs[1]) > MIN_DOUBLE))
						{
							for(j = 1; j <= n; j++)
							{
								ix = (j-1) * n + (i-1);
								iy = (j-1) * n + (mm-1);

								d     =  cs[0] * v[ix] + cs[1] * v[iy];
								v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
								v[ix] =  d;
							}
						}
					}
				}
				else
				{
					kk = ks + 1;

					fg[1]   = e[kk-2];
					e[kk-2] = 0.0;

					for(i = kk; i <= mm; i++)
					{
						fg[0] = s[i-1];

						sss(fg, cs);

						s[i-1] =  fg[0];
						fg[1]  = -cs[1] * e[i-1];
						e[i-1] =  cs[0] * e[i-1];

						//if((cs[0]!=1.0)||(cs[1]!=0.0))
						if((fabs(cs[0] - 1.0) > MIN_DOUBLE) || (fabs(cs[1]) > MIN_DOUBLE))
						{
							for(j = 1; j <= m; j++)
							{
								ix = (j-1) * m + (i-1);
								iy = (j-1) * m + (kk-2);

								d     =  cs[0] * u[ix] + cs[1] * u[iy];								
								u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
								u[ix] =  d;
							}
						}
					}
				}
			}
		}
	}

	return l;
}

/******************************************************************************
* 功  能：矩阵相乘

* 参  数：IN:
             matrix_C = matrix_A * matrix_B, 主义需要满足矩阵乘法的条件

* 返回值：
******************************************************************************/
void MATRIX_PROC_multiply(MATRIX    *matrix_A,
                          MATRIX    *matrix_B,
						  MATRIX    *matrix_C)
{
	int     i, j, l, u;
	int     m, n, k;
	double *a, *b, *c;

	if (matrix_A->matrix_rows != matrix_C->matrix_rows ||
		matrix_A->matrix_cols != matrix_B->matrix_rows ||
		matrix_C->matrix_cols != matrix_B->matrix_cols)
	{
		return;
	}

	a = matrix_A->matrix_data;
	b = matrix_B->matrix_data;
	c = matrix_C->matrix_data;

    m = matrix_A->matrix_rows;
    n = matrix_A->matrix_cols;
    k = matrix_C->matrix_cols;

	for(i = 0; i <= m-1; i++)
	{
		for(j = 0; j <= k-1; j++)
		{
			u = i * k + j;

			c[u] = 0;

			for(l = 0; l <= n-1; l++)
			{
				c[u] = c[u] + a[i * n + l] * b[l * k + j];			
			}
		}
	}

	return;
}

/******************************************************************************
* 功  能：矩阵求逆运算

* 参  数：IN:
             matrix  -------------------------  输入矩阵
             row     -------------------------  行缓存
             col     -------------------------  列缓存

* 返回值：
******************************************************************************/
void MATRIX_PROC_invert(MATRIX    *matrix, 
				        int          *row, 
				        int          *col)
{
	int   i , j , k , l , u , v, num_columns;
	double d = 0.0 , p = 0.0;
	double *p_matrix_data;

	num_columns   = matrix->matrix_cols;
	p_matrix_data = matrix->matrix_data;

	for (k = 0; k <= num_columns - 1; k++)
	{
		d = 0.0;
		for (i = k; i <= num_columns - 1; i++)
		{
			for (j = k; j <= num_columns - 1; j++)
			{
				l = i * num_columns + j;

				p = (float)fabs(p_matrix_data[l]);

				if (p > d)
				{
					d      = p;
					row[k] = i;
					col[k] = j;
				}
			}
		}

		if (d == 0.0)
		{
			return;
		}

		if (row[k] != k)
		{
			for (j = 0; j <= num_columns - 1; j++)
			{
				u = k * num_columns + j;
				v = row[k] * num_columns + j;
				p = p_matrix_data[u];

				p_matrix_data[u] = p_matrix_data[v];
				p_matrix_data[v] = p;
			}
		}

		if (col[k] != k)
		{
			for (i = 0 ; i <= num_columns - 1; i++)
			{
				u = i * num_columns + k ;
				v = i * num_columns + col[k];
				p = p_matrix_data[u];

				p_matrix_data[u] = p_matrix_data[v];
				p_matrix_data[v] = p;
			}
		}

		l = k * num_columns + k;

		p_matrix_data[l] = 1.0f / p_matrix_data[l];

		for (j = 0; j <= num_columns - 1; j++)
		{
			if (j != k)
			{
				u = k * num_columns + j;
				p_matrix_data[u] = p_matrix_data[u] * p_matrix_data[l];
			}
		}

		for (i = 0; i <= num_columns - 1; i++)
		{
			if (i != k)
			{
				for (j = 0; j <= num_columns - 1; j++)
				{
					if (j != k)
					{
						u = i * num_columns + j;
						p_matrix_data[u] = p_matrix_data[u] - 
							p_matrix_data[i * num_columns + k] * p_matrix_data[k * num_columns + j];
					}
				}
			}
		}

		for (i = 0; i <= num_columns - 1; i++)
		{
			if (i != k)
			{
				u = i * num_columns + k;
				p_matrix_data[u] = - p_matrix_data[u] * p_matrix_data[l];
			}
		}
	}

	for (k = num_columns - 1; k >= 0; k--)
	{
		if (col[k] != k)
		{
			for (j = 0; j <= num_columns - 1; j++)
			{
				u = k * num_columns + j;
				v = col[k] * num_columns + j;
				p = p_matrix_data[u];

				p_matrix_data[u] = p_matrix_data[v];
				p_matrix_data[v] = p;
			}
		}

		if (row[k] != k)
		{
			for (i = 0; i <= num_columns - 1; i++)
			{
				u = i * num_columns + k;
				v = i * num_columns + row[k];
				p = p_matrix_data[u];

				p_matrix_data[u] = p_matrix_data[v];
				p_matrix_data[v] = p;
			}
		}
	}

	return;
}

/******************************************************************************
* 功  能：矩阵转置运算

* 参  数：IN:
			matrix_A   -------------------------  输入矩阵
			matrix_At  -------------------------  转置后的矩阵

* 返回值：
******************************************************************************/
void MATRIX_PROC_transpose(MATRIX *matrix_A, MATRIX *matrix_At)
{
    int i, j;

	if (matrix_A == NULL || matrix_At == NULL)
	{
		return;
	}

	if (matrix_A->matrix_data == NULL || matrix_At->matrix_data == NULL)
	{
		return;
	}

    if (matrix_A->matrix_rows != matrix_At->matrix_cols ||
		matrix_A->matrix_cols != matrix_At->matrix_rows)
    {
		return;
    }

	for (i = 0; i < matrix_A->matrix_rows; ++i)
	{
		for (j = 0; j < matrix_A->matrix_cols; ++j)
		{
            matrix_At->matrix_data[j * matrix_At->matrix_cols + i] 
			= matrix_A->matrix_data[i * matrix_A->matrix_cols + j];
		}
	}
   
    return;
}


#undef MIN_DOUBLE