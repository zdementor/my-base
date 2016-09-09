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
	: m_ColorAttachement(0), m_DepthAttachement(0),
	m_Size(size), m_ColorFormat(colorFormat), m_Flags(flags), m_RTEntry(0)
{
	m_ColorAttachement = VIDEO_DRIVER.createRenderTargetTexture(m_Size, m_ColorFormat);

	img::E_COLOR_FORMAT depthFormat = (img::E_COLOR_FORMAT)-1;
	if (flags & ERTCF_DEPTH16)
		depthFormat = img::ECF_DEPTH16;
	else if (flags & ERTCF_DEPTH24)
		depthFormat = img::ECF_DEPTH24;
	else if (flags & ERTCF_DEPTH24)
		depthFormat = img::ECF_DEPTH32;

	if ((u32)depthFormat < img::E_COLOR_FORMAT_COUNT)
		m_DepthAttachement = VIDEO_DRIVER.createRenderTargetTexture(m_Size, depthFormat);
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	SAFE_DROP(m_ColorAttachement);
	SAFE_DROP(m_DepthAttachement);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
