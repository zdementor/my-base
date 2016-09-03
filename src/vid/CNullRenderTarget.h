//|-------------------------------------------------------------------------
//| File:        CNullRenderTarget.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CNullRenderTargetHPP
#define CNullRenderTargetHPP
//---------------------------------------------------------------------------

#include <vid/IRenderTarget.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

#define MY_VID_MAX_COLOR_ATTACHEMENTS 4

class __MY_VID_LIB_API__ CNullRenderTarget : public IRenderTarget
{
public:
	CNullRenderTarget();
	virtual ~CNullRenderTarget();

	virtual ITexture* getColorAttachement(u32 attachIndex);

	virtual u32 getColorAttachementsCount();

	virtual ITexture* getDepthAttachement();
	virtual ITexture* getStencilAttachement();
private:
	ITexture *m_ColorAttachements[MY_VID_MAX_COLOR_ATTACHEMENTS];
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
