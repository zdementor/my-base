//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFlyStraight.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CSceneNodeAnimatorFlyStraight.h"
#include <os/ITimer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CSceneNodeAnimatorFlyStraight::CSceneNodeAnimatorFlyStraight(
	u32 start_time, SFlyStraightParameters &params
	) 
: ISceneNodeAnimator(start_time), FlyStraightParameters(params), FirstTime(true)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorFlyStraight");
#endif

    EndTime = StartTime + FlyStraightParameters.TimeForWay;

    Vector = FlyStraightParameters.End - FlyStraightParameters.Start;
    WayLength = (f32)Vector.getLength();
    Vector.normalize();

    TimeFactor = WayLength / FlyStraightParameters.TimeForWay;
}

//---------------------------------------------------------------------------

//! destructor
CSceneNodeAnimatorFlyStraight::~CSceneNodeAnimatorFlyStraight()
{
}

//---------------------------------------------------------------------------

//! animates a scene node
void CSceneNodeAnimatorFlyStraight::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!IsEnabled)
		return;

    if (!node)
        return;    

    core::vector3df pos = FlyStraightParameters.Start;

	if (FirstTime)
	{
		StartTime = TIMER.getTime();
		EndTime = StartTime + FlyStraightParameters.TimeForWay;

		Vector = FlyStraightParameters.End - FlyStraightParameters.Start;
		WayLength = (f32)Vector.getLength();
		Vector.normalize();

		TimeFactor = WayLength / FlyStraightParameters.TimeForWay;

		FirstTime = false;
	}
	else
	{
		u32 t = (timeMs-StartTime);

		if (!FlyStraightParameters.Loop && t >= FlyStraightParameters.TimeForWay)
			pos = FlyStraightParameters.End;
		else
			pos += Vector * (f32)fmod((f32)t, (f32)FlyStraightParameters.TimeForWay) * TimeFactor;
	}

    node->setPosition(pos);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
