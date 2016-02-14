//|-------------------------------------------------------------------------
//| File:        IAnimatedMeshCal3d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IAnimatedMeshCal3dHPP
#define IAnimatedMeshCal3dHPP
//----------------------------------------------------------------------------

#include <scn/IAnimatedMesh.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {

//----------------------------------------------------------------------------

//! Interface for using some special functions of Cal3d meshes
class __MY_SCN_LIB_API__ IAnimatedMeshCal3d : public IAnimatedMesh
{
public:

	//! blending current animation with previous
	virtual bool executeCal3dAction(u32 anim, f32 delay=0.3f, bool loop=false) = 0;  
	
	//! blending current animation with previous
	virtual bool removeCal3dAction(f32 delay=0.3f) = 0;  
};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace my
//----------------------------------------------------------------------------

#endif

