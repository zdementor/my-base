//|-------------------------------------------------------------------------
//| File:        IShadow.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IShadowHPP
#define IShadowHPP
//---------------------------------------------------------------------------

#include <IUnknown.h>
#include <scn/IMesh.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Shadow casted from single light
struct SLightShadow
{
	core::aabbox3df BoundingBox;
	core::aabbox3df TransformedBoundingBox;
};

//---------------------------------------------------------------------------

//! Shadow 
class __MY_SCN_LIB_API__ IShadow : public IUnknown
{
public:

	//! returns the shadow type value
    virtual E_SHADOW_TYPE getShadowType() = 0;

	//! register Shadow for rendering in Video Driver rendering cache
	virtual void registerForRendering(
		bool fake_shadow, // turn on/off fake shadow mode
		u32 nowMs, // current time ms
		const vid::SLight &light0, u64 enabled_lights,
		IAnimatedMesh *amesh, IMesh *mesh, bool render_debug) = 0;

	//! return true if this shadow scene node attached to main player game node
	virtual bool isMainPlayerShadow() = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif //IShadowHPP

