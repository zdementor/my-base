//|-------------------------------------------------------------------------
//| File:        CCameraOverviewSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CCameraOverviewSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>
#include <dev/IDevice.h>
#include <io/IInputDispatcher.h>
#include <os/ITimer.h>
#include <io/ILogger.h>
#include <io/ICursorControl.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CCameraOverviewSceneNode::CCameraOverviewSceneNode(
	ISceneNode* parent, s32 id, f32 rotateSpeed, f32 zoomSpeed
	) : CCameraSceneNode(parent, id, new CCameraOverviewController)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CCameraOverviewSceneNode");
#endif

	setRotationSpeed(rotateSpeed);		
	setZoomSpeed(zoomSpeed);

    recalculateViewArea();
}

//---------------------------------------------------------------------------

CCameraOverviewSceneNode::~CCameraOverviewSceneNode()
{
}

//---------------------------------------------------------------------------

void CCameraOverviewController::animate(ICameraSceneNode *camera, u32 timeMs)
{
	ICameraController::animate(camera, timeMs);

	scn::ISceneManager   &smgr = SCENE_MANAGER;
	io::IInputDispatcher &inpd = INPUT_DISPATCHER;
	io::ICursorControl   &cursor = CURSOR_CONTROL;

	core::vector3df tvectZ = m_Tgt - m_Pos;
	tvectZ.normalize();
	tvectZ *= 100;

	f32 min_bound = 0.25f, max_bound = 0.75f;

	f32 rot_speed = camera->getRotationSpeed();

	// get time

    s32 timeDiff = getTimeDiff();

	// update Rotation

	core::position2d<f32> cursorpos = cursor.getRelativePosition();
      
    f32 rotX = 0.0f, rotY = 0.0f;

    if (m_FirstUpdate)
    { 
		cursor.setRelativePosition(0.5f, 0.5f); 
    }     

	if (cursorpos.X <= min_bound) 
    { 		
		rotX = min_bound - cursorpos.X;
		cursor.setRelativePosition(min_bound, cursorpos.Y); 
	} 
	else
	if (cursorpos.X >= max_bound) 
    { 		
		rotX = max_bound - cursorpos.X;
		cursor.setRelativePosition(max_bound, cursorpos.Y); 
	} 
	else
		rotX = 0.0f;

	cursorpos = cursor.getRelativePosition();

	if (cursorpos.Y <= min_bound) 
    { 		
		rotY = min_bound - cursorpos.Y;
		cursor.setRelativePosition(cursorpos.X, min_bound); 
	} 
	else
	if (cursorpos.Y >= max_bound) 
    { 		
		rotY = max_bound - cursorpos.Y;
		cursor.setRelativePosition(cursorpos.X, max_bound); 
	} 
	else
		rotY = 0.0f;

	rotY *= rot_speed;
	rotX *= rot_speed;

	// set target

	m_Tgt = m_Pos + tvectZ;
	m_Tgt.rotateXZByDegrees(rotX, m_Pos);

	m_Tgt.Y += rotY;
	core::vector3df vec = m_Tgt - m_Pos;
	vec.normalize();
	vec *= 100;
	m_Tgt = m_Pos + vec;

    // write right target

	camera->setPosition(m_Pos);
	camera->setTarget(m_Tgt);

	m_FirstUpdate = false;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------


