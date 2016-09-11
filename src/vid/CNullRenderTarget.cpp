//|-------------------------------------------------------------------------
//| File:        CNullRenderTarget.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CNullRenderTarget.h"
#include <vid/ITexture.h>
#include <vid/IVideoDriver.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormat)
	: m_OK(false), m_Driver(VIDEO_DRIVER), m_RTEntry(0), m_ColorTexture(NULL),
m_Size(size), m_ColorFormat(colorFormat), m_DepthFormat(depthFormat)
{
	m_ColorTexture = m_Driver.createRenderTargetTexture(m_Size, m_ColorFormat);
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	SAFE_DROP(m_ColorTexture);
}

//----------------------------------------------------------------------------

bool CNullRenderTarget::bind()
{
	m_Driver.setViewPort(0, 0, m_Size.Width, m_Size.Height);
	return true;
}

//----------------------------------------------------------------------------

bool CNullRenderTarget::unbind()
{
	return true;
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
