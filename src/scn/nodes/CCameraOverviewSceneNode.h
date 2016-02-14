//|-------------------------------------------------------------------------
//| File:        CCameraOverviewSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCameraOverviewSceneNodeHPP
#define CCameraOverviewSceneNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <io/ICursorControl.h>
#include <io/SKeyMap.h>
#include "CCameraSceneNode.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CCameraOverviewController : public ICameraController
{
public:
	CCameraOverviewController()
		: m_FirstUpdate(true) {}
	virtual void animate(ICameraSceneNode *camera, u32 timeMs);
	virtual void reset() { m_FirstUpdate = true; }
private:
	bool m_FirstUpdate;
};

//---------------------------------------------------------------------------

class CCameraOverviewSceneNode : public CCameraSceneNode
{
public:
    CCameraOverviewSceneNode(ISceneNode* parent, s32 id, 
		f32 rotateSpeed, f32 zoomSpeed);
    virtual ~CCameraOverviewSceneNode();
private:
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

