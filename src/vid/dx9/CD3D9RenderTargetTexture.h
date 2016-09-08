//|-------------------------------------------------------------------------
//| File:        CD3D9Texture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CD3D9RenderTargetTextureHPP
#define CD3D9RenderTargetTextureHPP
//----------------------------------------------------------------------------

#include "CD3D9Texture.h" 

#include <d3d/d3d9.h>

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

class CD3D9RenderTargetTexture : public CD3D9Texture
{
public:

    CD3D9RenderTargetTexture(
		const core::dimension2di &size, img::E_COLOR_FORMAT colorFormat);
    virtual ~CD3D9RenderTargetTexture();

	// interface CD3D9Texture

	virtual img::IImage* lock(u32 level);
	virtual void unlock(u32 level);

	virtual bool isRenderTarget() { return true; }

	// interface CD3D9RenderTargetTexture

	IDirect3DSurface9* getRenderTargetSurface();

private:
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CD3D9RenderTargetTextureHPP

