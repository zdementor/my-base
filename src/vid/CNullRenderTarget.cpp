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

//----------------------------------------------------------------------------
namespace my {
namespace vid {  
//----------------------------------------------------------------------------

CNullRenderTarget::CNullRenderTarget(
	const core::dimension2di &size, E_RENDER_TARGET_CREATION_FLAG flags)
	: m_ColorAttachementsCount(0), m_DepthAttachement(0),
	m_Size(size), m_Flags(flags)
{
	memset(m_ColorAttachements, 0, sizeof(m_ColorAttachements));
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
