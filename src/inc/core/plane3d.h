//|-------------------------------------------------------------------------
//| File:        plane3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef plane3dHPP
#define plane3dHPP
//-----------------------------------------------------------------------------

#include "vector3d.h"
#include "line3d.h"

//-----------------------------------------------------------------------------
namespace my {
namespace core {
//-----------------------------------------------------------------------------

//! Enumeration for intersection relations of 3d objects
enum E_INTERSECTION_RELATION_3D 
{
    EIREL3D_FRONT = 0,
    EIREL3D_BEHIND,
    EIREL3D_PLANAR
};

//-----------------------------------------------------------------------------

//! Template plane class with some intersection testing methods.
template <class T>
class plane3d
{
public:

    //------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------

    plane3d() : Normal(0,1,0) 
	{ 
		recalculateD(vector3d<T>(0,0,0)); 
	}

    plane3d(const vector3d<T>& point, const vector3d<T>& normal) 
		: Normal(normal) 
	{ 
		Normal.normalize();

		recalculateD(point); 
	}

    plane3d(T px, T py, T pz, T nx, T ny, T nz) 
		: Normal(nx, ny, nz) 
	{ 
		Normal.normalize();

		recalculateD(vector3d<T>(px, py, pz)); 
	}

    plane3d(const plane3d<T>& other) 
		: D(other.D), Normal(other.Normal) {}

    plane3d(const vector3d<T>& p1, const vector3d<T>& p2, const vector3d<T>& p3)    
	{ 
		set(p1, p2, p3); 
	}

    //------------------------------------------------------------------------
    // operators
    //------------------------------------------------------------------------

    inline bool operator==(const plane3d<T>& other) const 
	{ 
		return (D==other.D && Normal==other.Normal);
	}

    inline bool operator!=(const plane3d<T>& other) const 
	{ 
		return !(D==other.D && Normal==other.Normal);
	}

    //------------------------------------------------------------------------
    // functions
    //------------------------------------------------------------------------

    inline void set(const vector3d<T>& point, const vector3d<T>& nvector)
    {
        Normal = nvector;
        Normal.normalize();
        recalculateD(point);
    }

    //------------------------------------------------------------------------

    inline void set(const vector3d<T>& nvect, T d)
    {
        Normal = nvect;
        D = d;
    }

    //------------------------------------------------------------------------

    inline void set(const vector3d<T>& p1, const vector3d<T>& p2, const vector3d<T>& p3)
    {
        // creates the plane from 3 memberpoints
        Normal = (p2 - p1).getCrossProduct(p3 - p1);
        Normal.normalize();

        recalculateD(p1);
    }

    //------------------------------------------------------------------------

    //! Returns an intersection with a 3d line.
    //! \param lineVect: Vector of the line to intersect with.
    //! \param linePoint: Point of the line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLine(
		const vector3d<T>& linePoint, const vector3d<T>& lineVect,
        vector3d<T>& outIntersection
		) const
    {
        T t2 = Normal.getDotProduct(lineVect);

        if (t2 == 0)
            return false;

        T t =- (Normal.getDotProduct(linePoint) + D) / t2;         
        outIntersection = linePoint + (lineVect * t);

        return true;
    }

	//------------------------------------------------------------------------

    //! Returns an intersection with a 3d line.
    //! \param line: line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLine(
		const line3d<T>& line, vector3d<T>& outIntersection
		) const
    {
        return (
			getIntersectionWithLine(line.start, line.end - line.start, outIntersection)
			);
    }

    //------------------------------------------------------------------------

    //! Returns an intersection with a 3d line, limited between two 3d points.
    //! \param linePoint1: Point 1 of the line.
    //! \param linePoint2: Point 2 of the line.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLimitedLine(
		const vector3d<T>& linePoint1, const vector3d<T>& linePoint2, 
		vector3d<T>& outIntersection
		) const
    {
        return (
			getIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
            outIntersection.isBetweenPoints(linePoint1, linePoint2)
			);
    }

	//------------------------------------------------------------------------

    //! Returns an intersection with a 3d line, limited between two 3d points.
    //! \param line: line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLimitedLine(
		const line3d<T>& line, vector3d<T>& outIntersection
		) const
    {
        return (
			getIntersectionWithLine(line, outIntersection) &&
            outIntersection.isBetweenPoints(line.start, line.end)
			);
    }
    //------------------------------------------------------------------------

    //! Returns an intersection with a ray.
    //! \param linePoint1: Point 1 of the line.
    //! \param lineVect: normalized direction of the ray to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithRay(
		const vector3d<T>& rayPoint, const vector3d<T>& rayVect, 
		vector3d<T>& outIntersection,
		f32 rayLength = 1000000.0f
		) const
    {
		vector3d<T> point2 = rayPoint  + rayVect * rayLength;
        return (
			getIntersectionWithLine(rayPoint, rayVect, outIntersection) &&
            outIntersection.isBetweenPoints(rayPoint, point2));
    }
    //------------------------------------------------------------------------

    //! Returns a projection of the vector
    inline vector3d<T> getProjection( const vector3d<T>& vec ) const
	{
		vector3d<T> memberPoint = getMemberPoint();
		vector3d<T> linePoint   = memberPoint + vec;

		vector3d<T> outIntersection;
		getIntersectionWithLine(linePoint, Normal, outIntersection);

		//vector3d<T> outProjection = outIntersection - linePoint;
		vector3d<T> outProjection = outIntersection - memberPoint;
        
        return outProjection;
    }

    //------------------------------------------------------------------------

    //! Classifies the relation of a point to this plane.
    //! \param point: Point to classify its relation.
    //! \return Returns EIREL3D_FRONT if the point is in front of the plane,
    //! EIREL3D_BEHIND if the point is behind of the plane, and
    //! EIREL3D_PLANAR if the point is within the plane.
    inline E_INTERSECTION_RELATION_3D classifyPointRelation(const vector3d<T>& point) const
    {
        f32 d = Normal.getDotProduct(point) + D;

        if (d < -ROUNDING_ERROR_32)
            return EIREL3D_BEHIND;

        if (d > ROUNDING_ERROR_32)
            return EIREL3D_FRONT;

        return EIREL3D_PLANAR;
    }

	//---------------------------------------------------------------------------

	//! Classifies the relation of a sphere to this plane.
    //! \param center: Center of the sphere to classify its relation.
	//! \param radius: Radius of the sphere to classify its relation.
    //! \return Returns EIREL3D_FRONT if the sphere is in front of the plane,
    //! EIREL3D_BEHIND if the sphere is behind of the plane, and
    //! EIREL3D_PLANAR if the sphere is intersects with the plane.
	inline E_INTERSECTION_RELATION_3D classifySphereRelation( const core::vector3df& center, f32 radius ) const
	{
		f32 d = Normal.getDotProduct(center) + D;

		if (d < -ROUNDING_ERROR_32)
		{
			if (-d>=radius)
				return EIREL3D_BEHIND;
		}

        if (d > ROUNDING_ERROR_32)
		{
			if (d>=radius)
				return EIREL3D_FRONT;
		}	

		return EIREL3D_PLANAR;
	}

    //------------------------------------------------------------------------

    //! Recalculates the distance from origin by applying
    //! a new member point to the plane.
    inline void recalculateD(const vector3d<T>& MPoint)
    {
        D = - MPoint.getDotProduct(Normal);
    }

    //------------------------------------------------------------------------

    //!\return Returns a member point of the plane.
    inline vector3d<T> getMemberPoint() const
    {
        return Normal * -D;
    }

    //------------------------------------------------------------------------

    //! Tests if there is a intersection between this plane and another
    //! \return Returns true if there is a intersection.
    inline bool existsInterSection(const plane3d<T>& other) const
    {
        vector3d<T> cross = other.Normal.getCrossProduct(Normal);
        return cross.getLength() > 1e-08f;
    }

    //------------------------------------------------------------------------

    //! Intersects this plane with another.
    //! \return Returns true if there is a intersection, false if not.
    inline bool getIntersectionWithPlane(
		const plane3d<T>& other, vector3d<T>& outLinePoint, vector3d<T>& outLineVect
		) const
    {
        f64 fn00 = Normal.getLength();
        f64 fn01 = Normal.getDotProduct(other.Normal);
        f64 fn11 = other.Normal.getLength();
        f64 det = fn00*fn11 - fn01*fn01;

        if (fabs(det) < 1e-08f)
            return false;

        det = 1.0 / det;
        f64 fc0 = (fn11*-D + fn01*other.D) * det;
        f64 fc1 = (fn00*-other.D + fn01*D) * det;

        outLineVect = Normal.getCrossProduct(other.Normal);
        outLinePoint = Normal*(T)fc0 + other.Normal*(T)fc1;
        return true;
    }

    //------------------------------------------------------------------------

    //! Returns the intersection point with two other planes if there is one.
    inline bool getIntersectionWithPlanes(
		const plane3d<T>& o1, const plane3d<T>& o2, vector3d<T>& outPoint
		) const
    {
        vector3d<T> linePoint, lineVect;
        if (getIntersectionWithPlane(o1, linePoint, lineVect))
            return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

        return false;
    }

    //------------------------------------------------------------------------

    //! Returns if the plane is front of backfacing. Note that this only
    //! works if the normal is Normalized.
    //! \param lookDirection: Look direction.
    //! \return Returns true if the plane is front facing, which mean it would
    //! be visible, and false if it is backfacing.
    inline bool isFrontFacing(const vector3d<T>& lookDirection) const
    {
        return Normal.getDotProduct(lookDirection) <= 0.0f;
    }

    //------------------------------------------------------------------------

    //! Returns the distance to a point.  Note that this only
    //! works if the normal is Normalized.
    inline T getDistanceTo(const vector3d<T>& point) const
    {
        return point.getDotProduct(Normal) + D;
    }

    //------------------------------------------------------------------------
    // member variables
    //------------------------------------------------------------------------

    T D;                    // distance from origin
    vector3d<T> Normal;     // normal vector
};

//-----------------------------------------------------------------------------

//! Typedef for a f32 3d plane.
typedef plane3d<f32> plane3df;

//! Typedef for an integer 3d plane.
typedef plane3d<s32> plane3di;

//-----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//-----------------------------------------------------------------------------

#endif

