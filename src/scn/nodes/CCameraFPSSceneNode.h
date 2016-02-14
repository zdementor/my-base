//|-------------------------------------------------------------------------
//| File:        CCameraFPSSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCameraFPSSceneNodeHPP
#define CCameraFPSSceneNodeHPP
//---------------------------------------------------------------------------

#include "CCameraSceneNode.h"
#include <my_inc.h>
#include <io/ICursorControl.h>
#include <io/SKeyMap.h>
#include <game/IGameManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CCameraFPSController : public ICameraController
{
public:
	CCameraFPSController()
		: m_FirstUpdate(true) {}
	virtual void animate(ICameraSceneNode *camera, u32 timeMs);
	virtual void reset() { m_FirstUpdate = true; }
private:
    bool m_FirstUpdate;
};

//---------------------------------------------------------------------------

class CCameraFPSSceneNode : public CCameraSceneNode
{
public:
    CCameraFPSSceneNode(ISceneNode* player, s32 id,
		f32 rot_speed, f32 move_speed, f32 zoom_speed);
    virtual ~CCameraFPSSceneNode();
private:
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif