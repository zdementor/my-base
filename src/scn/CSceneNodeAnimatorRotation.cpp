//|-------------------------------------------------------------------------
//| File:        CSceneNodeAnimatorRotation.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CSceneNodeAnimatorRotation.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(
	u32 time, const core::vector3df& rotation
	) 
: ISceneNodeAnimator(time), Rotation(rotation)
{
#if MY_DEBUG_MODE 
	setClassName("CSceneNodeAnimatorRotation");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CSceneNodeAnimatorRotation::~CSceneNodeAnimatorRotation()
{
}

//----------------------------------------------------------------------------

//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!IsEnabled || !node)
		return;    
	
	node->setRotation(Rotation* ((timeMs-StartTime)/10.0f));
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
