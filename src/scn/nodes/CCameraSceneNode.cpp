//|-------------------------------------------------------------------------
//| File:        CCameraSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CCameraSceneNode.h"

#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>
#include <os/ITimer.h>
#include <dev/IDevice.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CCameraSceneNode::CCameraSceneNode(
	ISceneNode* parent, s32 id, ICameraController *controller,
    const core::vector3df& position, const core::vector3df& lookat
	) : ICameraSceneNode(parent, id),
	CameraPojectionType(ECPT_PERSPECTIVE),
	m_DefaultController(controller), m_CustomController(NULL)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CCameraSceneNode");
#endif

    BBox.reset(0,0,0);

    // set default view

    UpVector.set(0.0f, 1.0f, 0.0f);
    Target.set(lookat);

	setPosition(position);

    // set default projection

    fovy = core::PI / 2.5f; // Field of view, in radians. 
    aspect = 4.0f / 3.0f;   // Aspect ratio. 
    zn = 1.0f;              // value of the near view-plane. 
    zf = 5000.0f;           // Z-value of the far view-plane. 
	ViewVolume.Height = 5000;
	ViewVolume.Width  = 5000;

    vid::IVideoDriver* d = VIDEO_DRIVER_PTR;
    if (d)
    {
        screenDim.Width = (f32)d->getScreenSize().Width;
        screenDim.Height = (f32)d->getScreenSize().Height;
        aspect = screenDim.Width/screenDim.Height;
    }	

    recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

CCameraSceneNode::~CCameraSceneNode()
{
	SAFE_DROP(m_CustomController);
	SAFE_DROP(m_DefaultController);
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setProjectionType(E_CAMERA_PROJECTION_TYPE prj)
{
	CameraPojectionType = prj;

	recalculateProjectionMatrix();	
}

//---------------------------------------------------------------------------

E_CAMERA_PROJECTION_TYPE CCameraSceneNode::getProjectionType()
{
	return CameraPojectionType;
}

//---------------------------------------------------------------------------

const core::matrix4& CCameraSceneNode::getProjectionMatrix()
{
    return ViewArea.Projection;
}

//---------------------------------------------------------------------------

const core::matrix4& CCameraSceneNode::getViewMatrix()
{
    return ViewArea.View;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setTarget(const core::vector3df& pos)
{
    Target = pos;
}

//---------------------------------------------------------------------------

core::vector3df CCameraSceneNode::getTarget() const
{
    return Target;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setUpVector(const core::vector3df& up)
{
	if (up.X||up.Y||up.Z)
	{
		UpVector = up;
		recalculateProjectionMatrix();
	}
}

//---------------------------------------------------------------------------

core::vector3df CCameraSceneNode::getUpVector() const
{
    return UpVector;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setNearValue(f32 f)
{
	zn = f;

	if (zn != zn || zf != zf || zn < 1.f || zf <= zn)
	{
		LOGGER.logWarn("Incorrect camera parameters {zNear=%f, zFar=%f, FOV=%f, aspect=%f} fixed.", zn, zf, fovy, aspect);
		if (zn < 1.f || zn != zn)
			zn = 1.f;
		if (zf <= zn || zf != zf)
			zf = zn + 1.f;
	}
    recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getNearValue()
{
    return zn;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setFarValue(f32 f)
{
    zf = f;

	if (zn != zn || zf != zf || zn < 1.f || zf <= zn)
	{
		LOGGER.logWarn("Incorrect camera parameters {zNear=%f, zFar=%f, FOV=%f, aspect=%f} fixed.", zn, zf, fovy, aspect);
		if (zn < 1.f || zn != zn)
			zn = 1.f;
		if (zf <= zn || zf != zf)
			zf = zn + 1.f;
	}
    recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getFarValue()
{
    return zf;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setAspectRatio(f32 f)
{
    aspect = f;

	if (aspect != aspect)
	{
		LOGGER.logWarn("Incorrect camera parameters {zNear=%f, zFar=%f, FOV=%f, aspect=%f} fixed.", zn, zf, fovy, aspect);
		aspect = 1.f;
	}

    recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getAspectRatio()
{
    return aspect;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setFOV(f32 f)
{
    fovy = f;

	if (fovy != fovy)
	{
		LOGGER.logWarn("Incorrect camera parameters {zNear=%f, zFar=%f, FOV=%f, aspect=%f} fixed.", zn, zf, fovy, aspect);
		fovy = 1.f;
	}
    recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getFOV()
{
    return fovy;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setViewVolume(core::dimension2df view_volume)
{
	ViewVolume = view_volume;
	recalculateProjectionMatrix();
}

//---------------------------------------------------------------------------

core::dimension2df CCameraSceneNode::getViewVolume()
{
	return ViewVolume;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::recalculateProjectionMatrix()
{
	if (CameraPojectionType == ECPT_ORTHOGONAL)
		ViewArea.setProjectionOrtho(
			ViewVolume.Width, ViewVolume.Height, zn, zf);
	else
		ViewArea.setProjectionPerspective(fovy, aspect, zn, zf);
}

//---------------------------------------------------------------------------

void CCameraSceneNode::OnCheckCulling()
{
	if (!m_CullingChecked)
	{		
		m_Culled = SCENE_MANAGER.getActiveCamera()!=this;
		ISceneNode::OnCheckCulling();
	}
}

//---------------------------------------------------------------------------

void CCameraSceneNode::OnPreRender(u32 timeMs)
{
	scn::ISceneManager &smgr   = SCENE_MANAGER;
    vid::IVideoDriver  &driver = VIDEO_DRIVER;

    screenDim.Width  = (f32)driver.getScreenSize().Width;
    screenDim.Height = (f32)driver.getScreenSize().Height;

	if (m_CustomController)
		m_CustomController->reset();
	else
	if (m_DefaultController)
		m_DefaultController->reset();

    ISceneNode::OnPreRender(timeMs);
}

//---------------------------------------------------------------------------

void CCameraSceneNode::OnPostRender(u32 timeMs)
{
	ISceneNode::OnPostRender(timeMs);
	animate();
}

//---------------------------------------------------------------------------

const core::aabbox3d<f32>& CCameraSceneNode::getBoundingBox() const
{
    return BBox;
}

//---------------------------------------------------------------------------

const core::view_frustrum& CCameraSceneNode::getViewFrustrum()
{
    return ViewArea;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::recalculateViewArea()
{
	// if upvector and vector to the target are the same, we have a
    // problem. so solve this problem:

    core::vector3df pos = getAbsolutePosition();

    core::vector3df tgtv = getTarget() - pos;
    tgtv.normalize();

    core::vector3df up = getUpVector();
    up.normalize();

    f32 dp = tgtv.getDotProduct(up);
    if ((dp > -1.0001f && dp < -0.9999f) ||
			(dp < 1.0001f && dp > 0.9999f))
		up.X += 1.0f;
	
	ViewArea.setView(pos, getTarget(), up);
}

//---------------------------------------------------------------------------

void CCameraSceneNode::animate()
{
	u32 nowSystemTime = DEVICE.getDeviceSystemTime();
	if (m_CustomController)
		m_CustomController->animate(this, nowSystemTime);
	else
	if (m_DefaultController)
		m_DefaultController->animate(this, nowSystemTime);
	recalculateViewArea();
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getZoomSpeed()
{
	return ZoomSpeed;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setZoomSpeed(f32 zoom_speed)
{
	ZoomSpeed = zoom_speed;
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getRotationSpeed()
{
	return RotationSpeed;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setRotationSpeed(f32 rot_speed)
{
	RotationSpeed = rot_speed;
}

//---------------------------------------------------------------------------

f32 CCameraSceneNode::getMoveSpeed()
{
	return MoveSpeed;
}

//---------------------------------------------------------------------------

void CCameraSceneNode::setMoveSpeed(f32 move_speed)
{
	MoveSpeed = move_speed;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
