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

	//! Tests if this line intersects with another line.
	//! \param l: Other line to test intersection with.
	//! \param checkOnlySegments: Default is to check intersection between the begin and endpoints.
	//! When set to false the function will check for the first intersection point when extending the lines.
	//! \param out: If there is an intersection, the location of the
	//! intersection will be stored in this vector.
	//! return True if there is an intersection, false if not.
	bool intersectWith(const line2d<T>& l, vector2d<T>& out, bool checkOnlySegments=true) const
	{
		// Uses the method given at:
		// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
		const f32 commonDenominator = (f32)(l.end.Y - l.start.Y)*(end.X - start.X) -
			(l.end.X - l.start.X)*(end.Y - start.Y);

		const f32 numeratorA = (f32)(l.end.X - l.start.X)*(start.Y - l.start.Y) -
			(l.end.Y - l.start.Y)*(start.X -l.start.X);

		const f32 numeratorB = (f32)(end.X - start.X)*(start.Y - l.start.Y) -
			(end.Y - start.Y)*(start.X -l.start.X);

		if (math::Equals(commonDenominator, 0.f))
		{
			// The lines are either coincident or parallel
			// if both numerators are 0, the lines are coincident
			if(math::Equals(numeratorA, 0.f) && math::Equals(numeratorB, 0.f))
			{
				// Try and find a common endpoint
				if(l.start == start || l.end == start)
					out = start;
				else if(l.end == end || l.start == end)
					out = end;
				// now check if the two segments are disjunct
				else if (l.start.X>start.X && l.end.X>start.X && l.start.X>end.X && l.end.X>end.X)
					return false;
				else if (l.start.Y>start.Y && l.end.Y>start.Y && l.start.Y>end.Y && l.end.Y>end.Y)
					return false;
				else if (l.start.X<start.X && l.end.X<start.X && l.start.X<end.X && l.end.X<end.X)
					return false;
				else if (l.start.Y<start.Y && l.end.Y<start.Y && l.start.Y<end.Y && l.end.Y<end.Y)
					return false;
				// else the lines are overlapping to some extent
				else
				{
					// find the points which are not contributing to the
					// common part
					vector2d<T> maxp;
					vector2d<T> minp;
					if ((start.X>l.start.X && start.X>l.end.X && start.X>end.X) || (start.Y>l.start.Y && start.Y>l.end.Y && start.Y>end.Y))
						maxp=start;
					else if ((end.X>l.start.X && end.X>l.end.X && end.X>start.X) || (end.Y>l.start.Y && end.Y>l.end.Y && end.Y>start.Y))
						maxp=end;
					else if ((l.start.X>start.X && l.start.X>l.end.X && l.start.X>end.X) || (l.start.Y>start.Y && l.start.Y>l.end.Y && l.start.Y>end.Y))
						maxp=l.start;
					else
						maxp=l.end;
					if (maxp != start && ((start.X<l.start.X && start.X<l.end.X && start.X<end.X) || (start.Y<l.start.Y && start.Y<l.end.Y && start.Y<end.Y)))
						minp=start;
					else if (maxp != end && ((end.X<l.start.X && end.X<l.end.X && end.X<start.X) || (end.Y<l.start.Y && end.Y<l.end.Y && end.Y<start.Y)))
						minp=end;
					else if (maxp != l.start && ((l.start.X<start.X && l.start.X<l.end.X && l.start.X<end.X) || (l.start.Y<start.Y && l.start.Y<l.end.Y && l.start.Y<end.Y)))
						minp=l.start;
					else
						minp=l.end;

					// one line is contained in the other. Pick the center
					// of the remaining points, which overlap for sure
					out = core::vector2d<T>();
					if (start != maxp && start != minp)
						out += start;
					if (end != maxp && end != minp)
						out += end;
					if (l.start != maxp && l.start != minp)
						out += l.start;
					if (l.end != maxp && l.end != minp)
						out += l.end;
					out.X = (T)(out.X/2);
					out.Y = (T)(out.Y/2);
				}

				return true; // coincident
			}

			return false; // parallel
		}

		// Get the point of intersection on this line, checking that
		// it is within the line segment.
		const f32 uA = numeratorA / commonDenominator;
		if(checkOnlySegments && (uA < 0.f || uA > 1.f) )
			return false; // Outside the line segment

		const f32 uB = numeratorB / commonDenominator;
		if(checkOnlySegments && (uB < 0.f || uB > 1.f))
			return false; // Outside the line segment

		// Calculate the intersection point.
		out.X = (T)(start.X + uA * (end.X - start.X));
		out.Y = (T)(start.Y + uA * (end.Y - start.Y));
		return true;
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

