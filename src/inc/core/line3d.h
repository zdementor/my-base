//|-------------------------------------------------------------------------
//| File:        line3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef line3dHPP
#define line3dHPP
//---------------------------------------------------------------------------

#include "vector3d.h"

//---------------------------------------------------------------------------
namespace my {
namespace core {
//---------------------------------------------------------------------------

//! 3D line between two points with intersection methods.
template <class T>
class line3d
{
public:

    //-----------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------

    line3d()
		: start(0,0,0), end(1,1,1) {};
    line3d(T xa, T ya, T za, T xb, T yb, T zb) 
		: start(xa, ya, za), end(xb, yb, zb) {};
    line3d(const vector3d<T>& start, const vector3d<T>& end) 
		: start(start), end(end) {};
    line3d(const line3d<T>& other) 
		: start(other.start), end(other.end) {};

    //-----------------------------------------------------------------------
    // operators
    //-----------------------------------------------------------------------

    inline line3d<T> operator+(const vector3d<T>& point) const 
	{ 
		return line3d<T>(start + point, end + point); 
	}

	//-----------------------------------------------------------------------

    inline line3d<T>& operator+=(const vector3d<T>& point) 
	{ 
		start += point; end += point; return *this; 
	}

	//-----------------------------------------------------------------------

    inline line3d<T> operator-(const vector3d<T>& point) const 
	{ 
		return line3d<T>(start - point, end - point); 
	}

	//-----------------------------------------------------------------------

    inline line3d<T>& operator-=(const vector3d<T>& point) 
	{ 
		start -= point; end -= point; return *this; 
	}

	//-----------------------------------------------------------------------

    inline bool operator==(const line3d<T>& other) const 
	{ 
		return (start==other.start && end==other.end) || (end==other.start && start==other.end);
	}

	//-----------------------------------------------------------------------

    inline bool operator!=(const line3d<T>& other) const 
	{ 
		return !(start==other.start && end==other.end) || (end==other.start && start==other.end);
	}

    //-----------------------------------------------------------------------
    // functions
    //-----------------------------------------------------------------------
    
	inline void set(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb)
	{
		start.set(xa, ya, za); end.set(xb, yb, zb);
	}
    
	//-----------------------------------------------------------------------

    inline void set(const vector3d<T>& nstart, const vector3d<T>& nend)
	{
		start.set(nstart); end.set(nend);
	}

    //-----------------------------------------------------------------------

    inline void set(const line3d<T>& line)
	{
		start.set(line.start); end.set(line.end);
	}

    //-----------------------------------------------------------------------

    //! Returns length of line
    //! \return Returns length of line.
    inline f64 getLength() const 
	{ 
		return start.getDistanceFrom(end); 
	}

    //-----------------------------------------------------------------------

    //! Returns sqared length of line
    //! \return Returns sqared length of line.
    inline f64 getLengthSQ() const 
	{ 
		return start.getDistanceFromSQ(end); 
	}

    //-----------------------------------------------------------------------

    //! Returns middle of line
    inline vector3d<T> getMiddle() const
    {
        return (start + end) * (T)0.5;
    }

    //-----------------------------------------------------------------------

    //! Returns vector of line
    inline vector3d<T> getVector() const
    {
        return end - start;
    }

    //-----------------------------------------------------------------------

    //! Returns if the overgiven point is between start and end of the
    //! line. Assumes that the point is already somewhere on the line.
    inline bool isPointBetweenStartAndEnd(const vector3d<T>& point) const
    {
        return point.isBetweenPoints(start, end);
    }

    //-----------------------------------------------------------------------

    //! Returns the closest point on this line to a point
    inline vector3d<T> getClosestPoint(const vector3d<T>& point) const
    {
        vector3d<T> c = point - start;
        vector3d<T> v = end - start;
        T d = (T)v.getLength();
        v /= d;
        T t = v.getDotProduct(c);

        if (t < (T)0.0) return start;
        if (t > d) return end;

        v *= t;
        return start + v;
    }

    //-----------------------------------------------------------------------

    //! Returns if the line intersects with a shpere
    //! \param sorigin: Origin of the shpere.
    //! \param sradius: Radius if the sphere.
    //! \return Returns true if there is an intersection.
    //! If there is one, the distance to the first intersection point
    //! is stored in outdistance.
    inline bool getIntersectionWithSphere(vector3d<T> sorigin, T sradius, f64& outdistance)
    {
        vector3d<T> q = sorigin - start;
        f64 c = q.getLength();
        f64 v = q.getDotProduct(getVector().normalize());
        f64 d = sradius * sradius - (c*c - v*v);

        if (d < 0.0) 
            return false;

        outdistance = v - sqrt(d);
        return true;
    }

	//-----------------------------------------------------------------------

    //! Returns true if the line intersects with a shpere
    inline bool intersectsWithSphere(vector3d<T> sorigin, T sradius)
    {
        vector3d<T> q = sorigin - start;
        f64 c = q.getLength();
        f64 v = q.getDotProduct(getVector().normalize());
        f64 d = sradius * sradius - (c*c - v*v);

        if (d < 0.0) 
            return false;

        return true;
    }

    //-----------------------------------------------------------------------
    // member variables
    //-----------------------------------------------------------------------    

    vector3d<T> start;
    vector3d<T> end;
};

//-----------------------------------------------------------------------
//! Typedef for a f32 3d line.
typedef line3d<f32> line3df;

//! Typedef for an integer 3d line.
typedef line3d<s32> line3di;

//-----------------------------------------------------------------------
} // end namespace core
} // end namespace my
//-----------------------------------------------------------------------

#endif // #ifndef line3dHPP

