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

#include "COpenGLRenderTarget.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	const core::dimension2di &size, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(size, flags),
	m_FBO(0)
{
#if GL_ARB_framebuffer_object
	glGenFramebuffers(1, &m_FBO);
#endif
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(colorRenderTarget->getSize(), flags),
	m_FBO(0)
{
#if GL_ARB_framebuffer_object
	glGenFramebuffers(1, &m_FBO);
#endif
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::~COpenGLRenderTarget()
{
#if GL_ARB_framebuffer_object
	glDeleteFramebuffers(1, &m_FBO);
#endif
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
 