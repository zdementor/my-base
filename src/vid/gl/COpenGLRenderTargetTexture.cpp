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

	bool ret = createEmptyTexture(size, colorFormat, true);
	if (ret)
		createTextureLevel(0,  NULL, 0, m_ColorFormat);
	m_Created = true;

	LOGGER.log((!ret) ? io::ELL_ERROR : io::ELL_INFORMATION,
		"Created render target texture ( %s, mips %s, %dx%d )%s", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height,
		!ret ? " with errors" : "");
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
