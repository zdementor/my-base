 //|-------------------------------------------------------------------------
//| File:        CD3D9RenderTarget.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2013 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9RenderTargetHPP
#define CD3D9RenderTargetHPP
//---------------------------------------------------------------------------

#include "../CNullRenderTarget.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ CD3D9RenderTarget : public CNullRenderTarget
{
public:
	CD3D9RenderTarget(const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat,
		E_RENDER_TARGET_CREATION_FLAG flags);
	CD3D9RenderTarget(ITexture *colorRenderTarget, E_RENDER_TARGET_CREATION_FLAG flags);
	virtual ~CD3D9RenderTarget();
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
  