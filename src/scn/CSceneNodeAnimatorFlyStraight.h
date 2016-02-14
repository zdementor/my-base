//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorFlyStraight.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneNodeAnimatorFlyStraightHPP
#define CSceneNodeAnimatorFlyStraightHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SFlyStraightParameters
{
    core::vector3df Start;
    core::vector3df End;
	u32 TimeForWay;
    bool Loop;
};

//---------------------------------------------------------------------------

class CSceneNodeAnimatorFlyStraight : public ISceneNodeAnimator
{
public:

    //! constructor
    CSceneNodeAnimatorFlyStraight(
		u32 start_time, SFlyStraightParameters &params
		);

    //! destructor
    virtual ~CSceneNodeAnimatorFlyStraight();

    //! animates a scene node
    virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! return scene node animator type
	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{
		return ESNAT_FLY_STRAIGHT_ANIMATOR;
	}

	//! returning fly straight parameters
	SFlyStraightParameters getFlyStraightParameters()
	{
		return FlyStraightParameters;
	}

private:

	SFlyStraightParameters FlyStraightParameters;

    core::vector3df Vector;
    f32 WayLength;
    f32 TimeFactor;
    
    u32 EndTime;

	bool FirstTime;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

