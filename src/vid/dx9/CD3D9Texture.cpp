//|-------------------------------------------------------------------------
//| File:        CD3D9Texture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CD3D9Texture.h"
#include "CD3D9Driver.h"

#include <io/ILogger.h>
#include <vid/IVideoDriver.h>
#include <img/IImageLibrary.h>
#include <io/IFileSystem.h>

#include <d3d/d3dx9tex.h>

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

CD3D9Texture::CD3D9Texture(u32 flags) 
	: CNullHardwareTexture(flags)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Texture");    	
#endif
	_init();
}

//----------------------------------------------------------------------------

CD3D9Texture::CD3D9Texture(img::IImage* image, u32 flags) 
	: CNullHardwareTexture(flags)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Texture");    	
#endif
	_init();

    if (!createTextureFrom(image))
	{
		LOGGER.log("Cant create D3D9 hardware texture", io::ELL_ERROR);  		
		return;
	}  

	LOGGER.logInfo("Created texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------

CD3D9Texture::CD3D9Texture(core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags) 
	: CNullHardwareTexture(flags)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Texture");    	
#endif
	_init();

    if (!createEmptyTexture(size, format))
	{
		LOGGER.logErr("Cant create empty D3D9 hardware texture");  		
		return;
	}  
    
	LOGGER.logInfo("Created empty texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------

void CD3D9Texture::_init()
{
	Texture = NULL;

	Device = ((CD3D9Driver*)m_Driver)->_getDirect3DDevice();
    if (Device)
        Device->AddRef();
}

//----------------------------------------------------------------------------

CD3D9Texture::~CD3D9Texture()
{
    clear();
}

//----------------------------------------------------------------------------

void CD3D9Texture::clear()
{
	if (Device)
	{
        Device->Release();
		Device = NULL;
	}
    if (Texture)
	{
        Texture->Release();
		Texture = NULL;
	}
	freeImageData();
}

//----------------------------------------------------------------------------

bool CD3D9Texture::freeImageData()
{
	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Texture::createHardwareTexture(bool renderTarget)
{
	DWORD usage = 0;
	D3DPOOL pool = renderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	D3DFORMAT format;

	if (renderTarget)
		usage = D3DUSAGE_RENDERTARGET;

	switch (m_ColorFormat)
	{
	case img::ECF_A8R8G8B8:
	case img::ECF_A8B8G8R8:
		format = D3DFMT_A8R8G8B8;
		break;
	case img::ECF_R8G8B8:
		format = D3DFMT_R8G8B8; 
		break;
	case img::ECF_A1R5G5B5:
		format = D3DFMT_A1R5G5B5; 
		break;
	case img::ECF_R5G6B5:
		format = D3DFMT_R5G6B5; 
		break;
	case img::ECF_DXT1:
		format = D3DFMT_DXT1; 
		break;
	case img::ECF_DXT3:
		format = D3DFMT_DXT3; 
		break;
	case img::ECF_DXT5:
		format = D3DFMT_DXT5; 
		break;
	case img::ECF_LUMINANCE16:
		format = D3DFMT_L16;
		break;
	case img::ECF_A16B16G16R16:
		format = D3DFMT_A16B16G16R16;
		break;
	case img::ECF_DEPTH24_STENCIL8:
		format = FOURCC_INTZ;
		usage = D3DUSAGE_DEPTHSTENCIL;
		break;
	case img::ECF_LUMINANCE32F:
		format = D3DFMT_R32F;
		break;
	case img::ECF_A16B16G16R16F:
		format = D3DFMT_A16B16G16R16F;
		break;
	case img::ECF_A32B32G32R32F:
		format = D3DFMT_A32B32G32R32F;
		break;
	default:
		LOGGER.logErr("Unsupported D3D hardware texture format %s.",
			img::getColorFormatName(m_ColorFormat));
		return false;
    }

	HRESULT hr;
	
	hr = Device->CreateTexture(
		m_TextureSize.Width, m_TextureSize.Height,
		m_AutogenMipMaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		usage, format, pool, &Texture, NULL);

	if (FAILED(hr))
	{
	    LOGGER.logWarn("Could not create D3D hardware texture %s (%d).",
			img::getColorFormatName(m_ColorFormat),
			format);
		Texture = 0;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------

bool CD3D9Texture::createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format)
{
	if (!CNullHardwareTexture::createTextureLevel(level, data, dataSize, format))
		return false;

	bool lockHere = false;
	if (!m_LockImage[level])
	{
		lock(level);
		if (!m_LockImage[level])
		{
			LOGGER.logErr("Could create D3D Texture, locked zero image!");
			return false;
		}
		lockHere = true;
	}
	if (!Texture)
	{
		LOGGER.logErr("Could create D3D Texture, zero texture!");
		return false;
	}

	void *dest = (void*)m_LockImage[level]->getLevelData(level);

	memcpy(dest, data, dataSize);

	if (lockHere)
		unlock(level);

	return true;
}

//----------------------------------------------------------------------------

img::IImage* CD3D9Texture::lock(u32 level)
{
	if (!Texture || level >= getMaxMipMapLevels() ||
			m_LockImage[level])
	{
		LOGGER.logErr("Could not lock, D3D Texture (level %d)", level);
		return NULL;
	}

	D3DLOCKED_RECT rect;
	HRESULT hr = 0;
	
	hr = Texture->LockRect(level, &rect, 0, 0);
	if (SUCCEEDED(hr))
		m_LockImage[level] = IMAGE_LIBRARY.createImageFromData(
			m_ColorFormat, m_TextureSize,
			m_ImageDataSizeBytes[level], rect.pBits, false);

	if (FAILED(hr) || !m_LockImage[level])
	{
		LOGGER.logErr("Could not lock D3D Texture.");
 		return NULL;
	}

	return m_LockImage[level];
}

//----------------------------------------------------------------------------

void CD3D9Texture::unlock(u32 level)
{
	if (!Texture || level >= getMaxMipMapLevels() ||
			!m_LockImage[level])
	{
		LOGGER.logErr("Could not unlock D3D Texture (level %d)", level);
		return;
	}

	SAFE_DROP(m_LockImage[level]);

	Texture->UnlockRect(level);

	regenerateMipMapLevels();
}

//----------------------------------------------------------------------------

void CD3D9Texture::regenerateMipMapLevels()
{
    if (m_AutogenMipMaps)
	{
        // create mip maps.
		HRESULT hr  = D3DXFilterTexture(Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT);
		if (FAILED(hr))
		{
			m_AutogenMipMaps = false;
			LOGGER.logWarn("Could not create D3D Texture mip map levels.");
		}
	}
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

