//|-------------------------------------------------------------------------
//| File:        sort.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef sortHPP
#define sortHPP

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

// Sort speed results (Spec - CPU Centrino 1.6GHz, RAM 512 Mb), ms

// N          heapsort    quicksort   shellsort   binincsort  simpincsort  minimasort  bubblesort
// 10         0           0           0           0           0            0           0
// 100        0           0           0           0           0            0           0
// 1000       0           0           0           10          0            0           0
// 10000      0           0           20          30          80           161         380
// 100000     20          20          1512        3616        8121         14311       38215
// 1000000    371         180         153000      638248      1363300      1584168     4903941
// 10000000   9093        2134        -           -			  -			   -		   -
// 100000000  163165      24165       -           -			  -			   -		   -
 
// The Quick sort is the best.

//----------------------------------------------------------------------------

//! Sorts the array using Heap sort 
class heapsort
{
	//! Sinks an element into the heap.
	template <class T>
	inline void heapsink(T * array, s32 element, s32 max)
	{   
		T temp;

		while ((element<<1) < max)  // there is a left child
		{   
			s32 j = (element<<1);

			if (j+1 < max && array[j] < array[j+1])
				j = j+1;                            // take right child
		        
			if (array[element] < array[j])
			{   
				temp = array[j];                     // swap elements
				array[j] = array[element];
				array[element] = temp;
				element = j;
			}
			else
				return;
		}
	}

public:
	//! Sorts the array 'arr' with size 'size'
	template <class T>
	inline void operator()(T* arr, s32 size)
	{   
		// for heapsink we pretent this is not c++, where
		// arrays start with index 0. So we decrease the array pointer,
		// the maximum always +2 and the element always +1

		T* virtualArray = arr  - 1;
		s32 virtualSize = size + 2;

		s32 i;
		T temp;

		// build heap
		for (i=((size-1)/2); i>=0; --i)
			heapsink(virtualArray, i+1, virtualSize-1);

		// sort array

		for (i=size-1; i>=0; --i)
		{   
			temp = arr[0];
			arr[0] = arr[i];
			arr[i] = temp;
			heapsink(virtualArray, 1, i + 1);
		}
	}  
};

//----------------------------------------------------------------------------

//! Sorts the array using Quick sort 
class quicksort
{   
	template < class T >
	void sort ( T * array, int a, int b )
	{
		int A = a, B = b;

		T mid, temp;

		if ( b > a)
		{
			// Arbitrarily establishing partition element as the midpoint of
			// the array.
			mid = array[ ( a + b ) / 2 ];

			// loop through the array until indices cross
			while( A <= B )
			{
				// find the first element that is greater than or equal to
				// the partition element starting from the left Index.
				while( ( A < b ) && ( array[A] < mid )) ++A;

				// find an element that is smaller than or equal to
				// the partition element starting from the right Index.
				while( ( B > a ) && ( mid < array[B] )) --B;

				// if the indexes have not crossed, swap
				if( A <= B )
				{
					temp = array[A];
					array[A] = array[B];
					array[B] = temp;

					++A;
					--B;
				}
			}

			// If the right index has not reached the left side of array
			// must now sort the left partition.
			if( a < B ) sort( array, a, B );

			// If the left index has not reached the right side of array
			// must now sort the right partition.
			if( A < b ) sort( array, A, b ); 
		}
	}

	template < class T >
	void sort_ptr ( T * array, int a, int b )
	{
		int A = a, B = b;
		T mid, temp;
		if ( b > a)
		{
			mid = array[ ( a + b ) / 2 ];
			while ( A <= B )
			{
				while ( ( A < b ) && ( *array[A] < *mid ) ) ++A;
				while ( ( B > a ) && ( *mid < *array[B] ) ) --B;
				if ( A <= B )
				{
					temp = array[A];
					array[A++] = array[B];
					array[B--] = temp;
				}
			}
			if ( a < B ) sort_ptr ( array, a, B );
			if ( A < b ) sort_ptr ( array, A, b ); 
		}
	}

public:

	template < class T >
	inline void sort ( T * array, s32 num )    
	{ sort ( array, 0, num - 1 ); }

	template < class T >
	inline void sort_ptr ( T * array, s32 num )    
	{ sort_ptr ( array, 0, num - 1 ); }		
};  

//----------------------------------------------------------------------------

//! Sorts the array using Shell sort
class shellsort
{ 
public:
	//! Sorts the array 'array' with size 'num'
	template <class T>
	inline void operator()(T* array, s32 num)    
	{
		long i, j, g;
		short c; // flag
		T temp;

		g = num/2;
		do
		{
			i = g;
			do
			{
				j = i - g;
				c = 1;
				do
				{
					if (array[j] <= array[j+g]) c = 0;
					else 
					{
						temp = array[j];
						array[j] = array[j+g];
						array[j+g] = temp;
					}
					j--;
				} while(j >= 0 && c);
				i++;
			} while(i < num);
			g = g/2;
		} while(g > 0);
	}		
};

//----------------------------------------------------------------------------

//! Sorts the array using Binary Inclusion sort
class binincsort
{ 
public:
	//! Sorts the array 'array' with size 'num'
	template <class T>
	inline void operator()(T* array, s32 num)    
	{
		long i, b, e, c, k;
		static T temp;

		i = 1;
		do
		{
			b = 0;
			e = i - 1;
			c= (b + e)/2;

			while (b != c)
			{
				if (array[c] > array[i]) e=c; else b=c;
				c = (b + e)/2;
			}

			if (array[b] < array[i])
				if (array[i] > array[e]) b = e + 1; else b = e;
			k=i;
			temp = array[i];

			while (k > b)
			{
				array[k] = array[k-1];
				k--;
			}

			array[b] = temp;
			i++;
		} while (i < num);

	}		
}; 

//----------------------------------------------------------------------------

//! Sorts the array using Simple Inclusion sort
class simpincsort
{ 
public:
	//! Sorts the array 'array' with size 'num'
	template <class T>
	inline void operator()(T* array, s32 num)    
	{
		static long i, j, k;
		static T temp;

		i = 1;

		do
		{
			j = 0;
			do
			{
				if (array[i] <= array [j])
				{
					k = i;
					temp = array[i];
					do
					{
						array[k] = array[k-1];
						k--;
					} while (k > j);

					array[j] = temp;
					j = i;
				}
				else j++;
			} while (j < i);
			i++;
		} while (i < num);
	}
};

//----------------------------------------------------------------------------

//! Sorts the array using minima sort
class minimasort
{ 
public:
	//! Sorts the array 'array' with size 'num'
	template <class T>
	inline void operator()(T* array, s32 num)    
	{
		static long k, s, t;
		k = 0;
		static T temp;

		while (k < num)
		{
			s = k;
			t = s;
			while (t < num-1)
			{
				t++;
				if (array[t] < array[s]) s = t;
			}
			temp = array[s];
			array[s] = array[k];
			array[k] = temp;
			k++;
		}
	}
};

//----------------------------------------------------------------------------

//! Sorts the array using bubble sort 
class bubblesort
{ 
public:
	//! Sorts the array 'arr' with size 'size'
	template <class T>
	inline void operator()(T* arr, s32 size)
	{
		static T temp;
		s32 size_1 = size-1;

		for (int i=0; i<size; i++)
		{
			for (int j=size_1; j>i; j--)
			{
				if (arr[j] < arr[j-1] )
				{
					temp = arr[j];
					arr[j] = arr[j-1];
					arr[j-1] = temp;
				}
			}
		}
	}
};

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif

