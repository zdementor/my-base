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

CNullRenderTarget::CNullRenderTarget()
	: m_ColorAttachementsCount(0), m_DepthAttachement(0)
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
