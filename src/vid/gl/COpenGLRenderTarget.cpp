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
	const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
	E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(size, colorFormat, flags),
	m_FBO(0)
{
#if GL_ARB_framebuffer_object
	glGenFramebuffers(1, &m_FBO);
#endif
	LOGGER.logInfo("%s (%p) %dx%d format=%s flags=0x%08X",
		__FUNCTION__, this,
		size.Width, size.Height, img::ColorFormatStr[colorFormat], flags);

	m_ColorAttachements[0] = VIDEO_DRIVER.createRenderTargetTexture(m_Size, m_ColorFormat);
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(colorRenderTarget->getSize(), colorRenderTarget->getColorFormat(), flags),
	m_FBO(0)
{
#if GL_ARB_framebuffer_object
	glGenFramebuffers(1, &m_FBO);
#endif
	LOGGER.logInfo("%s (%p)", __FUNCTION__, this);

	m_ColorAttachements[0] = VIDEO_DRIVER.createRenderTargetTexture(m_Size, m_ColorFormat);
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::~COpenGLRenderTarget()
{
#if GL_ARB_framebuffer_object
	glDeleteFramebuffers(1, &m_FBO);
#endif
	LOGGER.logInfo("%s (%p)", __FUNCTION__, this);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
 