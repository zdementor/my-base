//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFollowSpline.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneNodeAnimatorFollowSplineHPP
#define CSceneNodeAnimatorFollowSplineHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SFollowSplineParameters
{
	core::array< core::vector3df > Points;
    f32 Speed;
    f32 Tightness;
};

//---------------------------------------------------------------------------

//! Scene node animator based free code Matthias Gall wrote and sent in. (Most of 
//! this code is written by him, I only modified bits.)
class CSceneNodeAnimatorFollowSpline : public ISceneNodeAnimator
{
public:

    //! constructor
    CSceneNodeAnimatorFollowSpline(
		u32 startTime, const SFollowSplineParameters& params
		);

    //! destructor
    virtual ~CSceneNodeAnimatorFollowSpline();

    //! animates a scene node
    virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! return scene node animator type
	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{
		return ESNAT_FOLLOW_SPLINE_ANIMATOR;
	}

	SFollowSplineParameters getFollowSplineParameters()
	{
		return FollowSplineParameters;
	}

protected:	

	//! clamps a the value idx to fit into range 0..size-1
    s32 clamp(s32 idx, s32 size);

	SFollowSplineParameters FollowSplineParameters;
    
    u32 NumPoints, NumPoints_1;

	bool ok;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

