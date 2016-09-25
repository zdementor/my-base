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
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	: m_OK(false), m_Driver(VIDEO_DRIVER), m_RTEntry(0),
m_ColorTexture(NULL), m_DepthTexture(NULL),
m_Size(size), m_ColorFormat(colorFormat), m_DepthFormat(depthFormat)
{
	m_ColorTexture = m_Driver.createRenderTargetTexture(m_Size, m_ColorFormat);
	if (m_Driver.queryFeature(vid::EVDF_DEPTH_STENCIL_TEXTURES)
			&& m_DepthFormat == img::ECF_DEPTH24_STENCIL8)
		m_DepthTexture = m_Driver.createRenderTargetTexture(m_Size, m_DepthFormat);
}

//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(ITexture *colorTexture, ITexture *depthTexture)
	: m_OK(false), m_Driver(VIDEO_DRIVER), m_RTEntry(0),
m_ColorTexture(NULL), m_DepthTexture(NULL),
m_Size(0, 0), m_ColorFormat(img::ECF_NONE), m_DepthFormat(img::ECF_NONE)
{
	if (colorTexture && colorTexture->isRenderTarget())
	{
		colorTexture->grab();
		m_ColorFormat = colorTexture->getColorFormat();
		m_Size = colorTexture->getSize();
		m_ColorTexture = colorTexture;
	}

	if (depthTexture
			&& m_Driver.queryFeature(vid::EVDF_DEPTH_STENCIL_TEXTURES)
			&& depthTexture->getColorFormat() == img::ECF_DEPTH24_STENCIL8
			&& depthTexture->isRenderTarget())
	{
		depthTexture->grab();
		m_DepthFormat = img::ECF_DEPTH24_STENCIL8;
		if (!m_ColorTexture)
			m_Size = depthTexture->getSize();
		m_DepthTexture = depthTexture;
	}
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	SAFE_DROP(m_ColorTexture);
	SAFE_DROP(m_DepthTexture);
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
