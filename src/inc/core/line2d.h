//|-------------------------------------------------------------------------
//| File:        line2d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef line2dHPP
#define line2dHPP
//----------------------------------------------------------------------------

#include "vector2d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! 2D line between two points with intersection methods.
template <class T>
class line2d
{
public:

    //-----------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------

    line2d()
		: start(0,0,0), end(1,1,1) {};
    line2d(T xa, T ya, T xb, T yb) 
		: start(xa, ya), end(xb, yb) {};
    line2d(const vector2d<T> &_start, const vector2d<T> &_end) 
		: start(_start), end(_end) {}
    line2d(const line2d<T> &other) 
		:start(other.start), end(other.end) {};

    //-----------------------------------------------------------------------
    // operators
    //-----------------------------------------------------------------------
    
	inline line2d<T> operator+(const vector2d<T>& point) const 
	{ 
		return line2d<T>(start + point, end + point); 
	}

	//-----------------------------------------------------------------------

    inline line2d<T>& operator+=(const vector2d<T>& point) 
	{ 
		start += point; end += point; return *this; 
	}

	//-----------------------------------------------------------------------

    inline line2d<T> operator-(const vector2d<T>& point) const 
	{ 
		return line2d<T>(start - point, end - point); 
	}

	//-----------------------------------------------------------------------

    inline line2d<T>& operator-=(const vector2d<T>& point) 
	{ 
		start -= point; end -= point; return *this; 
	}

	//-----------------------------------------------------------------------

    inline bool operator==(const line2d<T>& other) const 
	{ 
		return (start==other.start && end==other.end) || (end==other.start && start==other.end);
	}

	//-----------------------------------------------------------------------

    inline bool operator!=(const line2d<T>& other) const 
	{ 
		return !(start==other.start && end==other.end) || (end==other.start && start==other.end);
	}

    //-----------------------------------------------------------------------
    // functions
    //-----------------------------------------------------------------------

    inline void setLine(const T& xa, const T& ya, const T& xb, const T& yb)
	{
		start.set(xa, ya); end.set(xb, yb);
	}
    
	//-----------------------------------------------------------------------
    
	inline void setLine(const vector2d<T>& nstart, const vector2d<T>& nend)
	{
		start.set(nstart); end.set(nend);
	}

    //-----------------------------------------------------------------------

    inline void setLine(const line2d<T>& line)
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

    //! Returns the vector of the line.
    //! \return Returns the vector of the line.
    inline vector2d<T> getVector() const 
	{ 
		return vector2d<T>(start.X - end.X, start.Y - end.Y); 
	}

    //-----------------------------------------------------------------------

    //! Tests if this line intersects with an other line.
    //! \param l: Other line to test intersection with.
    //! \param out: If there is an intersection, the location of the intersection will
    //! be stored in this vector.
    //! \return Returns true if there is an intersection, false if not.
    inline bool intersectWith(const line2d<T>& l, vector2d<T>& out)
    {
        bool found = getInterSectionOfLines(
			start.X, start.Y, end.X-start.X, end.Y-start.Y,
            l.start.X, l.start.Y, l.end.X-l.start.X, l.end.Y-l.start.Y,
            out.X, out.Y
			);

        return (
			found
            &&  isPointBetweenPoints(out.X, out.Y, start.X, start.Y, end.X, end.Y)
            &&  isPointBetweenPoints(out.X, out.Y, l.start.X, l.start.Y, l.end.X, l.end.Y)
			);
    }

    //-----------------------------------------------------------------------

    //! Returns unit vector of the line.
    //! \return Returns unit vector of this line.
    inline vector2d<T> getUnitVector()
    {
        T len = (T)1.0 / (T)getLength();

        return vector2d<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
    }

    //-----------------------------------------------------------------------

    inline f64 getAngleWith(const line2d<T>& l)
    {
        vector2d<T> vect = getVector();
        vector2d<T> vect2 = l.getVector();

        return vectorAngle(vect.X, vect.Y, vect2.X, vect2.Y);
    }
	//-----------------------------------------------------------------------

    //! Returns middle of line
    inline vector2d<T> getMiddle() const
    {
        return (start + end) * (T)0.5;
    }


    //-----------------------------------------------------------------------    
    // member variables
    //-----------------------------------------------------------------------    

    vector2d<T> start;
    vector2d<T> end;
};

//-----------------------------------------------------------------------

//! Typedef for a f32 2d line.
typedef line2d<f32> line2df;

//! Typedef for an integer 2d line.
typedef line2d<s32> line2di;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef line2dHPP

