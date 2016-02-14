//|-------------------------------------------------------------------------
//| File:        vector3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef vector3dHPP
#define vector3dHPP

//---------------------------------------------------------------------------
namespace my {
namespace core {
//---------------------------------------------------------------------------

const f32 _255_DIV_360_ = 255.0f / 360.0f;
const f32 _360_DIV_255_ = 360.0f / 255.0f;

//---------------------------------------------------------------------------

//! 3d vector template class with lots of operators and methods.
template <class T>
class vector3d  
{
public:

    //------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------

    vector3d()
		: X(0), Y(0), Z(0) {};
    vector3d(T nx, T ny, T nz) 
		: X(nx), Y(ny), Z(nz) {};
    vector3d(const vector3d<T>& other)  
		: X(other.X), Y(other.Y), Z(other.Z) {};

    //------------------------------------------------------------------------
    // operators
    //------------------------------------------------------------------------

    inline vector3d<T>& operator = (const vector3d<T> &other)    
	{ 
		X = (T)other.X; Y = (T)other.Y; Z = (T)other.Z; 
		return *this; 
	}
    inline vector3d<T>& operator = (const T &number)
	{ 
		X = (T)number; Y = (T)number; Z = (T)number; 
		return *this; 
	}

	//---------------------------------------------------------------------------

    inline vector3d<T> operator + (const vector3d<T> &other) const 
	{
		return vector3d<T>(X + (T)other.X, Y + (T)other.Y, Z + (T)other.Z);
	}
    inline vector3d<T> operator + (const T &number) const 
	{
		return vector3d<T>(X + (T)number, Y + (T)number, Z + (T)number);
	}

	//---------------------------------------------------------------------------

    inline vector3d<T>& operator += (const vector3d<T> &other)   
	{ 
		X += (T)other.X; Y += (T)other.Y; Z += (T)other.Z; 
		return *this; 
	}
    inline vector3d<T>& operator += (const T &number)   
	{ 
		X += (T)number; Y += (T)number; Z += (T)number; 
		return *this; 
	}

	//---------------------------------------------------------------------------

    inline vector3d<T> operator - (const vector3d<T> &other) const 
	{
		return vector3d<T>(X - (T)other.X, Y - (T)other.Y, Z - (T)other.Z);
	}
    inline vector3d<T> operator - (const T &number) const 
	{
		return vector3d<T>(X - (T)number, Y - (T)number, Z - (T)number);
	}

	//---------------------------------------------------------------------------

    inline vector3d<T>& operator -= (const vector3d<T>&other)   
	{ 
		X -= (T)other.X; Y -= (T)other.Y; Z -= (T)other.Z; 
		return *this; 
	}
    inline vector3d<T>& operator -= (const T &number)   
	{ 
		X -= (T)number; Y -= (T)number; Z -= (T)number; 
		return *this; 
	}

	//---------------------------------------------------------------------------

    inline vector3d<T> operator * (const vector3d<T> &other) const 
	{
		return vector3d<T>(X * (T)other.X, Y * (T)other.Y, Z * (T)other.Z);
	}
    inline vector3d<T> operator * (const T &number) const 
	{
		return vector3d<T>(X * (T)number, Y * (T)number, Z * (T)number);
	}

	//---------------------------------------------------------------------------

    inline vector3d<T>& operator *= (const vector3d<T> &other)   
	{ 
		X *= (T)other.X; Y *= (T)other.Y; Z *= (T)other.Z; 
		return *this; 
	}
    inline vector3d<T>& operator *= (const T &number)   
	{ 
		X *= (T)number; Y *= (T)number; Z *= (T)number; 
		return *this; 
	}

	//---------------------------------------------------------------------------

    inline vector3d<T> operator / (const vector3d<T> &other) const 
	{
		return vector3d<T>(X / (T)other.X, Y / (T)other.Y, Z / (T)other.Z);
	}
    inline vector3d<T> operator / (const T &number) const 
	{
		T i=(T)1.0/number; 
		return vector3d<T>(X * i, Y * i, Z * i);
	}

	//---------------------------------------------------------------------------

    inline vector3d<T>& operator /= (const vector3d<T> &other)   
	{ 
		X /= (T)other.X; Y /= (T)other.Y; Z /= (T)other.Z; 
		return *this; 
	}
    inline vector3d<T>& operator /= (const T &number)   
	{ 
		T i=(T)1.0/number; 
		X *= i; Y *= i; Z *= i; 
		return *this; 
	}

	//---------------------------------------------------------------------------

    inline bool operator <= (const vector3d<T> &other) const 
	{ 
		return X <= other.X && Y <= other.Y && Z <= other.Z;
	}

	//---------------------------------------------------------------------------

    inline bool operator >= (const vector3d<T>&other) const 
	{ 
		return X >= other.X && Y >= other.Y && Z >= other.Z;
	}

	//---------------------------------------------------------------------------

    inline bool operator == (const vector3d<T>& other) const 
	{ 
		return other.X == X && other.Y == Y && other.Z == Z; 
	}

	//---------------------------------------------------------------------------

    inline bool operator != (const vector3d<T>& other) const 
	{ 
		return other.X != X || other.Y != Y || other.Z != Z; 
	}

    //------------------------------------------------------------------------
    // functions
    //------------------------------------------------------------------------

    inline void set(T nx, T ny, T nz) 
	{
		X=nx; Y=ny; Z=nz; 
	}

    //------------------------------------------------------------------------

    inline void set(const vector3d<T>& p) 
	{ 
		X=p.X; Y=p.Y; Z=p.Z;
	}

    //------------------------------------------------------------------------

    //! Returns length of the vector.
    inline f32 getLength() const 
	{ 
		return math::Sqrt(X*X + Y*Y + Z*Z); 
	}

    //------------------------------------------------------------------------

    //! Returns squared length of the vector.
    //! This is useful because it is much faster then
    //! getLength().
    inline f32 getLengthSQ() const 
	{ 
		return X*X + Y*Y + Z*Z; 
	}

    //------------------------------------------------------------------------

    //! Returns the dot product with another vector.
    inline T getDotProduct(const vector3d<T>& other) const
    {
        return X*other.X + Y*other.Y + Z*other.Z;
    }

    //------------------------------------------------------------------------

    //! Returns distance from an other point. Here, the vector is interpreted as
    //! point in 3 dimensional space.
    inline f32 getDistanceFrom(const vector3d<T>& other) const
    {
        f32 vx = X - other.X; f32 vy = Y - other.Y; f32 vz = Z - other.Z;
        return math::Sqrt(vx*vx + vy*vy + vz*vz);
    }

    //------------------------------------------------------------------------

    //! Returns squared distance from an other point. 
    //! Here, the vector is interpreted as point in 3 dimensional space.
    inline f32 getDistanceFromSQ(const vector3d<T>& other) const
    {
        f32 vx = X - other.X; f32 vy = Y - other.Y; f32 vz = Z - other.Z;
        return (vx*vx + vy*vy + vz*vz);
    }

    //------------------------------------------------------------------------

    inline vector3d<T> getCrossProduct(const vector3d<T>& p) const
    {
        return vector3d<T>(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
    }

	//------------------------------------------------------------------------

	inline vector3d<T>& crossProduct(const vector3d<T>& p)
    {
		T x = X, y = Y, z = Z;

        X = y * p.Z - z * p.Y;
		Y = z * p.X - x * p.Z;
		Z = x * p.Y - y * p.X;

		return *this;
    }

    //------------------------------------------------------------------------

    inline bool isBetweenPoints(const vector3d<T>& begin, const vector3d<T>& end) const
    {
        f32 len_sq = (f32)(end - begin).getLengthSQ();

		f32 dist_beg_sq = (f32)getDistanceFromSQ(begin);
		f32 dist_end_sq = (f32)getDistanceFromSQ(end);

        return (dist_beg_sq<len_sq && dist_end_sq<len_sq);			
	}

    //------------------------------------------------------------------------

    //! Normalizes the vector.
    inline vector3d<T>& normalize()
    {
		if (X || Y || Z)
		{
			T fInvLength = math::InvSqrt( X*X + Y*Y + Z*Z );

			X *= fInvLength;
			Y *= fInvLength;
			Z *= fInvLength;
		}

        return *this;
    }

    //------------------------------------------------------------------------

    //! Sets the lenght of the vector to a new value
    inline void setLength(T newlength)
    {
        normalize();
        *this *= newlength;
    }

    //------------------------------------------------------------------------

    //! Inverts the vector.
    inline void invert()
    {
        X *= -1.0f;
        Y *= -1.0f;
        Z *= -1.0f;
    }

    //------------------------------------------------------------------------

    inline void rotateXZByDegrees(f32 degrees, const vector3d<T>& center)
    {
        rotateXZByRadians(core::DEG2RAD*(degrees), center);
    }

    //------------------------------------------------------------------------

    inline void rotateXYByDegrees(f32 degrees, const vector3d<T>& center)
    {
		rotateXYByRadians(core::DEG2RAD*(degrees), center);
    }

    //------------------------------------------------------------------------

    inline void rotateYZByDegrees(f32 degrees, const vector3d<T>& center)
    {
        rotateYZByRadians(core::DEG2RAD*(degrees), center);
    }

    //------------------------------------------------------------------------

    inline void rotateXZByRadians(f32 radians, const vector3d<T>& center)
    {
        T cs = (T)math::Cos(radians);
        T sn = (T)math::Sin(radians);
        X -= center.X;
        Z -= center.Z;
        set(X*cs - Z*sn, Y, X*sn + Z*cs);
        X += center.X;
        Z += center.Z;
    }

    //------------------------------------------------------------------------

    inline void rotateXYByRadians(f32 radians, const vector3d<T>& center)
    {
        T cs = (T)math::Cos(radians);
        T sn = (T)math::Sin(radians);
        X -= center.X;
        Y -= center.Y;
        set(X*cs - Y*sn, X*sn + Y*cs, Z);
        X += center.X;
        Y += center.Y;
    }

    //------------------------------------------------------------------------

    inline void rotateYZByRadians(f32 radians, const vector3d<T>& center)
    {
        T cs = (T)math::Cos(radians);
        T sn = (T)math::Sin(radians);
        Z -= center.Z;
        Y -= center.Y;
        set(X, Y*cs - Z*sn, Y*sn + Z*cs);
        Z += center.Z;
        Y += center.Y;
    }

    //------------------------------------------------------------------------

    //! returns interpolated vector
    //! \param other: other vector to interpolate between
    //! \param d: value between 0.0f and 1.0f.
    inline vector3d<T> getInterpolated(const vector3d<T>& other, f32 d) const
    {
        f32 inv = 1.0f - d;
        return vector3d<T>(
			other.X*inv + X*d, other.Y*inv + Y*d, other.Z*inv + Z*d);
    }

    //------------------------------------------------------------------------

    //! returns interpolated vector
    //! \param other: other vector to interpolate between
    //! \param d: value between 0.0f and 1.0f.
    inline const vector3d<T>& interpolate(const vector3d<T>& other, f32 d)
    {
        f32 inv = 1.0f - d;
        X = other.X*inv + X*d;
		Y = other.Y*inv + Y*d;
		Z = other.Z*inv + Z*d;
		return *this;
    }

	//------------------------------------------------------------------------

	//! Gets the Y and Z rotations of a vector.
	//! \return A vector representing the rotation in degrees of
	//! this vector. The Z component of the vector will always be 0. 
	vector3d<T> getHorizontalAngle()
	{
		vector3d<T> angle;

		angle.Y = (T)atan2(X, Z); 
		angle.Y *= (f32)RAD2DEG;
			    
		if (angle.Y < 0.0f) angle.Y += 360.0f; 
		if (angle.Y >= 360.0f) angle.Y -= 360.0f; 
			    
		f32 z1 = (f32)sqrt(X*X + Z*Z); 
			    
		angle.X = (T)atan2(z1, Y); 
		angle.X *= (f32)RAD2DEG;
		angle.X -= 90.0f; 
			    
		if (angle.X < 0.0f) angle.X += 360.0f; 
		if (angle.X >= 360.0f) angle.X -= 360.0f; 

		return angle;
	}

    //------------------------------------------------------------------------

    //! calc angle between two vectors (degrees)
    inline f32 getAngleDeg(const vector3d<T>& other)
    {   
        f32 deg  = (f32)getAngleRad(other)*RAD2DEG;
        return deg;
    }

    //------------------------------------------------------------------------

    //! calc angle between two vectors (radians)
    inline f32 getAngleRad(const vector3d<T>& other)
    {
		f32 div  = getLength()*other.getLength();
        f32 _cos = (getDotProduct(other))/(div!=0?div:0.000001);
        f32 rad  = math::ACos(_cos);
        return rad;
    }

	//------------------------------------------------------------------------

	//! Converting vector to lattitude and longitude
	//
	// We use two byte encoded normals in some space critical applications.
	// Lat = 0 at (1,0,0) to 360 (-1,0,0), encoded in 8-bit sine table format
	// Lng = 0 at (0,0,1) to 180 (0,0,-1), encoded in 8-bit sine table format
	inline void toLatLong(u8 latlong[2]) 
	{
		// check for singularities
		if ( X == 0 && Y == 0 ) 
		{
			if ( Z > 0 ) 
			{
				latlong[0] = 0;
				latlong[1] = 0;       // lat = 0, long = 0
			} 
			else 
			{
				latlong[0] = 128;
				latlong[1] = 0;       // lat = 0, long = 128
			}
		} 
		else 
		{
			int lattitude, longitude;

			lattitude = atan2( Y, X ) * RAD2DEG * _255_DIV_360_;
			lattitude &= 0xff;

			longitude = acos( Z ) * RAD2DEG * _255_DIV_360_;
			longitude &= 0xff;

			latlong[0] = longitude;   
			latlong[1] = lattitude;  
		}
	}

	//------------------------------------------------------------------------

	//! Converting lattitude and longitude to vector
	//
	// Decode X as cos( lat ) * sin( long )
	// Decode Y as sin( lat ) * sin( long )
	// Decode Z as cos( long )
	inline void setFromLatLong(u8 latlong[2]) 
	{
		float longitude = f32(latlong[0]) * _360_DIV_255_ * DEG2RAD;
		float lattitude = f32(latlong[1]) * _360_DIV_255_ * DEG2RAD;

		X = cos(lattitude) * sin(longitude);
		Y = sin(lattitude) * sin(longitude);
		Z = cos(longitude);
	}

	//------------------------------------------------------------------------

	//! Returns a vector at a point half way between this and the passed
    //! in vector.
    inline vector3d<T> midPoint( const vector3d<T>& vec ) const
	{
		return vector3d<T>(
			( X + vec.X ) * 0.5, ( Y + vec.Y ) * 0.5, ( Z + vec.Z ) * 0.5 
			);
	}

    //------------------------------------------------------------------------
    // member variables
    //------------------------------------------------------------------------

    T X, Y, Z;
};

//---------------------------------------------------------------------------

//! Typedef for a f32 3d vector.
typedef vector3d<f32> vector3df;

//! Typedef for an integer 3d vector.
typedef vector3d<s32> vector3di;

//---------------------------------------------------------------------------
} // end namespac core
} // end namespac my
//---------------------------------------------------------------------------

#endif

