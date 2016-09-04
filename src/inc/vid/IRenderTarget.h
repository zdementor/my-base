//|-------------------------------------------------------------------------
//| File:        IRenderTarget.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IRenderTargetHPP
#define IRenderTargetHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! Class for hardware render target interface,
//! which may consists from color/depth/stencil buffers
class __MY_VID_LIB_API__ IRenderTarget : public IUnknown
{
public:

	virtual bool attachColorTexture(
		u32 attachIndex, ITexture *colorTex) = 0;
	virtual bool attachColorTexture(
		u32 attachIndex, core::dimension2di &size, img::E_COLOR_FORMAT format) = 0;
	virtual ITexture* getColorAttachement(u32 attachIndex = 0) = 0;
	virtual u32 getColorAttachementsCount() = 0;

	virtual bool attachDepthTexture(
		ITexture *depthTex) = 0;
	virtual bool attachDepthTexture(
		core::dimension2di &size, img::E_COLOR_FORMAT format) = 0;
	virtual ITexture* getDepthAttachement() = 0;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
