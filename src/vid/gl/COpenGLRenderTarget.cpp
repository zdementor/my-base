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
#include "COpenGLTexture.h"
 
//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
	E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(size, colorFormat, flags),
	m_FBO(0), m_OK(false)
{
	_rebuild();

	LOGGER.logInfo("%s (%p) %dx%d format=%s flags=0x%08X",
		__FUNCTION__, this,
		size.Width, size.Height, img::getColorFormatName(colorFormat), flags);
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags)
	: CNullRenderTarget(colorRenderTarget->getSize(), colorRenderTarget->getColorFormat(), flags),
	m_FBO(0), m_OK(false)
{
	_rebuild();

	LOGGER.logInfo("%s (%p)", __FUNCTION__, this);
}

//----------------------------------------------------------------------------

bool COpenGLRenderTarget::_rebuild()
{
	m_OK = false;
#if GL_ARB_framebuffer_object
	if (!m_FBO)
		glGenFramebuffers(1, &m_FBO);
	if (m_FBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		COpenGLTexture *colAttach = (COpenGLTexture *)m_ColorAttachment;
		if (colAttach)
		{
			GLuint tex = (GLuint)(size_t)colAttach->getHardwareTexture();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
		}
		GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_COMPLETE)
		{
			m_OK = true;
			LOGGER.logInfo("Complete OGL render target OK");
		}
		else
			LOGGER.logErr("Can not complete OGL render target (status=0x%08X)", status);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
#endif
	return m_OK;
}

//----------------------------------------------------------------------------

bool COpenGLRenderTarget::bind()
{
	if (!m_OK)
		return false;
#if GL_ARB_framebuffer_object
	//glFlush ();
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif
	return CNullRenderTarget::bind();
}

//----------------------------------------------------------------------------

bool COpenGLRenderTarget::unbind()
{
	if (!m_OK)
		return false;
#if GL_ARB_framebuffer_object
	//glFlush ();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
	return CNullRenderTarget::unbind();
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
 