//|-------------------------------------------------------------------------
//| File:        mathfunc.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <my_inc.h>
#include "optimization.hpp"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

// Size of the trig tables as determined by constructor.
static const int TRIG_TABLE_SIZE = 4096; 

// Radian -> index factor value ( mTrigTableSize / 2 * PI )
static const f32 TRIG_TABLE_FACTOR = (f32)TRIG_TABLE_SIZE / (f32)TWO_PI;

// tables
static f32 SinTable[TRIG_TABLE_SIZE];
static f32 TanTable[TRIG_TABLE_SIZE];  

//----------------------------------------------------------------------------

class CMathInitializator
{
public:
	CMathInitializator()
	{
		// Build trig lookup tables
		// Could get away with building only PI sized Sin table but simpler this 
		// way. Who cares, it'll ony use an extra 8k of memory anyway and I like 
		// simplicity.
		f32 angle;
		for (int i = 0; i < TRIG_TABLE_SIZE; ++i)
		{
			angle = (f32)TWO_PI * (f32)i / (f32)TRIG_TABLE_SIZE;
			SinTable[i] = sin(angle);
			TanTable[i] = cos(angle);
		}
	}
}
MathInitializator;

//-----------------------------------------------------------------------	

MY_FORCEINLINE f32 sinTable (f32 fValue)
{
	// Convert range to index values, wrap if required
    int idx;
    if (fValue >= 0)
    {
		idx = int(fValue * TRIG_TABLE_FACTOR) % TRIG_TABLE_SIZE;
    }
    else
    {
		idx = TRIG_TABLE_SIZE - (int(-fValue * TRIG_TABLE_FACTOR) % TRIG_TABLE_SIZE) - 1;
    }

	return SinTable[idx];
}
//-----------------------------------------------------------------------

MY_FORCEINLINE f32 tanTable (f32 fValue)
{
	// Convert range to index values, wrap if required
	int idx = int(fValue *= TRIG_TABLE_FACTOR) % TRIG_TABLE_SIZE;
	return TanTable[idx];
}

//---------------------------------------------------------------------

MY_FORCEINLINE s32 math::Random() // generates a pseudo random integer number in [-7FFFFFFF, 7FFFFFFF]
{   
	return randomizer::random();
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::UnitRandom()  // in [0,1]
{
	return (f32)math::Random()/(f32)0x7FFFFFFF;;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::RangeRandom(const f32& fLow, const f32& fHigh)  // in [fLow,fHigh]
{
	return (fHigh-fLow)*UnitRandom() + fLow;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::SymmetricRandom()  // in [-1,1]
{
	return 2.0f * UnitRandom() - 1.0f;
}

//---------------------------------------------------------------------

MY_FORCEINLINE s32 math::Max(const f32& v1, const f32& v2)
{   
	return (v1>v2)?v1:v2;
}

//---------------------------------------------------------------------

MY_FORCEINLINE s32 math::Max(const f32& v1, const f32& v2, const f32& v3)
{   
	f32 v1_v2 = Max(v1,v2);

    return Max(v1_v2,v3);;
}

//---------------------------------------------------------------------

MY_FORCEINLINE s32 math::Min(const f32& v1, const f32& v2)
{   
	return (v1<v2)?v1:v2;
}

//---------------------------------------------------------------------

MY_FORCEINLINE s32 math::Min(const f32& v1, const f32& v2, const f32& v3)
{   
	f32 v1_v2 = Min(v1,v2);

    return Min(v1_v2,v3);;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 math::IAbs(const s32& iValue)
{
	return ( iValue >= 0 ? iValue : -iValue ); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 math::ICeil(const f32& fValue)
{
	 return s32(ceil(fValue)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 math::IFloor(const f32& fValue)
{
	return s32(floor(fValue)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 math::ISign(const s32& iValue)
{
	return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Abs(const f32& fValue)
{
	return f32(fabs(fValue)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::ACos(const f32& fValue)
{
	if ( -1.0 < fValue )
    {
		if ( fValue < 1.0 )
			return f32(acos(fValue));
        else
            return f32(0.0);
    }
    else
    {
		return f32(PI);
    }
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::ASin(const f32& fValue)
{
	if ( -1.0 < fValue )
    {
		if ( fValue < 1.0 )
			return f32(asin(fValue));
        else
            return f32(-HALF_PI);
    }
    else
    {
		return f32(HALF_PI);
    }
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::ATan(const f32& fValue)
{
	 return f32(atan(fValue)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::ATan2(const f32& fY, const f32& fX)
{
	return f32(atan2(fY,fX)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Ceil(const f32& fValue)
{
	return f32(ceil(fValue));   
}

//----------------------------------------------------------------------------

// Cosine function.  
MY_FORCEINLINE f32 math::Cos(const f32& fValue, const bool& useTables)
{
	//return (!useTables) ? (f32)cos(fValue) : sinTable(fValue + HALF_PI);
	return cos(fValue);
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Exp(const f32& fValue)
{
	return f32(exp(fValue)); 
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Floor(const f32& fValue)
{
	return f32(floor(fValue));
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Log(const f32& fValue)
{
	return f32(log(fValue));
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Pow(const f32& fBase, const f32& fExponent)
{
	return f32(pow(fBase,fExponent));
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Sign(const f32& fValue)
{
	if ( fValue > 0.0 )
		return 1.0;

    if ( fValue < 0.0 )
		return -1.0;

	return 0.0;
}

//----------------------------------------------------------------------------

//  Sine function.
MY_FORCEINLINE f32 math::Sin(const f32& fValue, const bool& useTables)
{
	//return (!useTables) ? (f32)sin(fValue) :  sinTable(fValue);	
	return sin(fValue);	
}

//----------------------------------------------------------------------------

MY_FORCEINLINE f32 math::Sqr(const f32& fValue)
{
	return fValue*fValue;
}

//----------------------------------------------------------------------------   
   
//  Inverse square root i.e. 1 / Sqrt(x), good for vector
//  normalisation.          
MY_FORCEINLINE f32 math::InvSqrt(const f32& fValue)
{
//	return f32(asm_rsq(fValue));
	return 1.0f / Sqrt(fValue);
}

//----------------------------------------------------------------------------

//  Tangent function.       
MY_FORCEINLINE f32 math::Tan (const f32& fValue, const bool& useTables)
{
	//return (!useTables) ? (f32)tan(fValue) : tanTable(fValue);
	return tan(fValue);
}

//----------------------------------------------------------------------------

//! draw line from (x1,y1) to (x2,y2) into pixels array
void math::draw_line(
	s32 x1, s32 y1, s32 x2, s32 y2, core::array<core::vector2di> &pixels) 
{
	s32 Dx = (x2 - x1);
	s32 Dy = (y2 - y1);
	f32 k;

	pixels.set_used(0);

	s32 x, y;
	if (core::math::IAbs(Dx) > core::math::IAbs(Dy))
	{
		k = (f32)Dy / (f32)Dx;
		if (x1 > x2)
			for (x = x1; x >= x2; x--) 
			{
				s32 y = y1 + k * (x - x1) + 0.5f;
				pixels.push_back(core::vector2di(x,y));
			}
		else
			for (x = x1; x <= x2; x++) 
			{
				s32 y = y1 + k * (x - x1) + 0.5f;
				pixels.push_back(core::vector2di(x,y));
			}
	}
	else
	{
		k = (f32)Dx / (f32)Dy;
		if (y1 > y2)
			for (y = y1; y >= y2; y--)
			{
				s32 x = x1 + k * (y - y1) + 0.5f;
				pixels.push_back(core::vector2di(x,y));
			}
		else
			for (y = y1; y <= y2; y++)
			{
				s32 x = x1 + k * (y - y1) + 0.5f;
				pixels.push_back(core::vector2di(x,y));
			}
	}
}

//-----------------------------------------------------------------------

math::event_counter::event_counter() 
	: fps(0), startTime(0), framesCounted(100),
valuePS(0), startValueTime(0), valueCounted(100)
{
}

//-----------------------------------------------------------------------

//! returns current events per second count
s32 math::event_counter::getEventPerSecond()
{
    return fps;
}

//-----------------------------------------------------------------------

s32 math::event_counter::getValuePerSecond()
{
    return valuePS;
}

//-----------------------------------------------------------------------

//! to be called every frame
void math::event_counter::registerEvent(u32 now)
{
    framesCounted++;

    u32 milliseconds = now - startTime;

    if (milliseconds > 2000)
    {
        fps = (s32)((f32)framesCounted / ((f32)milliseconds / 1000.0f));

        startTime = now;
        framesCounted = 0;
    }
}

//-----------------------------------------------------------------------

void math::event_counter::registerValue(u32 now, u32 value)
{
    valueCounted += value;

    u32 milliseconds = now - startValueTime;

    if (milliseconds > 2000)
    {
        valuePS = (s32)((f32)valueCounted / ((f32)milliseconds / 1000.0f));

        startValueTime = now;
        valueCounted = 0;
    }
}

//-----------------------------------------------------------------------

math::spline_interpolator3d::spline_interpolator3d(
	core::vector3df *points, s32 size, f32 tightness
	) :
NumPoints(size), Tightness(tightness), Points(NULL)
{
	Points = new core::vector3df[NumPoints];

	for (u32 i=0; i<NumPoints; i++)
	{
		Points[i] = points[i];
	}
}

//-----------------------------------------------------------------------

math::spline_interpolator3d::~spline_interpolator3d()
{
	SAFE_DELETE_ARRAY(Points);
	
	NumPoints = 0;
}

//-----------------------------------------------------------------------
		
// returns interpolated value
core::vector3df math::spline_interpolator3d::getValue(const f32 &x)
{
	core::vector3df p, p0, p1, p2, p3;
    core::vector3df t1, t2;

    s32 idx = static_cast< s32 >( x ) % NumPoints;
    f32 u = 0.001f * fmodf( x*10000, 1000.0f );
    
    p0 = Points[ clamp( idx - 1, NumPoints ) ];
    p1 = Points[ clamp( idx,     NumPoints ) ];
    p2 = Points[ clamp( idx + 1, NumPoints ) ];
    p3 = Points[ clamp( idx + 2, NumPoints ) ];

    // hermite polynomials
    f32 h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
    f32 h2 = -2.0f * u * u * u + 3.0f * u * u;
    f32 h3 = u * u * u - 2.0f * u * u + u;
    f32 h4 = u * u * u - u * u;

    // tangents
    t1 = ( p2 - p0 ) * Tightness;
    t2 = ( p3 - p1 ) * Tightness;

    // interpolated point
    p = p1 * h1 + p2 * h2 + t1 * h3 + t2 * h4;
        
	return p;;
}

//-----------------------------------------------------------------------
		
//! clamps a the value idx to fit into range 0..size-1
s32 math::spline_interpolator3d::clamp(s32 idx, s32 size)
{
	return ( idx<0 ? size+idx : ( idx>=size ? idx-size : idx ) );
}

//----------------------------------------------------------------------------

//! point linear interpolator
math::pointlinear_interpolator3d::pointlinear_interpolator3d(
	core::vector3df *points, s32 size
	) :
NumPoints(size), Points(NULL)
{
	Points = new core::vector3df[NumPoints];

	for (u32 i=0; i<NumPoints; i++)
	{
		Points[i] = points[i];
	}
}

//----------------------------------------------------------------------------

math::pointlinear_interpolator3d::~pointlinear_interpolator3d()
{
	SAFE_DELETE_ARRAY(Points);
	
	NumPoints = 0;
}

//----------------------------------------------------------------------------
		
// returns interpolated value
core::vector3df math::pointlinear_interpolator3d::getValue(const f32 &x)
{
	core::vector3df p(0, 0, 0);

	u32 idx_left  = static_cast< u32 >( x );
	u32 idx_right = idx_left + 1;

	CHECK_RANGE(idx_left,  0, NumPoints-1);
	CHECK_RANGE(idx_right, 0, NumPoints);

	if ( !(idx_left<idx_right) )
		return p;

	core::vector3df &p1 = Points[idx_left];
	core::vector3df &p2 = Points[idx_right];

	p = p1 + (p2 - p1) * (x - idx_left);

	return p;
}

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------
