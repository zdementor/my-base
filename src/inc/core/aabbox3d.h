//|-------------------------------------------------------------------------
//| File:        aabbox3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef aabbox3dHPP
#define aabbox3dHPP
//----------------------------------------------------------------------------

#include "plane3d.h"
#include "line3d.h"
#include "array.h"
#include "triangle3d.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! Axis aligned bounding box in 3d dimensional space.
//! Has some useful methods used with occlusion culling or clipping.

template <class T>
class aabbox3d
{
public:

    //------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------

    aabbox3d()
		: MinEdge(-1,-1,-1), MaxEdge(1,1,1) {}
	aabbox3d(const vector3d<T> &init)
		: MinEdge(init), MaxEdge(init) {}
    aabbox3d(const vector3d<T> &min, const vector3d<T> &max)
		: MinEdge(min), MaxEdge(max) {}
    aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz)
		: MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {}
	aabbox3d(const aabbox3d<T> &box)
		: MinEdge(box.MinEdge), MaxEdge(box.MaxEdge) {}

    //------------------------------------------------------------------------
    // operators
    //------------------------------------------------------------------------

    inline bool operator==(const aabbox3d<T>& other) const 
	{ 
		return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
	}

	//------------------------------------------------------------------------
    
	inline bool operator!=(const aabbox3d<T>& other) const 
	{
		return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
	}

    //------------------------------------------------------------------------
    // functions
    //------------------------------------------------------------------------

    //! Adds a point to the bounding box, causing it to grow bigger, 
    //! if point is outside of the box
    //! \param p: Point to add into the box.
    inline void addInternalPoint(const vector3d<T>& p)
    {
        addInternalPoint(p.X, p.Y, p.Z);
    }

    //------------------------------------------------------------------------

    //! Adds an other bounding box to the bounding box, causing it to grow bigger,
    //! if the box is outside of the box
    //! \param b: Other bounding box to add into this box.
    inline void addInternalBox(const aabbox3d<T>& b)
    {
        addInternalPoint(b.MaxEdge);
        addInternalPoint(b.MinEdge);
    }

    //------------------------------------------------------------------------

    //! Resets the bounding box.
    inline void reset(T x, T y, T z)
    {
        MaxEdge.set(x,y,z);
        MinEdge = MaxEdge;
    }

    //------------------------------------------------------------------------

    //! Resets the bounding box.
    inline void reset(const aabbox3d<T>& initValue)
    {
        *this = initValue;
    }

    //------------------------------------------------------------------------

    //! Resets the bounding box.
    inline void reset(const vector3d<T>& initValue)
    {
        MaxEdge = initValue;
        MinEdge = initValue;
    }

    //------------------------------------------------------------------------

    //! Adds a point to the bounding box, causing it to grow bigger, 
    //! if point is outside of the box.
    //! \param x: X Coordinate of the point to add to this box.
    //! \param y: Y Coordinate of the point to add to this box.
    //! \param z: Z Coordinate of the point to add to this box.
    inline void addInternalPoint(T x, T y, T z)
    {
        if (x>MaxEdge.X) MaxEdge.X = x;
        if (y>MaxEdge.Y) MaxEdge.Y = y;
        if (z>MaxEdge.Z) MaxEdge.Z = z;

        if (x<MinEdge.X) MinEdge.X = x;
        if (y<MinEdge.Y) MinEdge.Y = y;
        if (z<MinEdge.Z) MinEdge.Z = z;
    }

    //------------------------------------------------------------------------

    //! Determinates if a point is within this box.
    //! \param p: Point to check.
    //! \return Returns true if the point is withing the box, and false if it is not.
    inline bool isPointInside(const vector3d<T>& p) const
    {
        return (   
			(p.X>=MinEdge.X || math::Equals(MinEdge.X, p.X)) && 
			(p.X<=MaxEdge.X || math::Equals(MaxEdge.X, p.X)) &&
            (p.Y>=MinEdge.Y || math::Equals(MinEdge.Y, p.Y)) && 
			(p.Y<=MaxEdge.Y || math::Equals(MaxEdge.Y, p.Y)) &&
            (p.Z>=MinEdge.Z || math::Equals(MinEdge.Z, p.Z)) && 
			(p.Z<=MaxEdge.Z || math::Equals(MaxEdge.Z, p.Z))
			);
    };

    //------------------------------------------------------------------------

    //! Determinates if a point is within this box and its borders.
    //! \param p: Point to check.
    //! \return Returns true if the point is withing the box, and false if it is not.
    inline bool isPointTotalInside(const vector3d<T>& p) const
    {
        return (    
			p.X > MinEdge.X && p.X < MaxEdge.X &&
            p.Y > MinEdge.Y && p.Y < MaxEdge.Y &&
            p.Z > MinEdge.Z && p.Z < MaxEdge.Z
			);
    };

    //------------------------------------------------------------------------

    //! Determinates if the box intersects with an other box.
    //! \param other: Other box to check a intersection with.
    //! \return Returns true if there is a intersection with the other box, 
    //! otherwise false.
    inline bool intersectsWithBox(const aabbox3d<T>& other) const
    {
        return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
    }

    //------------------------------------------------------------------------

    //! Tests if the box intersects with a line
    //! \param line: Line to test intersection with.
    //! \return Returns true if there is an intersection and false if not.
    inline bool intersectsWithLine(const line3d<T>& line) const
    {
        return intersectsWithLine(line.getMiddle(), line.getVector().normalize(), 
                (T)(line.getLength() * 0.5));
    }

    //------------------------------------------------------------------------

    //! Tests if the box intersects with a line
    //! \return Returns true if there is an intersection and false if not.
    inline bool intersectsWithLine(
		const vector3d<T>& linemiddle, const vector3d<T>& linevect, T halflength
		) const
    {
        const vector3d<T> e = (MaxEdge - MinEdge) * (T)0.5;
        const vector3d<T> t = (MinEdge + e) - linemiddle;
        float r;

        if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) || 
            (fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
            (fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)) )
            return false;

        r = e.Y * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.Y);
        if (fabs(t.Y*linevect.Z - t.Z*linevect.Y) > r )
            return false;

        r = e.X * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.X);
        if (fabs(t.Z*linevect.X - t.X*linevect.Z) > r )
            return false;

        r = e.X * (T)fabs(linevect.Y) + e.Y * (T)fabs(linevect.X);
        if (fabs(t.X*linevect.Y - t.Y*linevect.X) > r)
            return false;

        return true;
    }

    //------------------------------------------------------------------------

    //! Classifies a relation with a plane.
    //! \param plane: Plane to classify relation to.
    //! \return Returns EIREL3D_FRONT if the box is in front of the plane,
    //! EIREL3D_BEHIND if the box is back of the plane, and
    //! EIREL3D_PLANAR if is on both sides of the plane.
    inline E_INTERSECTION_RELATION_3D classifyPlaneRelation(const plane3d<f32>& plane) const
    {
        vector3d<T> nearPoint(MaxEdge);
        vector3d<T> farPoint(MinEdge);

        if (plane.Normal.X > (T)0)
        {
            nearPoint.X = MinEdge.X;
            farPoint.X = MaxEdge.X;
        }

        if (plane.Normal.Y > (T)0)
        {
            nearPoint.Y = MinEdge.Y;
            farPoint.Y = MaxEdge.Y;
        }

        if (plane.Normal.Z > (T)0)
        {
            nearPoint.Z = MinEdge.Z;
            farPoint.Z = MaxEdge.Z;
        }

        if (plane.Normal.getDotProduct(nearPoint) + plane.D > (T)0)
            return EIREL3D_FRONT;

        if (plane.Normal.getDotProduct(farPoint) + plane.D > (T)0)
            return EIREL3D_PLANAR;

        return EIREL3D_BEHIND;
    }

    //------------------------------------------------------------------------

    //! returns center of the bounding box
    inline vector3d<T> getCenter() const
    {
        return (MinEdge + MaxEdge) / 2;
    }

    //------------------------------------------------------------------------

    //! returns extend of the box
    inline vector3d<T> getExtend() const
    {
        return MaxEdge - MinEdge;
    }

    //------------------------------------------------------------------------

    //! stores all 8 edges of the box into a array
    //! \param edges: Pointer to array of 8 edges
    inline void getEdges(vector3d<T> edges[8]) const
    {
        core::vector3df middle = (MinEdge + MaxEdge) / 2;
        core::vector3df diag = middle - MaxEdge;
        //
        // Edges are stored in this way:
        // Hey, am I an ascii artist, or what? :) niko.
        //       /1--------/5
        //      /  |      / |
        //     /   |     /  |
        //     3---------7  |
        //     |   0- - -| -4
        //     |  /      |  /
        //     |/        | /
        //     2---------6/ 
        //
        edges[0].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
        edges[1].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
        edges[2].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
        edges[3].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
        edges[4].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
        edges[5].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
        edges[6].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
        edges[7].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
    }

	//------------------------------------------------------------------------

    //! stores all 6 planes of the box into a array
    //! \param edges: Pointer to array of 6 planes
    inline void getPlanes(plane3d<T> planes[6]) const
    {
		vector3d<T> edges[8];

		getEdges(edges);

		planes[0].set(edges[0], edges[2], edges[3]);
		planes[1].set(edges[2], edges[6], edges[7]);
		planes[2].set(edges[6], edges[4], edges[5]);
		planes[3].set(edges[4], edges[0], edges[1]);
		planes[4].set(edges[4], edges[6], edges[2]);
		planes[5].set(edges[1], edges[3], edges[7]);
    }

	//------------------------------------------------------------------------

    //! stores all 4 side planes of the box into a array
    //! \param edges: Pointer to array of 6 planes
    inline void getSidePlanes(plane3d<T> planes[4]) const
    {
		vector3d<T> edges[8];

		getEdges(edges);

		planes[0].set(edges[0], edges[2], edges[3]);
		planes[1].set(edges[2], edges[6], edges[7]);
		planes[2].set(edges[6], edges[4], edges[5]);
		planes[3].set(edges[4], edges[0], edges[1]);
    }

	//------------------------------------------------------------------------

    //! return top box plane
    inline plane3d<T> getTopPlane() const
    {
		vector3d<T> edges[8];

		getEdges(edges);

		return plane3d<T>(edges[1], edges[3], edges[7]);
    }

	//------------------------------------------------------------------------

    //! return bottom box plane
    inline plane3d<T> getBottomPlane() const
    {
		vector3d<T> edges[8];

		getEdges(edges);

		return plane3d<T>(edges[4], edges[6], edges[2]);
    }

    //------------------------------------------------------------------------

    //! returns if the box is empty, which means that there is
    //! no space within the min and the max edge.
    inline bool isEmpty() const
    {
        vector3df d = MinEdge - MaxEdge;
        if (d.X < 0) d.X = -d.X;
        if (d.Y < 0) d.Y = -d.Y;
        if (d.Z < 0) d.Z = -d.Z;

        return (d.X < ROUNDING_ERROR_32 && 
                d.Y < ROUNDING_ERROR_32 && 
                d.Z < ROUNDING_ERROR_32);
    }

    //------------------------------------------------------------------------

    //! repairs the box, if for example MinEdge and MaxEdge are swapped.
    inline void repair()
    {
        T t;

        if (MinEdge.X > MaxEdge.X)
            { t=MinEdge.X; MinEdge.X = MaxEdge.X; MaxEdge.X=t; }
        if (MinEdge.Y > MaxEdge.Y)
            { t=MinEdge.Y; MinEdge.Y = MaxEdge.Y; MaxEdge.Y=t; }
        if (MinEdge.Z > MaxEdge.Z)
            { t=MinEdge.Z; MinEdge.Z = MaxEdge.Z; MaxEdge.Z=t; }
    }

    //------------------------------------------------------------------------

    //! Calculates a new interpolated bounding box.
    //! \param other: other box to interpolate between
    //! \param d: value between 0.0f and 1.0f.
    inline aabbox3d<T> getInterpolated(const aabbox3d<T>& other, f32 d) const
    {
        f32 inv = 1.0f - d;
        return aabbox3d<T>((other.MinEdge*inv) + (MinEdge*d),
            (other.MaxEdge*inv) + (MaxEdge*d));
    }

	//------------------------------------------------------------------------

	//! Tests intersection with triangle
	inline bool intersectsWithTriangle(const triangle3d<T> &triangle) const
	{
		return intersectsWithTriangle(triangle.A, triangle.B, triangle.C);
	}

	//------------------------------------------------------------------------

	//! Tests intersection with triangle
	inline bool intersectsWithTriangle(
		const vector3d<T> &A, const vector3d<T> &B, const vector3d<T> &C
		) const
	{
		// fast checks

		// fast check 1
		if ((A.X>MaxEdge.X && B.X>MaxEdge.X && C.X>MaxEdge.X) ||
			(A.Y>MaxEdge.Y && B.Y>MaxEdge.Y && C.Y>MaxEdge.Y) ||
			(A.Z>MaxEdge.Z && B.Z>MaxEdge.Z && C.X>MaxEdge.Z) ||
			(A.X<MinEdge.X && B.X<MinEdge.X && C.X<MinEdge.X) ||
			(A.Y<MinEdge.Y && B.Y<MinEdge.Y && C.Y<MinEdge.Y) ||
			(A.Z<MinEdge.Z && B.Z<MinEdge.Z && C.X<MinEdge.Z)			
			)
		{
			return false;
		}		

		// fast check 2
		if (isPointInside(A) || isPointInside(B) || isPointInside(C))
		{
			return true;
		}		

		// testing intersection of box edges with triangle

		triangle3d<T> triangle(A, B, C);			

		core::vector3d<T> edges[8];
		getEdges(edges);

		core::vector3d<T> *edge_lines[12][2] = 
		{
			{ &edges[5], &edges[1] },
			{ &edges[1], &edges[3] },
			{ &edges[3], &edges[7] },
			{ &edges[7], &edges[5] },
			{ &edges[0], &edges[2] },
			{ &edges[2], &edges[6] },
			{ &edges[6], &edges[4] },
			{ &edges[4], &edges[0] },
			{ &edges[1], &edges[0] },
			{ &edges[3], &edges[2] },
			{ &edges[7], &edges[6] },
			{ &edges[5], &edges[4] }
		};

		s32 i=0;

		for (i=0; i<12; i++)
		{
			core::vector3d<T> intersection;

			if (triangle.getIntersectionWithLimitedLine(
					*edge_lines[i][0], *edge_lines[i][1], intersection
					)
				)
			{
				return true;
			}
		}

		// testing intersection triangle edges with the box

		triangle3df edge_tris[12] = 
		{
			triangle3df(edges[0], edges[2], edges[6]),
			triangle3df(edges[0], edges[6], edges[4]),
			triangle3df(edges[7], edges[3], edges[1]),
			triangle3df(edges[7], edges[1], edges[5]),
			triangle3df(edges[4], edges[6], edges[7]),
			triangle3df(edges[4], edges[7], edges[5]),
			triangle3df(edges[0], edges[1], edges[3]),
			triangle3df(edges[0], edges[3], edges[2]),
			triangle3df(edges[6], edges[2], edges[3]),
			triangle3df(edges[6], edges[3], edges[7]),
			triangle3df(edges[0], edges[4], edges[5]),
			triangle3df(edges[0], edges[5], edges[1])
			};
			
		for (i=0; i<12; i+=2)
		{
			core::vector3d<T> intersection;
			line3d<T> testline;

			for (s32 j=0; j<3; j++)
			{
				if (j==0)
					testline = line3d<T>(A, B);	
				else if (j==1)
					testline = line3d<T>(B, C);
				else
					testline = line3d<T>(C, A);

				if (edge_tris[i].getIntersectionWithLimitedLine(testline, intersection)|| (
						testline.isPointBetweenStartAndEnd(intersection) &&	(
							edge_tris[i].closestPointOnTriangle(intersection)-intersection
							).getLength()<ROUNDING_ERROR_32
						)
					)
				{
					return true;
				}
				else if (
					edge_tris[i+1].getIntersectionWithLimitedLine(testline, intersection)|| (
						testline.isPointBetweenStartAndEnd(intersection) &&	(
							edge_tris[i+1].closestPointOnTriangle(intersection)-intersection
							).getLength()<ROUNDING_ERROR_32
						)
					)
				{
					return true;
				}
			}
		}

		return false;
	}

	//------------------------------------------------------------------------

    inline bool isValide() const
    {
        return 
			(MaxEdge.X-MinEdge.X)>ROUNDING_ERROR_32 &&
			(MaxEdge.Y-MinEdge.Y)>ROUNDING_ERROR_32 &&
			(MaxEdge.Z-MinEdge.Z)>ROUNDING_ERROR_32;
    }

	//------------------------------------------------------------------------

	//! Getting occlusion planes, creating from this box as from occluder
	//! \param look: look position or look vector (depends on parameter value orthogonal=true/false) 
    //! \param planes: planes array to put calculated occlusion planes into
	//! \param orthogonal: if camera has perspective view, it parameter must equals False (otherwise True)

    inline void getOcclusionPlanes(
		const core::vector3df &look, core::array< plane3d<T> >& planes, bool orthogonal=false
		) const
	{		
		static core::vector3d<T> edges[8];
		getEdges(edges);

        // Edges are stored in this way:
        //       /1--------/5
        //      /  |      / |
        //     /   |     /  |
        //     3---------7  |
        //     |   0- - -| -4
        //     |  /      |  /
        //     |/        | /
        //     2---------6/ 

		core::vector3df ldir0(look), ldir7(look);

		if (!orthogonal)
		{
			ldir0.set(edges[0]-look);
			ldir7.set(edges[7]-look);
		}

		core::triangle3df side_tris[6] = 
		{
			core::triangle3df( edges[0], edges[2], edges[3] ),
			core::triangle3df( edges[2], edges[6], edges[7] ),
			core::triangle3df( edges[6], edges[4], edges[5] ),
			core::triangle3df( edges[4], edges[0], edges[1] ),
			core::triangle3df( edges[4], edges[6], edges[2] ),
			core::triangle3df( edges[1], edges[3], edges[7] )
		};

		bool front_plane[6] =
		{
			side_tris[0].isFrontFacing(ldir0),
			side_tris[1].isFrontFacing(ldir7),
			side_tris[2].isFrontFacing(ldir7),
			side_tris[3].isFrontFacing(ldir0),
			side_tris[4].isFrontFacing(ldir0),
			side_tris[5].isFrontFacing(ldir7) 
		};		
		
		static s32 linesadj[6][4][2] = 
		{
			{ {0, 2}, {2, 3}, {3, 1}, {1, 0} },
			{ {2, 6}, {6, 7}, {7, 3}, {3, 2} },
			{ {6, 4}, {4, 5}, {5, 7}, {7, 6} },
			{ {4, 0}, {0, 1}, {1, 5}, {5, 4} },
			{ {0, 4}, {4, 6}, {6, 2}, {2, 0} },
			{ {3, 7}, {7, 5}, {5, 1}, {1, 3} }
		};	 

		static s32 planeadj[6][4] = 
		{
			{ 4, 1, 5, 3 },
			{ 4, 2, 5, 0 },
			{ 4, 3, 5, 1 },
			{ 4, 0, 5, 2 },
			{ 3, 2, 1, 0 },
			{ 1, 2, 3, 0 }
		};
		
		planes.set_used(0);

		if (!orthogonal)
		{
			for (s32 p=0; p<6; p++)
			{		
				if (front_plane[p])
				{		
					planes.push_back( 
						core::plane3d<T>(
							side_tris[p].A, side_tris[p].B, side_tris[p].C
							) 
						);
					for (s32 ap=0; ap<4; ap++)
					{
						s32 p_ap = planeadj[p][ap];

						if (!front_plane[p_ap])
						{						
							planes.push_back( 						
								core::plane3d<T>(
									look,
									edges[ linesadj[p][ap][0] ],
									edges[ linesadj[p][ap][1] ]
									)									
								);	
						}
					}
				}
			}
		}
		else
		{
			for (s32 p=0; p<6; p++)
			{		
				if (front_plane[p])
				{		
					planes.push_back( 
						core::plane3d<T>(
							side_tris[p].A, side_tris[p].B, side_tris[p].C
							) 
						);
					for (s32 ap=0; ap<4; ap++)
					{
						s32 p_ap = planeadj[p][ap];

						if (!front_plane[p_ap])
						{						
							planes.push_back( 						
								core::plane3d<T>(
									edges[ linesadj[p][ap][0] ]-look,
									edges[ linesadj[p][ap][0] ],
									edges[ linesadj[p][ap][1] ]
									)									
								);	
						}
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------

	//! Getting visibility contour, creating from this box as from occluder
	//! \param look: look position or look vector (depends on parameter value orthogonal=true/false) 
    //! \param planes: planes array to put calculated occlusion planes into
	//! \param orthogonal: if camera has perspective view, it parameter must equals False (otherwise True)

	template <class TContour>
    inline void getVisibilityContour(
		const core::vector3df &look, core::array <TContour>& contour, bool orthogonal=false
		) const
	{
		static core::vector3d<T> edges[8];
		getEdges(edges);

		core::vector3df ldir0(look), ldir7(look);

		if (!orthogonal)
		{
			ldir0.set(edges[0]-look);
			ldir7.set(edges[7]-look);
		}

		bool front_plane[6] =
		{
			core::triangle3df( edges[0], edges[2], edges[3] ).isFrontFacing(ldir0),
			core::triangle3df( edges[2], edges[6], edges[7] ).isFrontFacing(ldir7),
			core::triangle3df( edges[6], edges[4], edges[5] ).isFrontFacing(ldir7),
			core::triangle3df( edges[4], edges[0], edges[1] ).isFrontFacing(ldir0),
			core::triangle3df( edges[4], edges[6], edges[2] ).isFrontFacing(ldir0),
			core::triangle3df( edges[1], edges[3], edges[7] ).isFrontFacing(ldir7) 
		};		
		
		static s32 linesadj[6][4][2] = 
		{
			{ {0, 2}, {2, 3}, {3, 1}, {1, 0} },
			{ {2, 6}, {6, 7}, {7, 3}, {3, 2} },
			{ {6, 4}, {4, 5}, {5, 7}, {7, 6} },
			{ {4, 0}, {0, 1}, {1, 5}, {5, 4} },
			{ {0, 4}, {4, 6}, {6, 2}, {2, 0} },
			{ {3, 7}, {7, 5}, {5, 1}, {1, 3} }
		};	 

		static s32 planeadj[6][4] = 
		{
			{ 4, 1, 5, 3 },
			{ 4, 2, 5, 0 },
			{ 4, 3, 5, 1 },
			{ 4, 0, 5, 2 },
			{ 3, 2, 1, 0 },
			{ 1, 2, 3, 0 }
		};
		
		static s32 edge_lines[24][2];
		s32 edge_count = 0;
		
		for (s32 p=0; p<6; p++)
		{			
			for (s32 ap=0; ap<4; ap++)
			{
				s32 p_ap = planeadj[p][ap];

				if (front_plane[p] && !front_plane[p_ap])
				{
					edge_lines[edge_count][0] = linesadj[p][ap][0];
					edge_lines[edge_count][1] = linesadj[p][ap][1];

					edge_count++;
				}
			}
		}

		contour.set_used(0);

		s32 next = -1;

		if (edge_count>0)
		{
			contour.push_back( edges[ edge_lines[0][0] ] );
			contour.push_back( edges[ edge_lines[0][1] ] );

			next = edge_lines[0][1];

			edge_lines[0][0] = -1;
		}

		for (s32 e=1; e<edge_count;)
		{
			if (edge_lines[e][0]==-1)
			{
				e++;
				continue;		
			}

			for (s32 ee=e; ee<edge_count; ee++)
			{
				if (edge_lines[ee][0]==-1)
					continue;

				if (edge_lines[ee][0] == next)
				{
					contour.push_back( edges[ edge_lines[ee][1] ] );

					next = edge_lines[ee][1];

					edge_lines[ee][0] = -1;

					break;
				}
			}		
		}
	}

	void set(const core::aabbox3d<T> &b)
	{
		MinEdge = b.MinEdge;
		MaxEdge = b.MaxEdge;
	}

    //------------------------------------------------------------------------

    // member variables    
    vector3d<T> MinEdge;
    vector3d<T> MaxEdge;
};

//----------------------------------------------------------------------------

//! Typedef for a f32 3d bounding box.
typedef aabbox3d<f32> aabbox3df;

//! Typedef for an integer 3d bounding box.
typedef aabbox3d<s32> aabbox3di;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif


