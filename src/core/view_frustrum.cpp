//|-------------------------------------------------------------------------
//| File:        view_frustrum.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace core { 
//---------------------------------------------------------------------------

//! This constructor creates a view frustrum based on a projection and view matrix.
inline view_frustrum::view_frustrum(const core::matrix4& proj, const core::matrix4& view)
: IsOrthogonalProjection(false)
{
	set(proj, view);
}

//---------------------------------------------------------------------------

//! transforms the furstum by the matrix
//! \param Matrix by which the view frustrum is transformed.
inline void view_frustrum::transform(const core::matrix4 &mat)
{
    for (int i=0; i<VF_PLANE_COUNT; ++i)
        mat.transformPlane(Planes[i]);

    mat.transformVect(Position);

    recalculateBoundingBox();
}

//---------------------------------------------------------------------------

//! returns the point which is on the far left upper corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getFarLeftUp() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_TOP_PLANE],
        Planes[view_frustrum::VF_LEFT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns the point which is on the far left bottom corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getFarLeftDown() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_BOTTOM_PLANE],
        Planes[view_frustrum::VF_LEFT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns the point which is on the far right top corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getFarRightUp() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_TOP_PLANE],
        Planes[view_frustrum::VF_RIGHT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns the point which is on the far right bottom corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getFarRightDown() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_FAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_BOTTOM_PLANE],
        Planes[view_frustrum::VF_RIGHT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns the point which is on the near left bottom corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getNearLeftDown() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_NEAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_BOTTOM_PLANE],
        Planes[view_frustrum::VF_LEFT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns the point which is on the near left upper corner inside the
//! the view frustrum.
inline core::vector3df view_frustrum::getNearLeftUp() const
{
    core::vector3df p;
    Planes[view_frustrum::VF_NEAR_PLANE].getIntersectionWithPlanes(
        Planes[view_frustrum::VF_TOP_PLANE],
        Planes[view_frustrum::VF_LEFT_PLANE], p
        );

    return p;
}

//---------------------------------------------------------------------------

//! returns a bounding box enclosing the whole view frustrum
inline core::aabbox3d<f32> view_frustrum::getBoundingBox() const
{
    return BoundingBox;
}

//---------------------------------------------------------------------------    

//! recalculates the bounding box member based on the Planes
inline void view_frustrum::recalculateBoundingBox()
{
    core::aabbox3d<f32> box(Position);

    box.addInternalPoint(getFarLeftUp());
    box.addInternalPoint(getFarRightUp());
    box.addInternalPoint(getFarLeftDown());
    box.addInternalPoint(getFarRightDown());

    BoundingBox = box;
}

//---------------------------------------------------------------------------

//! Tests if a aabox is within the frustrum. 
inline E_INTERSECTION_RELATION_3D view_frustrum::classifyBoxRelation( const core::aabbox3d<f32>& aabox ) const 
{ 
	// Get edges 
    static core::vector3d<f32> edges[8]; 
    aabox.getEdges(edges);         // let's generate the edge points 

	for (u32 p=0; p<VF_PLANE_COUNT; ++p)
	{
		bool boxInFrustum=false;

		for (u32 e=0; e<8; ++e)
		{
			if (Planes[p].classifyPointRelation(edges[e]) != EIREL3D_FRONT)
			{
				boxInFrustum=true;
				break;
			}
		}

		if (!boxInFrustum)
			return EIREL3D_FRONT;
	}

	return EIREL3D_BEHIND;
} 

//---------------------------------------------------------------------------

//! Tests if a sphere is within the frustrum. 
inline E_INTERSECTION_RELATION_3D view_frustrum::classifySphereRelation( const core::vector3df& center, f32 radius ) const
{
	u32 planar = 0;

	for( u32 p=0; p<VF_PLANE_COUNT; ++p ) 
    {
		E_INTERSECTION_RELATION_3D rel = Planes[p].classifySphereRelation(center, radius);

		// check if sphere is front of the plane 
		if( rel == core::EIREL3D_FRONT ) 
			return EIREL3D_FRONT;
		else 
		// sphere intersects with a plane 
		if( rel == core::EIREL3D_PLANAR )         
			planar++;
	}

	if (planar)
		return EIREL3D_PLANAR;
	
	// Whole sphere is inside the frustum
	return EIREL3D_BEHIND;
}

//---------------------------------------------------------------------------

//! Tests if a point is within the frustrum. 
inline E_INTERSECTION_RELATION_3D view_frustrum::classifyPointRelation( const core::vector3df& point ) const
{ 
	for( u32 p=0; p<6; ++p ) 
    { 
        // check if current point is front of the plane 
        if( Planes[p].classifyPointRelation(point) == EIREL3D_FRONT ) 
			return EIREL3D_FRONT;
	}

	return EIREL3D_BEHIND;
} 

//---------------------------------------------------------------------------
	 
//! set a perspective projection based on a field of view    
inline void view_frustrum::setProjectionPerspective(
	f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar
	)
{
	static core::matrix4 proj;

	proj.buildProjectionMatrixPerspectiveLH(fieldOfViewRadians, aspectRatio, zNear, zFar);

	set(proj, View);
}	

//---------------------------------------------------------------------------

//! set a orthogonal projection 
inline void view_frustrum::setProjectionOrtho(
	f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar
	)
{
	static core::matrix4 proj;	
	
	proj.buildProjectionMatrixOrthoLH(widthOfViewVolume, heightOfViewVolume, zNear, zFar);

	set(proj, View);

	IsOrthogonalProjection = true;
}

//---------------------------------------------------------------------------

//! sets a look-at 
inline void view_frustrum::setView(
	const vector3df& position, const vector3df& target, const vector3df& upVector
	)
{
	core::matrix4 view;

	view.buildViewMatrixLH(position, target, upVector);

	Position = position;

	setView(view);

	recalculateBoundingBox();
}

//---------------------------------------------------------------------------

//! sets a view 
inline void view_frustrum::setView(const core::matrix4& view)
{
	bool ortho = IsOrthogonalProjection;

	set(Projection, view);

	IsOrthogonalProjection = ortho;
}

//---------------------------------------------------------------------------

//! setting a view frustrum based on a projection and view matrix.
inline void view_frustrum::set(const core::matrix4& proj, const core::matrix4& view)
{
	Projection = proj;
	View = view;

	core::matrix4 mat = Projection*View;

	#define sw(a,b)     (mat((b),(a)))

    // left clipping plane
    Planes[view_frustrum::VF_LEFT_PLANE].Normal.X = -(sw(0,3) + sw(0,0));
    Planes[view_frustrum::VF_LEFT_PLANE].Normal.Y = -(sw(1,3) + sw(1,0));
    Planes[view_frustrum::VF_LEFT_PLANE].Normal.Z = -(sw(2,3) + sw(2,0));
    Planes[view_frustrum::VF_LEFT_PLANE].D =        -(sw(3,3) + sw(3,0));
                                                     
    // right clipping plane                  
    Planes[view_frustrum::VF_RIGHT_PLANE].Normal.X = -(sw(0,3) - sw(0,0));
    Planes[view_frustrum::VF_RIGHT_PLANE].Normal.Y = -(sw(1,3) - sw(1,0));
    Planes[view_frustrum::VF_RIGHT_PLANE].Normal.Z = -(sw(2,3) - sw(2,0));
    Planes[view_frustrum::VF_RIGHT_PLANE].D =        -(sw(3,3) - sw(3,0));
        
    // top clipping plane                  
    Planes[view_frustrum::VF_TOP_PLANE].Normal.X = -(sw(0,3) - sw(0,1));
    Planes[view_frustrum::VF_TOP_PLANE].Normal.Y = -(sw(1,3) - sw(1,1));
    Planes[view_frustrum::VF_TOP_PLANE].Normal.Z = -(sw(2,3) - sw(2,1));
    Planes[view_frustrum::VF_TOP_PLANE].D =        -(sw(3,3) - sw(3,1));

    // bottom clipping plane                  
    Planes[view_frustrum::VF_BOTTOM_PLANE].Normal.X = -(sw(0,3) + sw(0,1));
    Planes[view_frustrum::VF_BOTTOM_PLANE].Normal.Y = -(sw(1,3) + sw(1,1));
    Planes[view_frustrum::VF_BOTTOM_PLANE].Normal.Z = -(sw(2,3) + sw(2,1));
    Planes[view_frustrum::VF_BOTTOM_PLANE].D =        -(sw(3,3) + sw(3,1));
     
    // near clipping plane                   
    Planes[view_frustrum::VF_NEAR_PLANE].Normal.X = -sw(0,2);
    Planes[view_frustrum::VF_NEAR_PLANE].Normal.Y = -sw(1,2);
    Planes[view_frustrum::VF_NEAR_PLANE].Normal.Z = -sw(2,2);
    Planes[view_frustrum::VF_NEAR_PLANE].D =        -sw(3,2);
                                                     
    // far clipping plane                    
    Planes[view_frustrum::VF_FAR_PLANE].Normal.X = -(sw(0,3) - sw(0,2));
    Planes[view_frustrum::VF_FAR_PLANE].Normal.Y = -(sw(1,3) - sw(1,2));
    Planes[view_frustrum::VF_FAR_PLANE].Normal.Z = -(sw(2,3) - sw(2,2));
    Planes[view_frustrum::VF_FAR_PLANE].D =        -(sw(3,3) - sw(3,2));

    // normalize normals
    double del=0;
    for (s32 i=0; i<6; ++i)
    {
        del = Planes[i].Normal.getLength();
        
        //çàùèòà îò äåëåíèÿ íà íîëü (ìàëî ëè ÷åãî :) )
        if (del==0) del+=0.000001f;
            
        f32 len = (f32)(1.0f / del);
        Planes[i].Normal *= len;
        Planes[i].D *= len;
    }

    // make bounding box
    recalculateBoundingBox();

	IsOrthogonalProjection = false;
}

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------


