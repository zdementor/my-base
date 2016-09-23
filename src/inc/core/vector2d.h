//|-------------------------------------------------------------------------
//| File:        vector2d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef vector2dHPP
#define vector2dHPP

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! 2d vector template class with lots of operators and methods.
template <class T>
class vector2d
{
public:

    //------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------

    vector2d()
		: X(0), Y(0) {}
    vector2d(T nx, T ny) 
		: X(nx), Y(ny) {}
	template <class TT>
    vector2d(const vector2d<TT> &other)  
		: X((T)other.X), Y((T)other.Y) {}

    //------------------------------------------------------------------------
    // operators
    //------------------------------------------------------------------------
    
	template <class TT>
	inline vector2d<T>& operator=(const vector2d<TT>& other)    
	{ 
		X = (T)other.X; Y = (T)other.Y; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator+(const vector2d<T>& other) const 
	{ 
		return vector2d<T>(X + other.X, Y + other.Y);   
	}

	//------------------------------------------------------------------------

    inline vector2d<T>& operator+=(const vector2d<T>& other)   
	{ 
		X += other.X; Y+=other.Y; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator-(const vector2d<T>& other) const 
	{ 
		return vector2d<T>(X - other.X, Y - other.Y);   
	}

	//------------------------------------------------------------------------

    inline vector2d<T>& operator-=(const vector2d<T>& other)   
	{ 
		X-=other.X; Y-=other.Y; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator*(const vector2d<T>& other) const 
	{ 
		return vector2d<T>(X * other.X, Y * other.Y);   
	}

	//------------------------------------------------------------------------

    inline vector2d<T>& operator*=(const vector2d<T>& other)   
	{ 
		X*=other.X; Y*=other.Y; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator*(const T v) const 
	{ 
		return vector2d<T>(X * v, Y * v);  
	}

	//------------------------------------------------------------------------

    inline vector2d<T>& operator*=(const T v) 
	{ 
		X*=v; Y*=v; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator/(const vector2d<T>& other) const 
	{ 
		return vector2d<T>(X / other.X, Y / other.Y);   }

	//------------------------------------------------------------------------

    inline vector2d<T>& operator/=(const vector2d<T>& other)   
	{ 
		X/=other.X; Y/=other.Y; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline vector2d<T> operator/(const T v) const 
	{ 
		return vector2d<T>(X / v, Y / v);  
	}

	//------------------------------------------------------------------------

    inline vector2d<T>& operator/=(const T v) 
	{ 
		X/=v; Y/=v; 
		return *this; 
	}

	//------------------------------------------------------------------------

    inline bool operator==(const vector2d<T>& other) const 
	{ 
		return other.X==X && other.Y==Y; 
	}

	//------------------------------------------------------------------------

    inline bool operator!=(const vector2d<T>& other) const 
	{ 
		return other.X!=X || other.Y!=Y; 
	}

    //------------------------------------------------------------------------
    // functions
    //------------------------------------------------------------------------

    inline void set(T nx, T ny) 
	{
		X=nx; Y=ny; 
	}

    //------------------------------------------------------------------------
    
	inline void set(const vector2d<T>& p) 
	{ 
		X=p.X; Y=p.Y;
	}
    
	//------------------------------------------------------------------------
    
	//! Returns the length of the vector
    //! \return Returns the length of the vector.
    inline f64 getLength() const 
	{ 
		return math::Sqrt((f64)(X*X + Y*Y)); 
	}
    
	//------------------------------------------------------------------------
    
	//! Returns the dot product of this vector with an other.
    inline T getDotProduct(const vector2d<T>& other) const
    {
        return X*other.X + Y*other.Y;
    }
    
	//------------------------------------------------------------------------
    
	//! Returns distance from an other point. Here, the vector is interpreted as
    //! point in 2 dimensional space.
    inline f64 getDistanceFrom(const vector2d<T>& other) const
    {
        f64 vx = X - other.X; f64 vy = Y - other.Y;
        return math::Sqrt(vx*vx + vy*vy);
    }
    
	//------------------------------------------------------------------------
    
	//! rotates the point around a center by an amount of degrees.
    inline void rotateBy(f64 degrees, const vector2d<T>& center)
    {
        degrees *=gradPI2;
        T cs = (T)cos(degrees);
        T sn = (T)sin(degrees);

        X -= center.X;
        Y -= center.Y;

        set(X*cs - Y*sn, X*sn + Y*cs);

        X += center.X;
        Y += center.Y;
    }
    
	//------------------------------------------------------------------------
    
	//! normalizes the vector.
    inline vector2d<T>& normalize()
    {
        T len = (T)getLength();

        //защита от деления на ноль (мало ли чего :) )
        if (len==0) len+=(T)0.000001;

        T inv = (T)1.0 /len;
        X *= inv;
        Y *= inv;
        return *this;
    }
    
	//------------------------------------------------------------------------
    
	//! Calculates the angle of this vector in grad.
    //! \return Returns a value between 0 and 360.
    inline f64 getAngleDeg() const
    {
        if (Y == 0.0)
            return X > 0.0 ? 90.0 : 180.0;

        f64 tmp = Y / sqrt(X*X + Y*Y);

        tmp = RAD2DEG*(atan(sqrt(1 - tmp*tmp) / tmp));

        if (X>0.0 && Y>0.0)
            return tmp + 270;
        else
        if (X>0.0 && Y<0.0)
            return tmp + 90;
        else
        if (X<0.0 && Y<0.0)
            return 90 - tmp;
        else
        if (X<0.0 && Y>0.0)
            return 270 - tmp;

        return tmp;
    }
    
	//------------------------------------------------------------------------
    
	//! Calculates the angle between this vector and another one in grad.
    //! \return Returns a value between 0 and 90.
    inline f64 getAngleDeg(const vector2d<T>& b) const
    {
        f64 tmp = X*b.X + Y*b.Y;

        if (tmp == 0.0)
            return 90.0;

        tmp = tmp / math::Sqrt((X*X + Y*Y) * (b.X*b.X + b.Y*b.Y));
        if (tmp < 0.0) tmp = -tmp;

		return RAD2DEG * (math::ATan(math::Sqrt(1 - tmp*tmp) / tmp));
    }
    
	//------------------------------------------------------------------------
    // member variables
    //------------------------------------------------------------------------
    
	T X, Y;
};

//----------------------------------------------------------------------------

//! Typedef for f32 2d vector.
typedef vector2d<f32> vector2df;

//! Typedef for integer 2d vector.
typedef vector2d<s32> vector2di;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef vector2dHPP

