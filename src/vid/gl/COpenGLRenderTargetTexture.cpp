//|-------------------------------------------------------------------------
//| File:        COpenGLRenderTargetTexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLRenderTargetTexture.h"
#include "COpenGLDriver.h"

#include <io/ILogger.h>
#include <vid/IVideoDriver.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

COpenGLRenderTargetTexture::COpenGLRenderTargetTexture(const core::dimension2di &size)
	: COpenGLTexture()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLRenderTargetTexture");    
#endif

	m_TextureName.v = NULL;

	memset(m_ImageData, 0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageData));

	m_MaxMipMapLevels = 1;
	m_AutogenMipMaps = false;

	m_ImageSize = size;
	m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(size.Width);
	m_TextureSize.Height = core::math::GetNearestPowerOfTwo(size.Height);
   
	m_Pitch = m_TextureSize.Width*4;
	m_ColorFormat = img::ECF_A8B8G8R8;

	m_ImageDataSizeBytes[0] = m_Pitch * m_TextureSize.Width;

	createHardwareTexture();

	ITexture *curtex0 = m_Driver->_getCurrentTexture(0);
	if (this != curtex0)
		m_Driver->_setTexture(0, this);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		m_InternalFormat = GL_RGBA8, 
		m_TextureSize.Width,
		m_TextureSize.Height, 
		0, 
		m_PixelFormat = GL_RGBA, 
		m_PixelType = GL_UNSIGNED_BYTE,
		0);

	if (this != curtex0)
		m_Driver->_setTexture(0, curtex0);

	core::stringc msg;
	msg.sprintf(
		"Created render target texture ( %s, mips %s, %dx%d )", 
		img::ColorFormatStr[m_ColorFormat],
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
	LOGGER.log(msg.c_str());

	m_Created = true;
}

//----------------------------------------------------------------------------

COpenGLRenderTargetTexture::~COpenGLRenderTargetTexture()
{
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
