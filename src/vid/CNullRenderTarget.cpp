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
{
	memset(m_ColorAttachements, 0, sizeof(m_ColorAttachements));
}

//----------------------------------------------------------------------------

CNullRenderTarget::~CNullRenderTarget()
{
}

//----------------------------------------------------------------------------

ITexture* CNullRenderTarget::getColorAttachement(u32 attachIndex)
{
	return NULL;
}

//----------------------------------------------------------------------------

u32 CNullRenderTarget::getColorAttachementsCount()
{
	return 0;
}

//----------------------------------------------------------------------------

ITexture* CNullRenderTarget::getDepthAttachement()
{
	return NULL;
}

//----------------------------------------------------------------------------

ITexture* CNullRenderTarget::getStencilAttachement()
{
	return NULL;
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
