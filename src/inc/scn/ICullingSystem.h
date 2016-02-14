//|-------------------------------------------------------------------------
//| File:        ICullingSystem.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ICullingSystemHPP
#define ICullingSystemHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

//! The Cull System doing geometry culling
class __MY_SCN_LIB_API__ ICullingSystem : public IUnknown, public ISingleton<ICullingSystem>
{
public:

	//! return true if this Scene Node is culled
	virtual bool isCull(ISceneNode* node) const = 0;

	//! return true if this Bounding box is culled
	virtual bool isCull(const core::aabbox3df &bb, E_CULLING_TESTS tests) const = 0;

	//! return true if this 3D position is culled
	virtual bool isCull(const core::vector3df &pos, E_CULLING_TESTS tests) const = 0;

	//! return true if this 3D sphere is culled
	virtual bool isCull(const core::vector3df &center, f32 radius, E_CULLING_TESTS tests) const = 0;

	virtual bool addOccluder(ISceneNode* node) = 0;

	virtual s32 getOccludersCount() = 0;

	virtual void clearOccluders() = 0;

	virtual void initializeOccluders() = 0;

	virtual void setViewer(const core::view_frustrum &frustrum, const core::vector3df &look_at) = 0;

	virtual const core::view_frustrum& getViewerFrustrum() = 0;
	virtual const core::vector3df&     getViewerLookAt() = 0;

	//!	Return singleton object
	static ICullingSystem& getSingleton();

	//! Return pointer to singleton object
	static ICullingSystem* getSingletonPtr();
};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace irr
//----------------------------------------------------------------------------

#endif // #ifndef ICullingSystemHPP

