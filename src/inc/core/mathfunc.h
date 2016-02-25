//|-------------------------------------------------------------------------
//| File:        mathfunc.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef mathHPP
#define mathHPP
//----------------------------------------------------------------------------

#include "vector3d.h"
#include "vector2d.h"
#include "array.h"
#include "list.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
namespace math {
//----------------------------------------------------------------------------

template <class T>
MY_FORCEINLINE void Swap(T &v0, T &v1)
{
	T tmp = v0;
	v0 = v1;
	v1 = tmp;
}

//! returns linear interpolation of a and b with ratio t
//! \return: a if t==0, b if t==1, and the linear interpolation else
template <class T>
MY_FORCEINLINE T Lerp(const T a, const T b, const T t)
{
	return (a*(1-t)) + (b*t); 	
}

//! returns if a float equals the other one, taking floating 
//! point rounding errors into account
MY_FORCEINLINE bool Equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_32)
{ return (a + tolerance > b) && (a - tolerance < b); }

//! returns if a float equals zero, taking floating 
//! point rounding errors into account
MY_FORCEINLINE bool IsZero(const f32 a, const f32 tolerance = ROUNDING_ERROR_32)
{ return fabs( a ) < tolerance; }

//! returns nearest power of 2
MY_FORCEINLINE s32 GetNearestPowerOfTwo(s32 number)
{
	s32 ts = 0x01;
	while(ts < number)
		ts <<= 1;
	return ts;
}

__MY_CORE_LIB_API__ MY_FORCEINLINE s32 Random(); // generates a pseudo random integer number
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 UnitRandom ();  // in [0,1]
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 RangeRandom (const f32& fLow, const f32& fHigh);  // in [fLow,fHigh]
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 SymmetricRandom ();  // in [-1,1]

__MY_CORE_LIB_API__ MY_FORCEINLINE s32 Max    (const f32& v1, const f32& v2);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 Max    (const f32& v1, const f32& v2, const f32& v3);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 Min    (const f32& v1, const f32& v2);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 Min    (const f32& v1, const f32& v2, const f32& v3);

__MY_CORE_LIB_API__ MY_FORCEINLINE s32 IAbs   (const s32& iValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 ICeil  (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 IFloor (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE s32 ISign  (const s32& iValue);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Abs   (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 ACos  (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 ASin  (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 ATan  (const f32& fValue);
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 ATan2 (const f32& fY, const f32& fX);
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Ceil  (const f32& fValue);

//! draw line from (x1,y1) to (x2,y2) into pixels array
__MY_CORE_LIB_API__ MY_FORCEINLINE void draw_line(
	s32 x1, s32 y1, s32 x2, s32 y2, core::list<core::vector2di> &pixels
	);

// Cosine function.
//  @param
//      fValue Angle in radians
//  @param
//      useTables If true, uses lookup tables rather than
//      calculation - faster but less accurate.        
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Cos (const f32& fValue, const bool& useTables = false);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Exp (const f32& fValue);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Floor (const f32& fValue);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Log (const f32& fValue);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Pow (const f32& fBase, const f32& fExponent);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Sign (const f32& fValue);

//  Sine function.
//  @param
//      fValue Angle in radians
//  @param
//      useTables If true, uses lookup tables rather than
//      calculation - faster but less accurate.
//
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Sin (const f32& fValue, const bool& useTables = false);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Sqr (const f32& fValue);

__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Sqrt (const f32& fValue);
  
//  Inverse square root i.e. 1 / Sqrt(x), good for vector
//  normalisation.          
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 InvSqrt(const f32& fValue);

//  Tangent function.
//  @param
//      fValue Angle in radians
//  @param
//      useTables If true, uses lookup tables rather than
//      calculation - faster but less accurate.          
__MY_CORE_LIB_API__ MY_FORCEINLINE f32 Tan (const f32& fValue, const bool& useTables = false);

//-----------------------------------------------------------

//! linear interpolator 2d
template < class T >
class linear_interpolator_2d
{
	f32 k, c;
	T left_range, right_range;

public:

	linear_interpolator_2d() : k(0), c(0)	{}

	linear_interpolator_2d( const T& x1, const T& y1,const T& x2,const T& y2 )
	{
		init(x1, y1, x2, y2);
	}

	inline void init(const T& x1, const T& y1,const T& x2,const T& y2)
	{
		f32 dX = f32(x2) - f32(x1);
		f32 dY = f32(y2) - f32(y1);

		CHECK_NO_ZERO(dX);

		f32 _k = dY / dX;
		f32 _c = ( f32(y1) * f32(x2) - f32(y2) * f32(x1) ) / dX;

		k = _k; 
		c = _c;

		left_range = x1; 
		right_range = x2;
	}

	inline T getValue(const T &x)
	{
		return k * x + c;
	}

	inline bool isInRange(const T &x)
	{
		if (x>=left_range && x<=right_range)
			return true;
		
		return false;
	}
};

//! linear interpolator 2d for floats
typedef linear_interpolator_2d<f32> li2df;

//! linear interpolator 2d for integers
typedef linear_interpolator_2d<s32> li2di;

//-----------------------------------------------------------

//! point linear interpolator 2d
template < class T >
class point_linear_interpolator_2d
{
	core::array< linear_interpolator_2d<T> > linear_interpolators;

public:

	point_linear_interpolator_2d() {}

	point_linear_interpolator_2d(core::vector2d<T> *points, s32 size) 
	{
		init(points, size);			
	}

	inline void init(core::vector2d<T> *points, s32 size)
	{
		linear_interpolators.clear();

		s32 size_1 = size-1;

		for (s32 i=0; i<size_1; i++)
		{
			linear_interpolators.push_back(
				linear_interpolator_2d<T>(points[i].X, points[i].Y, points[i+1].X, points[i+1].Y )
				);
		}
	}

	inline T getValue(const T &x)
	{
		s32 size_1 = linear_interpolators.size()-1;
		for (s32 i=0; i<=size_1; i++)
		{
			if (i==size_1 || linear_interpolators[i].isInRange(x))
			{
				return linear_interpolators[i].getValue(x);
			}
		}
		return (T)0;
	}
};

//! point linear interpolator 2d for floats
typedef point_linear_interpolator_2d<f32> pli2df;

//! point linear interpolator 2d for integers
typedef point_linear_interpolator_2d<s32> pli2di;

//-----------------------------------------------------------

//! event counter
class __MY_CORE_LIB_API__ event_counter
{
public:

	event_counter();

	//! returns current events per second count
	s32 getEventPerSecond();
	s32 getValuePerSecond();

	//! to be called every frame
	void registerEvent(u32 now);
	void registerValue(u32 now, u32 value);

private:

	s32 fps;
	u32 startTime;
	u32 framesCounted;

	s32 valuePS;
	u32 startValueTime;
	u32 valueCounted;
};

//          *   *   *           //

//! spline interpolator
class __MY_CORE_LIB_API__ spline_interpolator3d
{
public:

	spline_interpolator3d(
		core::vector3df *points, s32 size, f32 tightness
		);	
	~spline_interpolator3d();
		
	// returns interpolated value
	core::vector3df getValue(const f32 &x);

private:

	//! clamps a the value idx to fit into range 0..size-1
	s32 clamp(s32 idx, s32 size);

	core::vector3df *Points;

	f32 Tightness;
	u32 NumPoints;
};

//-----------------------------------------------------------

//! point linear interpolator
class __MY_CORE_LIB_API__ pointlinear_interpolator3d
{
public:

	pointlinear_interpolator3d(
		core::vector3df *points, s32 size
		);	
	~pointlinear_interpolator3d();
		
	// returns interpolated value
	core::vector3df getValue(const f32 &x);

private:
	
	core::vector3df *Points;
	u32 NumPoints;
};

//----------------------------------------------------------------------------
} // end namespace math
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------
#endif