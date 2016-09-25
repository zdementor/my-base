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

COpenGLRenderTarget::COpenGLRenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	: CNullRenderTarget(size, colorFormat, depthFormat),
m_FBO(0)
{
	_rebuild();
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::COpenGLRenderTarget(
	ITexture *colorTexture, ITexture *depthTexture)
	: CNullRenderTarget(colorTexture, depthTexture),
m_FBO(0)
{
	_rebuild();
}

//----------------------------------------------------------------------------

COpenGLRenderTarget::~COpenGLRenderTarget()
{
#if GL_ARB_framebuffer_object
	if (m_FBO)
	{
		glDeleteFramebuffers(1, &m_FBO);
		m_FBO = 0;
	}
#endif
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

		ITexture *colAttach = m_ColorTexture;
		ITexture *depAttach = m_DepthTexture;

		if (colAttach)
		{
			GLuint colTex = (GLuint)(size_t)colAttach->getHardwareTexture();
			GLuint depTex = (GLuint)(size_t)(depAttach ? depAttach->getHardwareTexture() : 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D, depTex, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, colTex, 0);
		}

		GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_COMPLETE)
		{
			m_OK = true;
		}
		else
			LOGGER.logErr("Can not complete OGL render target (status=0x%08X)", status);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
		LOGGER.logErr("Can not create OGL render target");
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
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
 