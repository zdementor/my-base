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

	virtual ITexture* getColorTexture() = 0;

	virtual bool bind() = 0;
	virtual bool unbind() = 0;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
