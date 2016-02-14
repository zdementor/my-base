//|-------------------------------------------------------------------------
//| File:        triangle3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef triangle3dHPP
#define triangle3dHPP
//----------------------------------------------------------------------------

#include "vector3d.h"
#include "line3d.h"
#include "plane3d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------   

//! 3d triangle template class for doing collision detection and other things.
template <class T>
class triangle3d
{
public:

	triangle3d() {}

	triangle3d(const vector3d<T>& a, const vector3d<T>& b, const vector3d<T>& c)
	{
		set(a, b, c);
	}

    //------------------------------------------------------------------------

    inline bool operator==(const triangle3d<T>& other) const 
	{ 
		return other.A==A && other.B==B && other.C==C; 
	}

	//------------------------------------------------------------------------

    inline bool operator!=(const triangle3d<T>& other) const 
	{ 
		return other.A!=A || other.B!=B || other.C!=C; 
	}

    //------------------------------------------------------------------------

    //! Returns the closest point on a triangle to a point on the same plane.
    //! \param p: Point which must be on the same plane as the triangle.
    inline vector3df closestPointOnTriangle(const vector3df& p) const
    {
        vector3df rab = line3d<f32>(A, B).getClosestPoint(p);
        vector3df rbc = line3d<f32>(B, C).getClosestPoint(p);
        vector3df rca = line3d<f32>(C, A).getClosestPoint(p);

        T d1 = (T)rab.getDistanceFrom(p);
        T d2 = (T)rbc.getDistanceFrom(p);
        T d3 = (T)rca.getDistanceFrom(p);

        if (d1 < d2)
            return d1 < d3 ? rab : rca;
        
        return d2 < d3 ? rbc : rca;
    }

    //------------------------------------------------------------------------

    //! Returns if a point is inside the triangle
    //! \param p: Point to test. Assumes that this point is already on the plane
    //! of the triangle.
    //! \return Returns true if the point is inside the triangle, otherwise false.
    inline bool isPointInside(const vector3d<T>& p) const
    {
        return (isOnSameSide(p, A, B, C) &&
            isOnSameSide(p, B, A, C) &&
            isOnSameSide(p, C, A, B));
    }

    //------------------------------------------------------------------------

    //! Returns if a point is inside the triangle. This method is an implementation
    //! of the example used in a paper by Kasper Fauerby original written
    //! by Keidy from Mr-Gamemaker.
    //! \param p: Point to test. Assumes that this point is already on the plane
    //! of the triangle.
    //! \return Returns true if the point is inside the triangle, otherwise false.
    inline bool isPointInsideFast(const vector3d<T>& p) const
    {
        vector3d<T> f = B - A;
        vector3d<T> g = C - A;

        f32 a = f.getDotProduct(f);
        f32 b = f.getDotProduct(g);
        f32 c = g.getDotProduct(g);

        f32 ac_bb = (a*c)-(b*b);
        vector3d<T> vp = p - A;

        f32 d = vp.getDotProduct(f);
        f32 e = vp.getDotProduct(g);
        f32 x = (d*c)-(e*b);
        f32 y = (e*a)-(d*b);
        f32 z = x+y-ac_bb;

        return (( ((u32&)z)& ~(((u32&)x)|((u32&)y))) & 0x80000000)!=0;
    }

    //------------------------------------------------------------------------

    inline bool isOnSameSide(
		const vector3d<T>& p1, const vector3d<T>& p2, 
        const vector3d<T>& a, const vector3d<T>& b
		) const
    {
        vector3d<T> bminusa = b - a;
        vector3d<T> cp1 = bminusa.getCrossProduct(p1 - a);
        vector3d<T> cp2 = bminusa.getCrossProduct(p2 - a);
        
		return (cp1.getDotProduct(cp2) > 0.0f);
    }


    //------------------------------------------------------------------------

    //! Returns an intersection with a 3d limited line.
    //! \param line: 3d line to test intersection with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLimitedLine(
		const line3d<T>& line, vector3d<T>& outIntersection
		) const
    {
        return 
			getIntersectionWithLimitedLine(line.start, line.end, outIntersection);			
    }

    //------------------------------------------------------------------------

    //! Returns an intersection with a 3d limited line.
    //! \param line_start: Vector of the line to intersect with.
    //! \param line_end: Point of the line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLimitedLine(
		const vector3d<T>& line_start, const vector3d<T>& line_end, vector3d<T>& outIntersection
		) const
    {
        return 
			getIntersectionWithLine(
				line_start,
				line_end-line_start, outIntersection
				) &&
            outIntersection.isBetweenPoints(line_start, line_end);
    }

    //------------------------------------------------------------------------

    //! Returns an intersection with a 3d line.
    //! Please note that also points are returned as intersection, which
    //! are on the line, but not between the start and end point of the line.
    //! If you want the returned point be between start and end, please
    //! use getIntersectionWithLimitedLine().
    //! \param lineVect: Vector of the line to intersect with.
    //! \param linePoint: Point of the line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionWithLine(
		const vector3d<T>& linePoint,
        const vector3d<T>& lineVect, vector3d<T>& outIntersection
		) const
    {
        if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
            return isPointInside(outIntersection);

        return false;           
    }

    //------------------------------------------------------------------------

    //! Calculates the intersection between a 3d line and 
    //! the plane the triangle is on.
    //! \param lineVect: Vector of the line to intersect with.
    //! \param linePoint: Point of the line to intersect with.
    //! \param outIntersection: Place to store the intersection point, if there is one.
    //! \return Returns true if there was an intersection, false if there was not.
    inline bool getIntersectionOfPlaneWithLine(
		const vector3d<T>& linePoint,
        const vector3d<T>& lineVect, vector3d<T>& outIntersection
		) const
    {
        vector3d<T> normal = getNormal();
        T t2 = normal.getDotProduct(lineVect);
        
        if (t2 == 0.0f)
            return false;

        T d = A.getDotProduct(normal);
        T t =- (normal.getDotProduct(linePoint) - d) / t2;
        outIntersection = linePoint + (lineVect * t);
        return true;
    }

    //------------------------------------------------------------------------  
	
    //! Returns the normal of the triangle.
    //! Please note: The normal is not normalized.
    inline vector3d<T> getNormal() const
    {
        return (B - A).getCrossProduct(C - A);
    }

    //------------------------------------------------------------------------

    //! Returns if the triangle is front of backfacing.
    //! \param lookDirection: Look direction.
    //! \return Returns true if the plane is front facing, which mean it would
    //! be visible, and false if it is backfacing.
    inline bool isFrontFacing(const vector3d<T>& lookDirection) const
    {
        vector3d<T> n = getNormal();
        n.normalize();
        return n.getDotProduct(lookDirection) <= 0.0f;
    }

    //------------------------------------------------------------------------

    //! Returns the plane of this triangle.
    inline plane3d<T> getPlane() const
    {
        return plane3d<T>(A, B, C);
    }

    //------------------------------------------------------------------------

    inline void set(const vector3d<T>& a, const vector3d<T>& b, const vector3d<T>& c)
    {
        A = a;
        B = b;
        C = c;
    }

	//------------------------------------------------------------------------

    //! split triangle into back and front triangles
    inline bool splitByPlane(
		plane3d<T> &plane, 
		array < triangle3d<T> > &back_tri_list,
		array < triangle3d<T> > &front_tri_list
		) const
	{
		vector3d<T> ab, bc, ac;

		E_INTERSECTION_RELATION_3D a_rel = plane.classifyPointRelation(A);
		E_INTERSECTION_RELATION_3D b_rel = plane.classifyPointRelation(B);
		E_INTERSECTION_RELATION_3D c_rel = plane.classifyPointRelation(C);

		s32 planar_rel = 
			((a_rel==EIREL3D_PLANAR)?1:0) + ((b_rel==EIREL3D_PLANAR)?1:0) + ((c_rel==EIREL3D_PLANAR)?1:0);

		bool ab_intersect = plane.getIntersectionWithLimitedLine(A, B, ab);
		bool bc_intersect = plane.getIntersectionWithLimitedLine(B, C, bc);
		bool ac_intersect = plane.getIntersectionWithLimitedLine(A, C, ac);

		if (planar_rel==1)
		{	
			if (a_rel==EIREL3D_PLANAR)
			{
				if (b_rel==c_rel)
				// this variant
				//      /  
				//   A o---------o C
				//    / \       /
				//   /   \     /
				//        \   /   
				//         \ /     
				//          o B  
				{
					if (b_rel == EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}
					else
					{
						back_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}
				}
				else
				// this variant
				//    \    
				//   A o\--------o C
				//      \ \     /
				//       \  \  /
				//        \   . bc
				//         \ / \   
				//          o B  
				{
					if (c_rel==EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( C,  A, bc ) );
						back_tri_list.push_back ( triangle3d<T>( B, bc,  A ) );
					}
					else
					{
						back_tri_list.push_back ( triangle3d<T>( C,  A, bc ) );
						front_tri_list.push_back( triangle3d<T>( B, bc,  A ) );
					}				
				}
			}
			else
			if (b_rel==EIREL3D_PLANAR)
			{
				if (a_rel==c_rel)
				// this variant
				//          
				//   A o---------o C
				//      \       /
				//       \     /
				//        \   /   
				//         \ /B    
				//     -----o-----
				{
					if (a_rel == EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}
					else
					{
						back_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}	
				}
				else
				// this variant
				//          |
				//   A o----.----o C
				//      \   |ac /
				//       \  |  /    
				//        \ | /
				//         \|/
				//          o B
				{
					if (a_rel==EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( A, B, ac ) );
						back_tri_list.push_back ( triangle3d<T>( C, ac, B ) );
					}
					else
					{
						back_tri_list.push_back ( triangle3d<T>( A, B, ac ) );
						front_tri_list.push_back( triangle3d<T>( C, ac, B ) );
					}
				}
			}
			else
			{
				if (a_rel==b_rel)
				/* this variant          */
				/*              \        */
				/*   A o---------o C     */
				/*      \       / \      */
				/*       \     /   \     */
				/*        \   /          */
				/*         \ /           */
				/*          o B          */
				{
					if (a_rel == EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}
					else
					{
						back_tri_list.push_back( triangle3d<T>( A, B, C ) );
					}	
				}
				else
				// this variant
				//                /   
				//   A o--------/o C  
				//      \     / /  
				//       \  /  /
				//     ab .   /
				//      /  \ /     
				//          o B
				{
					if (b_rel==EIREL3D_FRONT)
					{
						front_tri_list.push_back( triangle3d<T>( B, C, ab ) );
						back_tri_list.push_back ( triangle3d<T>( A, ab, C ) );
					}
					else
					{
						back_tri_list.push_back ( triangle3d<T>( B, C, ab ) );
						front_tri_list.push_back( triangle3d<T>( A, ab, C ) );
					}
				}
			}

			return true;
		}
		else if (planar_rel==2)
		// one of this variants:              /  \             
		//                                   /    \            
		// ----o---------o----    o---------o      o---------o   
		//    A \       / C      A \       / C    A \       / C
		//       \     /            \     /          \     /   
		//        \   /              \   /            \   /    
		//         \ /B               \ /B             \ /B    
		//          o                  o                o      
        //                            /                  \     
		{
			if (a_rel==EIREL3D_FRONT || b_rel==EIREL3D_FRONT || c_rel==EIREL3D_FRONT)			
			{
				front_tri_list.push_back( triangle3d<T>( A, B, C ) );				
			}
			else
			{
				back_tri_list.push_back( triangle3d<T>( A, B, C ) );
			}
			return true;
		}
		else if (planar_rel==3)
		{
			front_tri_list.push_back( triangle3d<T>( A, B, C ) );

			return true;
		}	

		if ((!ab_intersect && !bc_intersect) || (!bc_intersect && !ac_intersect) ||
			(!ab_intersect && !ac_intersect)
			)
		{
			return false;
		}

		if (ab_intersect && bc_intersect && !ac_intersect)
		// this variant
		// A o---------o C
		//    \       /
		//  ---.-----.----
		//   ab \   /  bc
		//       \ /
		//        o B
		{
			if (b_rel==EIREL3D_FRONT)
			{
				front_tri_list.push_back( triangle3d<T>( B, bc, ab ) );				
				back_tri_list.push_back ( triangle3d<T>( C, ab, bc ) );
				back_tri_list.push_back ( triangle3d<T>( C, A,  ab ) );			
			}
			else
			{
				back_tri_list.push_back ( triangle3d<T>( B, bc, ab ) );				
				front_tri_list.push_back( triangle3d<T>( C, ab, bc ) );
				front_tri_list.push_back( triangle3d<T>( C, A,  ab ) );				
			}
			return true;
		}
		else
		if (!ab_intersect && bc_intersect && ac_intersect)
		// this variant
		//        ac |
		// A o-------.-o C
		//    \      |/
		//     \     . bc
		//      \   /|
		//       \ / |
		//        o B
		{
			if (c_rel==EIREL3D_FRONT)
			{
				front_tri_list.push_back( triangle3d<T>( C, ac, bc ) );				
				back_tri_list.push_back ( triangle3d<T>( A, bc, ac ) );
				back_tri_list.push_back ( triangle3d<T>( A, B,  bc ) );
			}
			else
			{
				back_tri_list.push_back ( triangle3d<T>( C, ac, bc ) );				
				front_tri_list.push_back( triangle3d<T>( A, bc, ac ) );
				front_tri_list.push_back( triangle3d<T>( A, B,  bc ) );
			}
			return true;
		}
		else
		if (ab_intersect && !bc_intersect  && ac_intersect)
		// this variant
		//     | ac
		// A o-.-------o C
		//    \|      / 
		//     . ab  /
		//     |\   /
		//     | \ /
		//        o B
		{
			if (a_rel==EIREL3D_FRONT)
			{
				front_tri_list.push_back( triangle3d<T>( A, ab, ac ) );				
				back_tri_list.push_back ( triangle3d<T>( B, ac, ab ) );
				back_tri_list.push_back ( triangle3d<T>( B, C,  ac ) );
			}
			else
			{
				back_tri_list.push_back ( triangle3d<T>( A, ab, ac ) );				
				front_tri_list.push_back( triangle3d<T>( B, ac, ab ) );
				front_tri_list.push_back( triangle3d<T>( B, C,  ac ) );
			}	
			return true;
		}

		return false;
	}	

	//------------------------------------------------------------------------

    inline bool isValide()
	{ 
		return 
			(!math::Equals(A.X, B.X) || !math::Equals(A.Y, B.Y) || !math::Equals(A.Z, B.Z)) &&
			(!math::Equals(B.X, C.X) || !math::Equals(B.Y, C.Y) || !math::Equals(B.Z, C.Z)) &&
			(!math::Equals(A.X, C.X) || !math::Equals(A.Y, C.Y) || !math::Equals(A.Z, C.Z)); 
	}
    
    //------------------------------------------------------------------------

    //! the three points of the triangle
    vector3d<T> A; 
    vector3d<T> B; 
    vector3d<T> C; 
};

//----------------------------------------------------------------------------

//! Typedef for a f32 3d triangle.
typedef triangle3d<f32> triangle3df;

//! Typedef for an integer 3d triangle.
typedef triangle3d<s32> triangle3di;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespac my
//----------------------------------------------------------------------------

#endif

