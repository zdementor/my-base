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

CNullRenderTarget::CNullRenderTarget(
	const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
	E_RENDER_TARGET_CREATION_FLAG flags)
	: m_ColorAttachment(0), m_DepthAttachment(0),
	m_Size(size), m_ColorFormat(colorFormat), m_Flags(flags), m_RTEntry(0),
	m_Driver(VIDEO_DRIVER)
{
	m_ColorAttachment = m_Driver.createRenderTargetTexture(m_Size, m_ColorFormat);

	img::E_COLOR_FORMAT depthFormat = (img::E_COLOR_FORMAT)-1;
	if (flags & ERTCF_DEPTH16)
		depthFormat = img::ECF_DEPTH16;
	else if (flags & ERTCF_DEPTH24)
		depthFormat = img::ECF_DEPTH24;
	else if (flags & ERTCF_DEPTH24)
		depthFormat = img::ECF_DEPTH32;

	if ((u32)depthFormat < img::E_COLOR_FORMAT_COUNT)
		m_DepthAttachment = m_Driver.createRenderTargetTexture(m_Size, depthFormat);
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	SAFE_DROP(m_ColorAttachment);
	SAFE_DROP(m_DepthAttachment);
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
