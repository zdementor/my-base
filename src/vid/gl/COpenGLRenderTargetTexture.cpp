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

COpenGLRenderTargetTexture::COpenGLRenderTargetTexture(
	const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat)
	: COpenGLTexture()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLRenderTargetTexture");    
#endif

	m_TextureName.v = NULL;

	memset(m_ImageData, 0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageData));

	m_MaxMipMapLevels = 1;
	m_AutogenMipMaps = false;

	m_TextureSize = m_ImageSize = size;
	//m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(size.Width);
	//m_TextureSize.Height = core::math::GetNearestPowerOfTwo(size.Height);
   
	m_Pitch = m_TextureSize.Width*4;
	m_ColorFormat = colorFormat;

	m_ImageDataSizeBytes[0] = m_Pitch * m_TextureSize.Width;

	createHardwareTexture();

	ITexture *curtex0 = m_Driver->_getCurrentTexture(0);
	if (this != curtex0)
		m_Driver->_setTexture(0, this);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		m_InternalFormat, 
		m_TextureSize.Width,
		m_TextureSize.Height, 
		0, 
		m_PixelFormat, 
		m_PixelType,
		NULL);

	if (this != curtex0)
		m_Driver->_setTexture(0, curtex0);

	LOGGER.logInfo("Created render target texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);

	m_Created = true;
}

//----------------------------------------------------------------------------

COpenGLRenderTargetTexture::~COpenGLRenderTargetTexture()
{
	LOGGER.logInfo("Destroyed render target texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
