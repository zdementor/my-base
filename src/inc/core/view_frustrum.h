//|-------------------------------------------------------------------------
//| File:        view_frustrum.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef view_frustrumHPP
#define view_frustrumHPP

//---------------------------------------------------------------------------
namespace my {
namespace core { 
//---------------------------------------------------------------------------

//! Defines the view frustrum. Thats the space viewed by the camera.
//! The view frustrum is enclosed by 6 planes. These six planes share
//! four points. A bounding box around these four points is also stored in
//! this structure.
class __MY_CORE_LIB_API__ view_frustrum
{
public:

    enum VFPLANES
    {
        //! Far plane of the frustrum. Thats the plane farest away from the eye.
        VF_FAR_PLANE = 0,
        //! Near plane of the frustrum. Thats the plane nearest to the eye.
        VF_NEAR_PLANE,
        //! Left plane of the frustrum.
        VF_LEFT_PLANE,
        //! Right plane of the frustrum.
        VF_RIGHT_PLANE,
        //! Bottom plane of the frustrum.
        VF_BOTTOM_PLANE,
        //! Top plane of the frustrum.
        VF_TOP_PLANE,

        //! Amount of planes enclosing the view frustum. Should be 6.
        VF_PLANE_COUNT
    };

	//! Projection and view matrices
	core::matrix4 Projection, View;

    //! the position of the camera
    core::vector3df Position;

    //! all planes enclosing the view frustrum.
    core::plane3d<f32> Planes[VF_PLANE_COUNT];

    //! bouding box around the view frustrum
    core::aabbox3d<f32> BoundingBox;

    //! Default Constructor
	view_frustrum() : IsOrthogonalProjection(false) {}

    //! This constructor creates a view frustrum based on a projection and view matrix.
    view_frustrum(const core::matrix4& proj, const core::matrix4& view);

    //! transforms the furstum by the matrix
    //! \param Matrix by which the view frustrum is transformed.
    void transform(const core::matrix4 &mat);

    //! returns the point which is on the far left upper corner inside the
    //! the view frustrum.
    core::vector3df getFarLeftUp() const;

    //! returns the point which is on the far left bottom corner inside the
    //! the view frustrum.
    core::vector3df getFarLeftDown() const;

    //! returns the point which is on the far right top corner inside the
    //! the view frustrum.
    core::vector3df getFarRightUp() const;

    //! returns the point which is on the far right bottom corner inside the
    //! the view frustrum.
    core::vector3df getFarRightDown() const;

	//! returns the point which is on the near left bottom corner inside the
	//! the view frustrum.
	core::vector3df getNearLeftDown() const;

    //! returns the point which is on the near left upper corner inside the
    //! the view frustrum.
    core::vector3df getNearLeftUp() const;

    //! returns a bounding box enclosing the whole view frustrum
    core::aabbox3d<f32> getBoundingBox() const;

    //! recalculates the bounding box member based on the planes
    void recalculateBoundingBox();   
    
    //! Tests if a aabox is within the frustrum. 
    E_INTERSECTION_RELATION_3D classifyBoxRelation( const core::aabbox3d<f32>& aabox ) const;

	//! Tests if a sphere is within the frustrum. 
	E_INTERSECTION_RELATION_3D classifySphereRelation( const core::vector3df& center, f32 radius ) const;

    //! Tests if a point is within the frustrum. 
    E_INTERSECTION_RELATION_3D classifyPointRelation( const core::vector3df& point ) const;

	 //! set a perspective projection based on a field of view
    void setProjectionPerspective(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar);

	//! set a orthogonal projection
    void setProjectionOrtho(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar);

	//! sets a view 
    void setView(const vector3df& position, const vector3df& target, const vector3df& upVector);

	//! sets a view 
    void setView(const core::matrix4& view);

	//! setting a view frustrum based on a projection and view matrix.
	void set(const core::matrix4& proj, const core::matrix4& view);

	//! return true if view frustrum contains orthogonal projection matrix
	bool isOrthogonalProjection() const { return IsOrthogonalProjection; }

private:

	bool IsOrthogonalProjection;
};

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------

#endif

