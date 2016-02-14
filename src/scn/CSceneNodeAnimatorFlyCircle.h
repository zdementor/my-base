//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFlyCircle.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneNodeAnimatorFlyCircleHPP
#define CSceneNodeAnimatorFlyCircleHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SFlyCircleParameters
{
	core::vector3df Center;
    f32 Radius;
    f32 Speed;    
};

//---------------------------------------------------------------------------
class CSceneNodeAnimatorFlyCircle : public ISceneNodeAnimator
{
public:

    //! constructor
    CSceneNodeAnimatorFlyCircle(u32 start_time, SFlyCircleParameters &params);

    //! destructor
    virtual ~CSceneNodeAnimatorFlyCircle();

    //! animates a scene node
    virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! return scene node animator type
	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{
		return ESNAT_FLY_CIRCLE_ANIMATOR;
	}

	SFlyCircleParameters getFlyCircleParameters()
	{
		return FlyCircleParameters;
	}

private:

	SFlyCircleParameters FlyCircleParameters;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

