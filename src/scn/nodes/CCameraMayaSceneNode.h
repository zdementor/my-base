//|-------------------------------------------------------------------------
//| File:        CCameraMayaSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CCameraMayaSceneNodeHPP
#define CCameraMayaSceneNodeHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include "CCameraSceneNode.h"

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class CCameraMayaController : public ICameraController
{
public:
	CCameraMayaController()
	{
	    zooming = false;
		rotating = false;
		moving = false;
		translating = false;
		currentZoom = 70.0f;
	}
	virtual void animate(ICameraSceneNode *camera, u32 timeMs);
	virtual void reset() {}
private:
    bool zooming, rotating, moving, translating;
    f32 rotateStartX, rotateStartY;
    f32 zoomStartX, zoomStartY;
    f32 translateStartX, translateStartY;
    f32 currentZoom;
};

//---------------------------------------------------------------------------

class CCameraMayaSceneNode : public CCameraSceneNode
{
public:
    CCameraMayaSceneNode(ISceneNode* parent, s32 id,
        f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed);
    virtual ~CCameraMayaSceneNode();
private:
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------
#endif

