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

void CNullRenderTarget::_defaultInit()
{
	m_OK = false;
	m_RTEntry = 0;

	m_DepthTexture = 0;
	m_DepthFormat = img::ECF_NONE;

	memset(m_Size, 0, sizeof(m_Size));
	memset(m_ColorFormat, 0, sizeof(m_ColorFormat));
	memset(m_ColorTexture, 0, sizeof(m_ColorTexture));
}

//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget()
	: m_Driver(VIDEO_DRIVER)
{
	_defaultInit();	
}

//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(const core::dimension2di &size,
	img::E_COLOR_FORMAT colorFormat, img::E_COLOR_FORMAT depthFormat)
	: m_Driver(VIDEO_DRIVER)
{
	_defaultInit();

	m_Size[0] = size;

	m_ColorFormat[0] = colorFormat;
	m_ColorTexture[0] = m_Driver.createRenderTargetTexture(size, colorFormat);

	if (m_Driver.queryFeature(vid::EVDF_DEPTH_STENCIL_TEXTURES)
			&& depthFormat == img::ECF_DEPTH24_STENCIL8)
	{
		m_DepthFormat = depthFormat;
		m_DepthTexture = m_Driver.createRenderTargetTexture(size, depthFormat);
	}
}

//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(ITexture *colorTexture, ITexture *depthTexture)
	: m_Driver(VIDEO_DRIVER)
{
	_defaultInit();

	bindColorTexture(colorTexture, false);
	bindDepthTexture(depthTexture, false);
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	for (u32 no = 0; no < MY_MAX_COLOR_ATTACHMENTS; no++)
		SAFE_DROP(m_ColorTexture[no]);
	SAFE_DROP(m_DepthTexture);
}

//----------------------------------------------------------------------------

bool CNullRenderTarget::bindColorTexture(u32 no, ITexture *colorTexture, bool doRebuild)
{
	if (no >= MY_MAX_COLOR_ATTACHMENTS)
		return false;

	bool ret = true;

	if (colorTexture && colorTexture->isRenderTarget())
	{
		if (no == 0 && m_DepthTexture && m_Size[0] != colorTexture->getSize())
			ret = false;
		if (ret)
		{
			SAFE_DROP(m_ColorTexture[no]);
			colorTexture->grab();
			m_Size[no] = colorTexture->getSize();
			m_ColorFormat[no] = colorTexture->getColorFormat();
			m_ColorTexture[no] = colorTexture;
		}
	}
	else
		ret = false;

	if (ret && doRebuild)
		ret = _rebuild();

	return ret;
}

//----------------------------------------------------------------------------

bool CNullRenderTarget::bindDepthTexture(ITexture *depthTexture, bool doRebuild)
{
	bool ret = true;

	if (depthTexture
			&& m_Driver.queryFeature(vid::EVDF_DEPTH_STENCIL_TEXTURES)
			&& depthTexture->getColorFormat() == img::ECF_DEPTH24_STENCIL8
			&& depthTexture->isRenderTarget())
	{
		ITexture *colTex0 = m_ColorTexture[0];
		if (!colTex0)
			m_Size[0] = depthTexture->getSize();
		else if (m_Size[0] != depthTexture->getSize())
			ret = false;
		if (ret)
		{
			SAFE_DROP(m_DepthTexture);
			depthTexture->grab();
			m_DepthFormat = img::ECF_DEPTH24_STENCIL8;
			m_DepthTexture = depthTexture;
		}
	}
	else
		ret = false;

	if (ret && doRebuild)
		ret = _rebuild();

	return ret;
}

//----------------------------------------------------------------------------

bool CNullRenderTarget::bind()
{
	m_Driver.setViewPort(0, 0, m_Size[0].Width, m_Size[0].Height);
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
