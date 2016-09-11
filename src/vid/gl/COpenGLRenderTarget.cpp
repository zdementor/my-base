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
	img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormats)
	: CNullRenderTarget(size, colorFormat, depthFormats),
m_FBO(0), m_DepthStencilBuffer(0)
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
	if (m_DepthStencilBuffer)
	{
        glDeleteRenderbuffers( 1, &m_DepthStencilBuffer);
		m_DepthStencilBuffer = 0;
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
		if (colAttach)
		{
			GLuint tex = (GLuint)(size_t)colAttach->getHardwareTexture();
			
			GLenum internalFormat = GL_NONE;
			switch (m_DepthFormat)
			{
			case ERTDF_DEPTH16:
				internalFormat = GL_DEPTH_COMPONENT16;
				break;
			case ERTDF_DEPTH24:
				internalFormat = GL_DEPTH_COMPONENT24;
				break;
			case ERTDF_DEPTH32:
				internalFormat = GL_DEPTH_COMPONENT32;
				break;
			case ERTDF_DEPTH24_STENCIL8:
				internalFormat = GL_DEPTH24_STENCIL8;
				break;
			default:
				break;
			}
			if (internalFormat != GL_NONE)
			{
				if (!m_DepthStencilBuffer)
					glGenRenderbuffers(1, &m_DepthStencilBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, internalFormat,
					m_Size.Width, m_Size.Height);
			}
			else
			{
				if (m_DepthStencilBuffer)
				{
					glDeleteRenderbuffers(1, &m_DepthStencilBuffer);
					m_DepthStencilBuffer = 0;
				}
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}
			
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,  GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER, m_DepthStencilBuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, tex, 0);
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
	if (!m_OK)
	{
		if (m_DepthStencilBuffer)
		{
			glDeleteRenderbuffers(1, &m_DepthStencilBuffer);
			m_DepthStencilBuffer = 0;
		}
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
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
 