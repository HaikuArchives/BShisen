#ifndef __ARRAY2D_H__
#define __ARRAY2D_H__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* 
 * templated class to hold a 2-d array of some data
 *
 *
 *
 */

template <class TYPE>
class array2d
{
public:

	array2d(uint32 x, uint32 y);
	~array2d();

/*
 * return x/y max
 */
uint32	X(void) {return _x;};
uint32	Y(void) {return _y;};

void	SetData(TYPE T, uint32 x, uint32 y)
	{
		if (x > _x || y > _y)
		{
			fprintf(stderr, "SetData Out of Bounds: x = %ld y = %ld\n", x, y);
			exit(-1);
		}
		//if ( x < _x && y < _y)
			data[x][y] = T;
	};

TYPE	GetData(uint32 x, uint32 y)
	{
		if (x > _x || y > _y)
		{
			fprintf(stderr, "GetData Out of Bounds: x = %ld y = %ld\n", x, y);
			exit(-1);
		}
//		if (x < _x && y < _y)
			return data[x][y];
		#if 0
		else {
			fprintf(stderr, "\n\nx = %ld, y = %ld\n", x, y);
			fprintf(stderr, "_x = %ld, _y = %ld\n", _x, _y);
			
			assert(!"Array out of bounds");
		}
		#endif
	};
private:
uint32 _x;
uint32 _y;

TYPE **data;
};

/*
 * constructor
 *
 */
template <class TYPE>
array2d<TYPE>::array2d(uint32 x, uint32 y):
	_x(x),
	_y(y),
	data(NULL)
{   
register uint i;

	data = (TYPE **)malloc(sizeof (TYPE *) * x);

	for (i = 0; i < x; i++)
		data[i] = (TYPE *)malloc(sizeof (TYPE) * y);

}

template <class TYPE>
array2d<TYPE>::~array2d()
{
register uint i;

	for (i = 0; i < _x; i++)
		free (data[i]);

	free(data);
}
#endif
