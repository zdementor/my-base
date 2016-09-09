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
#ifndef CD3D9TextureHPP
#define CD3D9TextureHPP
//----------------------------------------------------------------------------

#include "../CNullHardwareTexture.h" 

#include <d3d/d3d9.h>

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

class CD3D9Texture : public CNullHardwareTexture
{
public:

	CD3D9Texture(img::IImage* image, u32 flags);
	CD3D9Texture(core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags);
    virtual ~CD3D9Texture();

	// interface CNullHardwareTexture

	virtual void clear();

    virtual img::IImage* lock(u32 level);
    virtual void unlock(u32 level);

	virtual bool freeImageData();

    virtual void regenerateMipMapLevels();

	virtual void* getHardwareTexture() { return (void*)Texture; }

protected:

	CD3D9Texture();

	virtual bool createHardwareTexture(bool renderTarget);

	virtual bool createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format);

	void _init();

    IDirect3DDevice9* Device;
    IDirect3DTexture9* Texture;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif 

