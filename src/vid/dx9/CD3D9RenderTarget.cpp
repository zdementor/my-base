//|-------------------------------------------------------------------------
//| File:        COpenGLRenderTarget.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CD3D9RenderTarget.h"
#include "CD3D9Driver.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

CD3D9RenderTarget::CD3D9RenderTarget(
	const core::dimension2di &size, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(size, flags)
{
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::CD3D9RenderTarget(
	ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(colorRenderTarget->getSize(), flags)
{
}

//----------------------------------------------------------------------------

CD3D9RenderTarget::~CD3D9RenderTarget()
{
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
  