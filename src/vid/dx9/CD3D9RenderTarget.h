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
#include "CD3D9Driver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class __MY_VID_LIB_API__ CD3D9RenderTarget : public CNullRenderTarget
{
public:
	CD3D9RenderTarget(const core::dimension2di &size,
		img::E_COLOR_FORMAT colorFormat, E_RENDER_TARGET_DEPTH_FORMAT depthFormat);
	virtual ~CD3D9RenderTarget();

	virtual bool bind();
	virtual bool unbind();

private:
	bool _rebuild();
	void _bindMainRT();

	CD3D9Driver *m_D3DDriver;

	IDirect3DSurface9* m_D3DRenderTargetSurface;
	IDirect3DSurface9* m_D3DDepthStencilSurface;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif
  