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

CD3D9Texture::CD3D9Texture() 
	: CNullHardwareTexture()
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
		img::ColorFormatStr[m_ColorFormat],
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
		img::ColorFormatStr[m_ColorFormat],
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

bool CD3D9Texture::createHardwareTexture()
{
	D3DFORMAT format; 

	switch(m_ColorFormat)
    {
    case img::ECF_A1R5G5B5:
    case img::ECF_R5G6B5:
    case img::ECF_A8R8G8B8:
    case img::ECF_A8B8G8R8:
    case img::ECF_R8G8B8:
	case img::ECF_DXT1:
	case img::ECF_DXT3:
	case img::ECF_DXT5:
		break;
	default:
		LOGGER.logErr("Unsupported data format %s.",
			img::ColorFormatStr[m_ColorFormat]);
		return false;
	}

	if (m_ColorFormat == img::ECF_A8R8G8B8)
    {
		format = D3DFMT_A8R8G8B8; 
    }
	else
	if (m_ColorFormat == img::ECF_R8G8B8)
    {
		format = D3DFMT_R8G8B8; 
    }
	else
	if (m_ColorFormat == img::ECF_A1R5G5B5)
    {
		format = D3DFMT_A1R5G5B5; 
    }	
	else
	if (m_ColorFormat == img::ECF_R5G6B5)
    {
		format = D3DFMT_R5G6B5; 
    }

	else
	if (m_ColorFormat == img::ECF_DXT1)
    {
		format = D3DFMT_DXT1; 
    }
	else
	if (m_ColorFormat == img::ECF_DXT3)
    {
		format = D3DFMT_DXT3; 
    }
	else
	if (m_ColorFormat == img::ECF_DXT5)
    {
		format = D3DFMT_DXT5; 
    }
	else
	{
		LOGGER.log("Unsupported D3D hardware texture format", io::ELL_ERROR);

		return false;
	}

	HRESULT hr = Device->CreateTexture(
		m_TextureSize.Width, m_TextureSize.Height,
        m_AutogenMipMaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
        0, format, D3DPOOL_MANAGED, &Texture, NULL);

    if (FAILED(hr))
		return false;

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

