//|-------------------------------------------------------------------------
//| File:        IGameNodeMainPlayer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameNodeMainPlayerHPP
#define IGameNodeMainPlayerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <game/IGameNodeAI.h>
#include <mm/ISoundDriver.h>
#include <scn/ISceneManager.h>
#include <io/ILogger.h>
#include <game/SGameCameraStyle.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

//! Game Units Fromation Class
class __MY_GAME_LIB_API__ IGameNodeMainPlayer : public IGameNodeAI
{
public:

	//! constructor
	IGameNodeMainPlayer(scn::ISceneNode* n, SGameNodeParams &params) 
		: IGameNodeAI(n, params), m_CurrentCameraStyleIdx(0), m_CameraAutoZoom(false), m_CameraZoomValue(0.0f)
	{
		m_SceneNode->setIsMainPlayer(true);

		SCENE_MANAGER.addCameraSceneNodeFPS(n);

		// default camera style   
		addCameraStyle(SGameCameraStyle());  
		setCameraStyle(0);
	}

	//! destructor
	virtual ~IGameNodeMainPlayer() {};

	//! calling every time before game sound playing
    virtual void OnPreSound()
	{
		IGameNodeAI::OnPreSound();

		scn::ICameraSceneNode* camera = m_SceneNode->getCamera();
		if (camera)
		{
			// positioning all sound effects, attached to this node
			core::list<mm::ISound*>::iterator it = m_SoundEffects.begin();

			for (; it!=m_SoundEffects.end(); ++it)
			{
				mm::ISound *sound = (*it);

				if (sound)
				{
					sound->moveTo((m_TransformedBBoxCenter+camera->getPosition())*0.5f);
				}
			}
		}
	}

	//! getting current game camera style
	virtual const SGameCameraStyle& getCurrentCameraStyle()
	{
		return m_CameraStyles[m_CurrentCameraStyleIdx];
	}

	//! adding new game camera style
	virtual void addCameraStyle(const SGameCameraStyle& style)
	{
		m_CameraStyles.push_back(style);
		m_CameraStyles[m_CameraStyles.size()-1].Name.make_lower();
	}

	//! getting game camera styles count
	virtual s32 getCameraStylesCount()
	{
		return m_CameraStyles.size();
	}

	//! setting game camera style by index
	virtual bool setCameraStyle(s32 idx)
	{
        if (idx>=0 && idx<getCameraStylesCount())
		{
			m_CurrentCameraStyleIdx = idx;

			setupCamera();

			return true;
		}

		return false;
	}

	//! setting game camera style by name
	virtual bool setCameraStyle(const c8* name)
	{
		SGameCameraStyle s;

		s.Name = name;
		s.Name.make_lower();  
		
		m_CurrentCameraStyleIdx = m_CameraStyles.binary_search(s);

		bool res = true;

		if (m_CurrentCameraStyleIdx==-1)
		{
			m_CurrentCameraStyleIdx = 0;

			core::stringc msg;
			msg.sprintf("Unknown camera style '%s', can't set it.", name);
			LOGGER.log(msg, io::ELL_ERROR);

			res = false;
		}

		setupCamera();

		return res;
	}

	//! setting camera auto zoom flag value
	virtual void setCameraAutoZoom(bool auto_zoom)
	{	
		m_CameraAutoZoom = auto_zoom;
	}

	//! getting camera auto zoom flag value
	virtual bool isCameraAutoZoom()
	{	
		return m_CameraAutoZoom;
	}

	//! setting camera zoom value [ 0.0 - 1.0 ]
    virtual void setCameraZoom(float zoom)
	{
		m_CameraZoomValue = zoom;
	}

    //! getting camera zoom value [ 0.0 - 1.0 ]
    virtual float getCameraZoom()
	{
		return m_CameraZoomValue;
	}

protected:

	bool m_CameraAutoZoom;
	f32 m_CameraZoomValue;

private:

	void setupCamera()
	{
		scn::ICameraSceneNode* camera = m_SceneNode->getCamera();

		if (camera)
		{
			const SGameCameraStyle& cam_style = getCurrentCameraStyle();

			camera->setUpVector		 (cam_style.UpVector     );
			camera->setRotationSpeed (cam_style.RotationSpeed);
			camera->setZoomSpeed     (cam_style.ZoomSpeed    );
			camera->setMoveSpeed     (cam_style.MoveSpeed    );			
			camera->setProjectionType(cam_style.ProjType     );
		}
	}

	core::array<SGameCameraStyle> m_CameraStyles;
	s32 m_CurrentCameraStyleIdx;	
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameNodeMainPlayerHPP