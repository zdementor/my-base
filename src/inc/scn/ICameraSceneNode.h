//|-------------------------------------------------------------------------
//| File:        ICameraSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ICameraSceneNodeHPP
#define ICameraSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <io/IEventReceiver.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Scene Node which is a (controlable) camera.
//! The whole scene will be
//! rendered from the cameras point of view. Because the ICameraScenNode
//! is a SceneNode, it can be attached to any other scene node, and will
//! follow its parents movement, rotation and so on.
class __MY_SCN_LIB_API__ ICameraSceneNode : public ISceneNode
{
public:

    //! Constructor
    ICameraSceneNode(ISceneNode* parent, s32 id ) 
		: ISceneNode(ESNT_CAMERA_SCENE_NODE, parent, id),
		m_SceneEditorCamera(false) {}

    //! Destructor
    virtual ~ICameraSceneNode() {};

    //! Gets the current projection matrix of the camera.
    //! \return Returns the current projection matrix of the camera.
    virtual const core::matrix4& getProjectionMatrix() = 0;

    //! Gets the current view matrix of the camera.
    //! \return Returns the current view matrix of the camera.
    virtual const core::matrix4& getViewMatrix() = 0;

    //! Sets the look at target of the camera
    //! \param pos: Look at target of the camera.
    virtual void setTarget(const core::vector3df& pos) = 0;

    //! Gets the current look at target of the camera
    //! \return Returns the current look at target of the camera
    virtual core::vector3df getTarget() const = 0;

    //! Sets the up vector of the camera.
    //! \param pos: New upvector of the camera.
    virtual void setUpVector(const core::vector3df& pos) = 0;

    //! Gets the up vector of the camera.
    //! \return Returns the up vector of the camera.
    virtual core::vector3df getUpVector() const = 0;    

    //! Sets the value of the near clipping plane. (default: 1.0f)
    //! \param zn: New z near value.
    virtual void setNearValue(f32 zn) = 0;

	//! \return Returns the value of the near plane of the camera.
    virtual f32 getNearValue() = 0;

    //! Sets the value of the far clipping plane (default: 2000.0f)
    //! \param zf: New z far value.
    virtual void setFarValue(f32 zf) = 0;

	//! \return Returns the value of the far plane of the camera.
    virtual f32 getFarValue() = 0;

    //! Sets the aspect ratio (default: 4.0f / 3.0f)
	//! ( This method has affect only for cameras with perspective projection )
    //! \param aspect: New aspect ratio.
    virtual void setAspectRatio(f32 aspect) = 0;

	//! \return Returns the aspect ratio of the camera.
    virtual f32 getAspectRatio() = 0;

    //! Sets the field of view (Default: PI / 2.5f)
	//! ( This method has affect only for cameras with perspective projection )
    //! \param fovy: New field of view.
    virtual void setFOV(f32 fovy) = 0;

	//! \return Returns the field of view of the camera.
    virtual f32 getFOV() = 0;

	//! Setting height and width of view volume (Default: 5000x5000)
	//! ( This method has affect only for cameras with orthogonal projection )
	//! \param width: New width of camera view
	virtual void setViewVolume(core::dimension2df view_volume) = 0;

	//! \return Returns height and width of view volume
	virtual core::dimension2df getViewVolume() = 0;

    //! Returns the view frustrum. Needed sometimes by bspTree or LOD render nodes.
    //! \return Returns the current view frustrum.
    virtual const core::view_frustrum& getViewFrustrum() = 0;

	//! setting camera projection mode
	//! \param prj: new camera projection type
	virtual void setProjectionType(E_CAMERA_PROJECTION_TYPE prj) = 0;

	//! setting camera projection mode
	virtual E_CAMERA_PROJECTION_TYPE getProjectionType() = 0;

	virtual f32 getZoomSpeed() = 0;
	virtual void setZoomSpeed(f32 zoom_speed) = 0;

	virtual f32 getRotationSpeed() = 0;
	virtual void setRotationSpeed(f32 rot_speed) = 0;

	virtual f32 getMoveSpeed() = 0;
	virtual void setMoveSpeed(f32 move_speed) = 0;

	virtual void setCustomController(ICameraController *controller) = 0;
	virtual ICameraController& getController() = 0;

	virtual void animate() = 0;

	virtual void setSceneEditorCamera(bool value)
	{ m_SceneEditorCamera = value; }

	virtual bool isSceneEditorCamera()
	{ return m_SceneEditorCamera; }

private:
	bool m_SceneEditorCamera;
};

//---------------------------------------------------------------------------

class ICameraController : public IUnknown,
	public io::IEventReceiver, public io::IEventCapturer
{
public:
	ICameraController()
		: m_TimeDiff(0), m_LastTimeMs(0), m_AutoCaptureInput(true)
	{
#if MY_DEBUG_MODE 
	setClassName("ICameraController");
#endif 
	}
	virtual void animate(ICameraSceneNode *camera, u32 timeMs)
	{
		if (m_AutoCaptureInput)
			captureInput();
		if (!m_LastTimeMs)
			m_LastTimeMs = timeMs;
		m_TimeDiff = timeMs - m_LastTimeMs;
		m_LastTimeMs = timeMs;

		m_Pos = camera->getPosition();
		m_Tgt = camera->getTarget();
		m_Up = camera->getUpVector();
	}
	u32 getTimeDiff()
	{ return m_TimeDiff; }
	virtual void reset() = 0;
	virtual bool OnEvent(const io::SEvent &event)
	{
		collectEvent(event);
		return true;
	}
	void setAutoCaptureInput(bool auto_capture)
	{ m_AutoCaptureInput = auto_capture; }
	bool isAutoCaptureInput()
	{ return m_AutoCaptureInput; }
protected:
	core::vector3df m_Pos, m_Tgt, m_Up;
private:
	u32 m_TimeDiff, m_LastTimeMs;
	bool m_AutoCaptureInput;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ICameraSceneNodeHPP

