//|-------------------------------------------------------------------------
//| File:        CCameraMayaSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CCameraMayaSceneNode.h"

#include <vid/IVideoDriver.h>
#include <scn/ISceneManager.h>  
#include <io/IInputDispatcher.h>  
#include <dev/IDevice.h>
#include <io/ILogger.h>
#include <os/ITimer.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! constructor
CCameraMayaSceneNode::CCameraMayaSceneNode(
	ISceneNode* parent, s32 id, f32 rs, f32 zs, f32 ts
	) : CCameraSceneNode(parent, id, new CCameraMayaController)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CCameraMayaSceneNode");
#endif

	setTarget(core::vector3df(0.0f, 0.0f, 0.0f));
	setZoomSpeed(zs);
    setRotationSpeed(rs);
    setMoveSpeed(ts);

    recalculateViewArea();
	recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

CCameraMayaSceneNode::~CCameraMayaSceneNode()
{
}

//---------------------------------------------------------------------------

void CCameraMayaController::animate(ICameraSceneNode *camera, u32 timeMs)
{
	ICameraController::animate(camera, timeMs);

	scn::ISceneManager   &smgr    = SCENE_MANAGER;
	vid::IVideoDriver    &driver  = VIDEO_DRIVER;

	core::dimension2d<s32> ssize = driver.getScreenSize();
	core::dimension2d<f32> ssizef(ssize.Width, ssize.Height);

	core::position2d<f32> CursorPos(
		getMousePosX() / ssizef.Width,
		getMousePosY() / ssizef.Height);

	core::vector2df MousePos(CursorPos.X, CursorPos.Y);

	const core::view_frustrum& va = camera->getViewFrustrum();

	core::vector3df tvectZ = m_Tgt - m_Pos;
	tvectZ.normalize();
	tvectZ *= 100;

	core::vector3df translate = m_Pos + tvectZ;

	f32 rotX = 0.0f;
	f32 rotY = 0.0f;
	f32 nZoom = currentZoom;

	f32 zoom_val	= -0.0005f * getTimeDiff() * camera->getZoomSpeed();
	f32 mov_speed	=  1.5f * getTimeDiff() * camera->getMoveSpeed();
	f32 rot_speed	= -1.5f * camera->getRotationSpeed();

	// Zooming

	if (getMouseState(io::EMC_LMOUSE) && 
		getMouseState(io::EMC_RMOUSE))
	{	
		if (!zooming)
		{
			zoomStartX = MousePos.X;
			zoomStartY = MousePos.Y;
			zooming = true;
			nZoom = currentZoom;
		}
		else
		{
			f32 old = nZoom;
			nZoom += (zoomStartY - MousePos.Y) * zoom_val;

			translate += tvectZ * (MousePos.Y - zoomStartY) * zoom_val;
			if (nZoom < 0)
				nZoom = old;
		}
	}
	else
	{
		if (zooming)
		{
			f32 old = currentZoom;
			currentZoom = currentZoom + (MousePos.Y - zoomStartY) * zoom_val;
			nZoom = currentZoom;

			translate += tvectZ * (zoomStartY - MousePos.Y) * zoom_val;
			if (nZoom < 0)
				nZoom = currentZoom = old;
		}
		zooming = false;
	}

	// Translation

	core::vector3df tvectX = m_Pos - m_Tgt;
	tvectX = tvectX.getCrossProduct(m_Up);
	tvectX.normalize();

	core::vector3df tvectY = (va.getFarLeftDown() - va.getFarRightDown());
	tvectY = tvectY.getCrossProduct(m_Up.Y > 0 ? m_Pos - m_Tgt : m_Tgt - m_Pos);
	tvectY.normalize();

	if (getMouseState(io::EMC_RMOUSE) && !zooming)
	{
		if (!translating)
		{
			translateStartX = MousePos.X;
			translateStartY = MousePos.Y;
			translating = true;
		}
		else
		{
			translate +=    
				tvectX * (translateStartX - MousePos.X) * mov_speed + 
				tvectY * (translateStartY - MousePos.Y) * mov_speed;
		}
	}
	else
	{
		if (translating)
		{
			translate +=    
				tvectX * (translateStartX - MousePos.X) * mov_speed +                                 
				tvectY * (translateStartY - MousePos.Y) * mov_speed;
		}
		translating = false;
	}

	// Rotation

	if (getMouseState(io::EMC_LMOUSE) && !zooming)
	{
		if (!rotating)
		{
			rotateStartX = MousePos.X;
			rotateStartY = MousePos.Y;
			rotating = true;
		}
		else
		{
			rotX = (rotateStartX - MousePos.X) * rot_speed;
			rotY = (rotateStartY - MousePos.Y) * rot_speed;
		}
	}
	else
	{
		rotating = false;
		rotX = rotY = 0.0f;
	}

	// set Target and Position
	m_Tgt = translate;
	{
		m_Pos = m_Tgt - tvectZ;
		m_Pos.rotateXZByDegrees(-rotX, m_Tgt);

		m_Pos.Y += rotY;
		core::vector3df vec = m_Pos - m_Tgt;
		if (vec.getLength() == 0.0f)
		{
			m_Pos = m_Tgt.Y + 0.001f;
			vec = m_Pos - m_Tgt;
		}
		vec.normalize();
		vec *= 100;
		m_Pos = m_Tgt + vec;		
	}

	rotateStartX = MousePos.X;
	rotateStartY = MousePos.Y;
	translateStartX = MousePos.X;
	translateStartY = MousePos.Y;

	camera->setTarget(m_Tgt);
	camera->setUpVector(m_Up);
    camera->setPosition(m_Pos);
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
