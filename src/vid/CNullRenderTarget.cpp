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

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(
	const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
	E_RENDER_TARGET_CREATION_FLAG flags)
	: m_ColorAttachementsCount(0), m_DepthAttachement(0),
	m_Size(size), m_ColorFormat(colorFormat), m_Flags(flags), m_RTEntry(0)
{
	memset(m_ColorAttachements, 0, sizeof(m_ColorAttachements));
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
	for (u32 i = 0; i < sizeof(m_ColorAttachements) / sizeof(*m_ColorAttachements); i++)
		SAFE_DROP(m_ColorAttachements[i]);
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
