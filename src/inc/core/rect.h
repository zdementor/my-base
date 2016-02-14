//|-------------------------------------------------------------------------
//| File:        rect.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef rectHPP
#define rectHPP
//----------------------------------------------------------------------------

#include "dimension2d.h"
#include "position2d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! Rectangle template.
//! Mostly used by 2D GUI elements and for 2D drawing methods.
//! It has 2 positions instead of position and dimension
//! and a fast method for collision detection with other rectangles and points.
template <class T>
class rect
{
public:

    //------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------

    rect()
        : UpperLeftCorner(0,0), LowerRightCorner(0,0) {}

    rect(T x, T y, T x2, T y2)
        : UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}

	template <class TT>
    rect(const position2d <TT>& upperLeft, const position2d<TT>& lowerRight)
        : UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	template <class TT>
    rect(const rect <TT> & other)
        : UpperLeftCorner(other.UpperLeftCorner), LowerRightCorner(other.LowerRightCorner) {};

    rect(const position2d<T>& pos, const dimension2d<T>& size)
        : UpperLeftCorner(pos), LowerRightCorner(pos.X + size.Width, pos.Y + size.Height) {};

    //------------------------------------------------------------------------
    // operators
    //------------------------------------------------------------------------

    inline rect<T> operator+(const position2d<T>& pos) const
    {
        rect<T> ret(*this);
        ret.UpperLeftCorner += pos;
        ret.LowerRightCorner += pos;
        return ret;
    }

    //------------------------------------------------------------------------

    inline const rect<T>& operator+=(const position2d<T>& pos)
    {
        UpperLeftCorner += pos;
        LowerRightCorner += pos;
        return *this;
    }

    //------------------------------------------------------------------------

    inline bool operator == (const rect<T>& other) const
    {
        return UpperLeftCorner == other.UpperLeftCorner && LowerRightCorner == other.LowerRightCorner;
    }

    //------------------------------------------------------------------------
    
    inline bool operator != (const rect<T>& other) const
    {
        return UpperLeftCorner != other.UpperLeftCorner || LowerRightCorner != other.LowerRightCorner;
    }
    
    //------------------------------------------------------------------------
    
	template < class TT >
    inline const rect<T>& operator = (const rect < TT >& other)
    {
        UpperLeftCorner		= other.UpperLeftCorner;
        LowerRightCorner	= other.LowerRightCorner;
        return *this;
    }

    //------------------------------------------------------------------------
    // member methods
    //------------------------------------------------------------------------
    
    //! Returns if a 2d point is within this rectangle.
    //! \param pos: Position to test if it lies within this rectangle.
    //! \return Returns true if the position is within the rectangle, false if not.
    inline bool isPointInside(const position2d<T>& pos) const
    {
        return UpperLeftCorner.X <= pos.X && UpperLeftCorner.Y <= pos.Y &&
            LowerRightCorner.X >= pos.X && LowerRightCorner.Y >= pos.Y;
    }

    //------------------------------------------------------------------------
    
    //! Returns if the rectangle collides with an other rectangle.
    inline bool isRectCollided(const rect<T>& other) const
    {
        return (
            LowerRightCorner.Y > other.UpperLeftCorner.Y && UpperLeftCorner.Y < other.LowerRightCorner.Y &&
            LowerRightCorner.X > other.UpperLeftCorner.X && UpperLeftCorner.X < other.LowerRightCorner.X
            );
    }

    //------------------------------------------------------------------------
    
    //! Clips this rectangle with another one.
    inline void clipAgainst(const rect<T>& other) 
    {
        if (other.LowerRightCorner.X < LowerRightCorner.X)
            LowerRightCorner.X = other.LowerRightCorner.X;
        if (other.LowerRightCorner.Y < LowerRightCorner.Y)
            LowerRightCorner.Y = other.LowerRightCorner.Y;

        if (other.UpperLeftCorner.X > UpperLeftCorner.X)
            UpperLeftCorner.X = other.UpperLeftCorner.X;
        if (other.UpperLeftCorner.Y > UpperLeftCorner.Y)
            UpperLeftCorner.Y = other.UpperLeftCorner.Y;
    }

    //------------------------------------------------------------------------

    //! Returns width of rectangle.
    inline T getWidth() const
    {
        return LowerRightCorner.X - UpperLeftCorner.X;
    }

    //------------------------------------------------------------------------

    //! Returns height of rectangle.
    inline T getHeight() const
    {
        return LowerRightCorner.Y - UpperLeftCorner.Y;
    }

    //------------------------------------------------------------------------

    //! If the lower right corner of the rect is smaller then the upper left,
    //! the points are swapped.
    inline void repair()
    {
        if (LowerRightCorner.X < UpperLeftCorner.X)
        {
            s32 t = LowerRightCorner.X;
            LowerRightCorner.X = UpperLeftCorner.X;
            UpperLeftCorner.X = t;
        }

        if (LowerRightCorner.Y < UpperLeftCorner.Y)
        {
            s32 t = LowerRightCorner.Y;
            LowerRightCorner.Y = UpperLeftCorner.Y;
            UpperLeftCorner.Y = t;
        }
    }

    //------------------------------------------------------------------------

	//! Returns if the rect is valid to draw. It could be invalid, if
	//! The UpperLeftCorner is lower or more right than the LowerRightCorner,
	//! or if the area described by the rect is 0.
	inline bool isValid()
	{
		return (LowerRightCorner.X > UpperLeftCorner.X) &&
			(LowerRightCorner.Y > UpperLeftCorner.Y);
	}

    //------------------------------------------------------------------------

    //! Returns the center of the rectangle
    inline position2d<T> getCenter() const
    {
        return position2d<T>(
            (UpperLeftCorner.X + LowerRightCorner.X) / 2,
            (UpperLeftCorner.Y + LowerRightCorner.Y) / 2
            );
    }

    //------------------------------------------------------------------------

    inline void set(const T& _left, const T& _top, const T& _right, const T& _bottom) 
    {
        UpperLeftCorner.set(_left, _top);
        LowerRightCorner.set(_right, _bottom);
    }

    //------------------------------------------------------------------------

    inline void set(
        const position2d<T> &_UpperLeftCorner, const position2d<T> &_LowerRightCorner
        ) 
    {
        UpperLeftCorner = _UpperLeftCorner;
        LowerRightCorner = _LowerRightCorner;
    }

    //------------------------------------------------------------------------
    // member variables
    //------------------------------------------------------------------------

#if MY_COMPILER == MY_COMPILER_BORL

    position2d<T> UpperLeftCorner;
    position2d<T> LowerRightCorner;   

#else
      
    union 
    {
        struct
        {
            position2d<T> UpperLeftCorner;
            position2d<T> LowerRightCorner;
        };

        struct
        {
            T Left, Top, Right, Bottom;
        };
    };    

#endif

};

//----------------------------------------------------------------------------

//! Typedef for a float rect
typedef rect<f32> rectf;

//! Typedef for an integer rect
typedef rect<s32> recti;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef rectHPP

