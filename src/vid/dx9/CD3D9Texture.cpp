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

//! constructor 1
CD3D9Texture::CD3D9Texture(img::IImage* image, u32 flags) 
	: CNullHardwareTexture(flags), Device(NULL), Texture(0),
	IsRenderTarget(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Texture");    	
#endif

	Device = ((CD3D9Driver*)m_Driver)->_getDirect3DDevice();

    if (Device)
        Device->AddRef();

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

//! constructor 2
CD3D9Texture::CD3D9Texture(core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags) 
	: CNullHardwareTexture(flags), Device(NULL), Texture(0),
	IsRenderTarget(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CD3D9Texture");    	
#endif

	Device = ((CD3D9Driver*)m_Driver)->_getDirect3DDevice();

    if (Device)
        Device->AddRef();

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

//! rendertarget constructor
CD3D9Texture::CD3D9Texture(core::dimension2d<s32> size) 
	: Device(NULL), Texture(0), IsRenderTarget(true)
{
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CD3D9Texture");
#endif

	Device = ((CD3D9Driver*)m_Driver)->_getDirect3DDevice();

	if (Device)
        Device->AddRef();

	m_ImageSize = size;
	m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(size.Width);
	m_TextureSize.Height = core::math::GetNearestPowerOfTwo(size.Height);

    // get backbuffer format to create the render target in the 
    // same format

    IDirect3DSurface9* bb;
    D3DFORMAT d3DFormat;

    if (!FAILED(Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
    {
        D3DSURFACE_DESC desc;
        bb->GetDesc(&desc);
        d3DFormat = desc.Format;
        
        if (d3DFormat == D3DFMT_X8R8G8B8)
            d3DFormat = D3DFMT_A8R8G8B8;

        bb->Release();
    }
    else
    {
        LOGGER.logWarn("Could not create RenderTarget texture: could not get BackBuffer.");
        return;
    }

    // create texture
    HRESULT hr;

    hr = Device->CreateTexture(
        m_TextureSize.Width,
        m_TextureSize.Height,
        1, // mip map level count, we don't want mipmaps here
        D3DUSAGE_RENDERTARGET,
        d3DFormat,
        D3DPOOL_DEFAULT,
        &Texture,
        NULL);

    // get irrlicht format from D3D format

	m_Pitch = 0;

    switch(d3DFormat)
    {
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
        m_ColorFormat = img::ECF_A1R5G5B5;
		m_Pitch = m_TextureSize.Width*2;
        break;
    case D3DFMT_A8B8G8R8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        m_ColorFormat = img::ECF_A8R8G8B8;
		m_Pitch = m_TextureSize.Width*4;
        break;
    case D3DFMT_R5G6B5:
        m_ColorFormat = img::ECF_R5G6B5;
		m_Pitch = m_TextureSize.Width*2;
        break;
    default:
        m_ColorFormat = (img::E_COLOR_FORMAT)-1;
    };

    if (FAILED(hr))
	{
        LOGGER.log("Could not create render target texture");
		return;
	}

	LOGGER.logInfo("Created render target texture ( %s, mips %s, %dx%d )", 
		img::ColorFormatStr[m_ColorFormat],
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);
}

//----------------------------------------------------------------------------

//! destructor
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

//! frees image data of this texture
//! Warning!!! after that methods lock() and unlock()
//! will not be working
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

	if (isRenderTarget())
	{
		IDirect3DSurface9 * pSurface, * pTargetSurface;
		ID3DXBuffer * pBuffer;

		hr = Texture->GetSurfaceLevel(0, &pSurface);
		if (SUCCEEDED(hr))
			hr  = Device->CreateOffscreenPlainSurface(getSize().Width, getSize().Height,
				D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pTargetSurface, NULL);

		if (SUCCEEDED(hr))
			hr = Device->GetRenderTargetData(pSurface, pTargetSurface);
		SAFE_RELEASE(pSurface);

		if (SUCCEEDED(hr))
			hr = D3DXSaveSurfaceToFileInMemory(
				&pBuffer, D3DXIFF_BMP, pTargetSurface, NULL, NULL);
		SAFE_RELEASE(pTargetSurface);

		if (SUCCEEDED(hr))
		{
			io::IReadFile * file = FILE_SYSTEM.createMemoryReadFile(
				pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), "#MemoryFile.bmp", false);
			if (file)
				m_LockImage[level] = IMAGE_LIBRARY.createImageFromFile(file);
			SAFE_DROP(file);
		}
		SAFE_RELEASE(pBuffer);
	}
	else
	{
		hr = Texture->LockRect(level, &rect, 0, 0);
		if (SUCCEEDED(hr))
			m_LockImage[level] = IMAGE_LIBRARY.createImageFromData(
				m_ColorFormat, m_TextureSize,
				m_ImageDataSizeBytes[level], rect.pBits, false);
	}

	if (FAILED(hr) || !m_LockImage)
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

	if (!isRenderTarget())
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

bool CD3D9Texture::isRenderTarget()
{
    return IsRenderTarget;
}

//----------------------------------------------------------------------------

IDirect3DSurface9* CD3D9Texture::getRenderTargetSurface()
{
    if (!IsRenderTarget)
        return 0;

    IDirect3DSurface9 *pRTTSurface = 0;
    if (Texture)
        Texture->GetSurfaceLevel(0, &pRTTSurface);

    if (pRTTSurface)
        pRTTSurface->Release();

    return pRTTSurface;
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

