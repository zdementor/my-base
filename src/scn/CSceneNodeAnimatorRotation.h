//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorRotation.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSceneNodeAnimatorRotationHPP
#define CSceneNodeAnimatorRotationHPP
//----------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CSceneNodeAnimatorRotation : public ISceneNodeAnimator
{
public:

    //! constructor
    CSceneNodeAnimatorRotation(u32 time, const core::vector3df& rotation);

    //! destructor
    virtual ~CSceneNodeAnimatorRotation();

    //! animates a scene node
    virtual void animateNode(ISceneNode* node, u32 timeMs);

	//! return scene node animator type
	virtual E_SCENE_NODE_ANIMATOR_TYPE getAnimatorType()
	{
		return ESNAT_ROTATION_ANIMATOR;
	}

	const core::vector3df& getRotationPerSec()
	{
		return Rotation;
	}

private:

    core::vector3df Rotation;
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------

#endif

