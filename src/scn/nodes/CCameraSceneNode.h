//|-------------------------------------------------------------------------
//| File:        CCameraSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCameraSceneNodeHPP
#define CCameraSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ICameraSceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CCameraSceneNode : public ICameraSceneNode
{
public:

    CCameraSceneNode(ISceneNode* parent, s32 id, 
		ICameraController *controller,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& lookat = core::vector3df(0,0,1));
    virtual ~CCameraSceneNode();

    virtual const core::matrix4& getProjectionMatrix();

    virtual const core::matrix4& getViewMatrix();

    virtual void setTarget(const core::vector3df& pos);
    virtual core::vector3df getTarget() const;

    virtual void setUpVector(const core::vector3df& pos);
    virtual core::vector3df getUpVector() const;    

    virtual void setNearValue(f32 zn);
    virtual f32 getNearValue();

    virtual void setFarValue(f32 zf);
    virtual f32 getFarValue();

    virtual void setAspectRatio(f32 aspect);
    virtual f32 getAspectRatio();

    virtual void setFOV(f32 fovy);
    virtual f32 getFOV();

	virtual void setViewVolume(core::dimension2df view_volume);

	virtual core::dimension2df getViewVolume();

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);
    virtual void OnPostRender(u32 timeMs);

    virtual const core::aabbox3d<f32>& getBoundingBox() const;

    virtual const core::view_frustrum& getViewFrustrum();

	virtual void setProjectionType(E_CAMERA_PROJECTION_TYPE prj);
	virtual E_CAMERA_PROJECTION_TYPE getProjectionType();

	virtual f32 getZoomSpeed();
	virtual void setZoomSpeed(f32 zoom_speed);

	virtual f32 getRotationSpeed();
	virtual void setRotationSpeed(f32 rot_speed);

	virtual f32 getMoveSpeed();
	virtual void setMoveSpeed(f32 move_speed);

	virtual void setCustomController(ICameraController *controller)
	{
		SAFE_DROP(m_CustomController);
		m_CustomController = controller;
		SAFE_GRAB(m_CustomController);
	}

	virtual ICameraController& getController()
	{
		return m_CustomController ?
			*m_CustomController : *m_DefaultController;
	}

	virtual const c8* getSceneCorePropertiesXMLString()
	{
		static core::stringc xmlstr;
		xmlstr = "";
		xmlstr.append("\n");
		return xmlstr.c_str();
	}

	virtual void animate();

protected:
    
	void recalculateProjectionMatrix();
    void recalculateViewArea();

private:

	ICameraController *m_CustomController, *m_DefaultController;

    //core::vector3df Pos;
    core::vector3df Target;
    core::vector3df UpVector;

    core::aabbox3d<f32> BBox;

    f32 fovy;   // Field of view, in radians. 
    f32 aspect; // Aspect ratio. 
    f32 zn;     // value of the near view-plane. 
    f32 zf;     // Z-value of the far view-plane.
	core::dimension2df ViewVolume;

    core::dimension2d<f32> screenDim;

    core::view_frustrum ViewArea;

	E_CAMERA_PROJECTION_TYPE CameraPojectionType;

	f32 ZoomSpeed;
    f32 RotationSpeed;
    f32 MoveSpeed;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

