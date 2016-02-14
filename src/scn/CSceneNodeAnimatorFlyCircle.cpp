//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFlyCircle.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSceneNodeAnimatorFlyCircle.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CSceneNodeAnimatorFlyCircle::CSceneNodeAnimatorFlyCircle(
	u32 start_time, SFlyCircleParameters &params
	)
: ISceneNodeAnimator(start_time), FlyCircleParameters(params)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorFlyCircle");
#endif
}

//---------------------------------------------------------------------------

//! destructor
CSceneNodeAnimatorFlyCircle::~CSceneNodeAnimatorFlyCircle()
{
}

//---------------------------------------------------------------------------

//! animates a scene node
void CSceneNodeAnimatorFlyCircle::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!IsEnabled)
		return;

    core::matrix4 mat;

    f32 t = (timeMs - StartTime) * FlyCircleParameters.Speed;

    core::vector3df circle(
		FlyCircleParameters.Radius * (f32)sin(t), 
		0, 
		FlyCircleParameters.Radius * (f32)cos(t)
		);

    node->setPosition(FlyCircleParameters.Center + circle);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
