//|-------------------------------------------------------------------------
//| File:        COpenGLTexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "COpenGLTexture.h"
#include "COpenGLDriver.h"

#include <io/ILogger.h>
#include <vid/IVideoDriver.h>
#include <img/IImageLibrary.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

#include "OGLHelper.h"

//----------------------------------------------------------------------------

COpenGLTexture::COpenGLTexture(u32 flags) 
	: CNullHardwareTexture(flags), m_Created(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLTexture");
#endif
	m_TextureName.v = NULL;
	memset(m_ImageData, 0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageData));
}

//----------------------------------------------------------------------------

COpenGLTexture::COpenGLTexture(img::IImage* image, u32 flags) 
	: CNullHardwareTexture(flags), m_Created(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLTexture");
#endif

	m_TextureName.v = NULL;
	memset(m_ImageData, 0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageData));

	if (!createTextureFrom(image))
	{
		LOGGER.logErr("Cant create OGL hardware texture"); 
		return;
	}

	freeImageData();
	
	LOGGER.logInfo("Created texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);

	m_Created = true;
}

//----------------------------------------------------------------------------

COpenGLTexture::COpenGLTexture(core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags) 
	: CNullHardwareTexture(flags), m_Created(false)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("COpenGLTexture");
#endif
	m_TextureName.v = NULL;
	memset(m_ImageData, 0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageData));

	if (!createEmptyTexture(size, format))
	{
		LOGGER.logErr("Cant create empty OGL hardware texture"); 
		return;
	}

	freeImageData();
	
	LOGGER.logInfo("Created empty texture ( %s, mips %s, %dx%d )", 
		img::getColorFormatName(m_ColorFormat),
		hasMipMaps() ? "on" : "off",
		getSize().Width, getSize().Height);

	m_Created = true;
}

//----------------------------------------------------------------------------

COpenGLTexture::~COpenGLTexture()
{
	clear();
}

//----------------------------------------------------------------------------

void COpenGLTexture::clear()
{
	if (m_TextureName.u)
	{
		glDeleteTextures(1, &m_TextureName.u);
		m_TextureName.u = 0;
	}
	freeImageData();
}

//----------------------------------------------------------------------------

bool COpenGLTexture::freeImageData()
{
	for (u32 i = 0; i < MY_TEXTURE_MAX_MIP_LEVELS; i++)
		SAFE_DELETE_ARRAY(m_ImageData[i]);
	return true;  
}

//----------------------------------------------------------------------------

bool COpenGLTexture::createHardwareTexture(bool renderTarget)
{
	switch(m_ColorFormat)
    {
#ifdef GL_VERSION_1_2
	case img::ECF_A1R5G5B5:
		m_InternalFormat= GL_RGB5_A1;
		m_PixelFormat	= GL_BGRA;
		m_PixelType		= GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
		m_AutogenMipMaps= false;
    case img::ECF_R5G6B5:
		m_InternalFormat= GL_RGB5;
		m_PixelFormat	= GL_RGB;
		m_PixelType		= GL_UNSIGNED_SHORT_5_6_5;
		m_AutogenMipMaps= false;
		break;
#endif
    case img::ECF_A8R8G8B8:
		m_InternalFormat= GL_RGBA8;
		m_PixelFormat	= GL_BGRA;
		m_PixelType		= GL_UNSIGNED_BYTE;
		break;
    case img::ECF_A8B8G8R8:
		m_InternalFormat= GL_RGBA8;
		m_PixelFormat	= GL_RGBA;
		m_PixelType		= GL_UNSIGNED_BYTE;
		break;
    case img::ECF_R8G8B8:
		m_InternalFormat= GL_RGB8;
		m_PixelFormat	= GL_RGB;
		m_PixelType		= GL_UNSIGNED_BYTE;
		break;
#ifdef GL_EXT_texture_compression_s3tc
    case img::ECF_DXT1:
		m_InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		m_PixelFormat	= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		m_PixelType		= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case img::ECF_DXT3:
		m_InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		m_PixelFormat	= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		m_PixelType		= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case img::ECF_DXT5:
		m_InternalFormat= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		m_PixelFormat	= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		m_PixelType		= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
#endif
	case img::ECF_LUMINANCE16:
		m_InternalFormat= GL_LUMINANCE16;
		m_PixelFormat	= GL_LUMINANCE;
		m_PixelType		= GL_UNSIGNED_SHORT;
		break;
	case img::ECF_A16B16G16R16:
		m_InternalFormat= GL_RGBA16;
		m_PixelFormat	= GL_RGBA;
		m_PixelType		= GL_UNSIGNED_SHORT;
		break;
#ifdef GL_ARB_framebuffer_object
	case img::ECF_DEPTH24_STENCIL8:
		m_InternalFormat= GL_DEPTH24_STENCIL8;
		m_PixelFormat	= GL_DEPTH_STENCIL;
		m_PixelType		= GL_UNSIGNED_INT_24_8;
		break;
#endif
#ifdef GL_ARB_texture_float
	case img::ECF_LUMINANCE32F:
		m_InternalFormat= GL_LUMINANCE32F_ARB;
		m_PixelFormat	= GL_LUMINANCE;
		m_PixelType		= GL_FLOAT;
		break;
#ifdef GL_ARB_texture_float
	case img::ECF_A16B16G16R16F:
		m_InternalFormat= GL_RGBA16F_ARB;
		m_PixelFormat	= GL_RGBA;
		m_PixelType		= GL_HALF_FLOAT_ARB;
		break;
#endif
	case img::ECF_A32B32G32R32F:
		m_InternalFormat= GL_RGBA32F_ARB;
		m_PixelFormat	= GL_RGBA;
		m_PixelType		= GL_FLOAT;
		break;
#endif
	default:
		LOGGER.logErr("Unsupported OGL hardware texture format %s.",
			img::getColorFormatName(m_ColorFormat));
		return false;
	}

	glGenTextures(1, &m_TextureName.u);

	return m_TextureName.v != NULL;
}

//----------------------------------------------------------------------------

bool COpenGLTexture::createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format)
{
	if (!CNullHardwareTexture::createTextureLevel(level, data, dataSize, format))
		return false;

	if (!m_TextureName.u)
	{
		LOGGER.logErr("Could create OGL Texture, zero texture!");
		return NULL;
	}

	testGLError();

	// recreate texture
	
	ITexture *curtex0 = m_Driver->_getCurrentTexture(0);
	if (this != curtex0)
		m_Driver->_setTexture(0, this);

#ifdef GL_SGIS_generate_mipmap
	if (m_AutogenMipMaps && GLEW_SGIS_generate_mipmap)
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, TRUE);
#endif

	if (isCompressed()) 
	{
#ifdef GL_VERSION_1_3
		glCompressedTexImage2D(
			GL_TEXTURE_2D, level, m_InternalFormat, 
			m_TextureSize.Width, m_TextureSize.Height, 0, 
			dataSize, data);
#else
		LOGGER.logErr("Unsupported OGL Compressed Texture");
#endif
	}
	else
		glTexImage2D(
			GL_TEXTURE_2D, level, m_InternalFormat, 
			m_TextureSize.Width, m_TextureSize.Height, 0, 
			m_PixelFormat, m_PixelType, data);

	if (testGLError())
		LOGGER.logErr("Could not create OGL Texture");
	else
	if (level == 0)
		_regenerateMipMapLevels((const u8*)data);

	if (this != curtex0)
		m_Driver->_setTexture(0, curtex0);

	return true;
}

//----------------------------------------------------------------------------

img::IImage* COpenGLTexture::lock(u32 level)
{
	if (!m_TextureName.u
			|| level >= getMaxMipMapLevels()
			|| m_LockImage[level])
	{
		LOGGER.logErr("Could not lock, OGL Texture (level %d)", level);
		return NULL;
	}

	if (!m_ImageData[level])
	{
		m_ImageData[level] = new u8[m_ImageDataSizeBytes[level]];
	}

	if (m_Created)
	{
		ITexture *curtex0 = m_Driver->_getCurrentTexture(0);
		if (this != curtex0)
			m_Driver->_setTexture(0, this);
#ifdef GL_VERSION_1_3
		if (isCompressed())
			glGetCompressedTexImage(GL_TEXTURE_2D, level,
				m_ImageData[level]);
		else
#endif
			glGetTexImage(GL_TEXTURE_2D, level,
				m_PixelFormat, m_PixelType, m_ImageData[level]);			
		if (this != curtex0)
			m_Driver->_setTexture(0, curtex0);
	}

	m_LockImage[level] = IMAGE_LIBRARY.createImageFromData(
		m_ColorFormat, m_TextureSize,
		m_ImageDataSizeBytes[level], m_ImageData[level], 
		false);

	return m_LockImage[level];
}

//----------------------------------------------------------------------------

void COpenGLTexture::unlock(u32 level)
{
	if (!m_TextureName.u
			|| level >= getMaxMipMapLevels()
			|| !m_LockImage[level])
	{
		LOGGER.logErr("Could not unlock, OGL Texture (level %d)", level);
		return;
	}

	SAFE_DROP(m_LockImage[level]);

	createTextureLevel(level,  m_ImageData[level], m_ImageDataSizeBytes[level], m_ColorFormat);

	if (isRenderTarget())
		SAFE_DELETE_ARRAY(m_ImageData[level]);
}

//----------------------------------------------------------------------------

void COpenGLTexture::regenerateMipMapLevels()
{
	ITexture *curtex0 = m_Driver->_getCurrentTexture(0);
	if (this != curtex0)
		m_Driver->_setTexture(0, this);
	_regenerateMipMapLevels(m_ImageData[0]);
	if (this != curtex0)
		m_Driver->_setTexture(0, curtex0);
}

//----------------------------------------------------------------------------

void COpenGLTexture::_regenerateMipMapLevels(const u8 *imageData)
{
	if (m_AutogenMipMaps
#ifdef GL_SGIS_generate_mipmap
			&& !GLEW_SGIS_generate_mipmap
#endif
			)
	{
		bool mips = false;
		do
		{
			if (isCompressed())
			{
				LOGGER.logErr("Could not create mip maps for compressed OGL Texture.");
				break;
			}
			s32 ret = gluBuild2DMipmaps(
				GL_TEXTURE_2D, m_InternalFormat, 
				m_ImageSize.Width, m_ImageSize.Height, m_PixelFormat, m_PixelType,
				imageData);
			if (ret)
			{
				LOGGER.logErr("Could not create OGL Texture mip maps: %s",
					(c8*)gluErrorString(ret));
				break;
			}
			mips = true;
		}
		while (0);
		if (!mips)
		{
			LOGGER.logErr("Disabling mip maps for this OGL Texture.");
			m_AutogenMipMaps = false;
		}
	}
	if (hasMipMaps())
	{
		// enable bilinear filter with mipmaps
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
	else
	{
		// enable bilinear filter without mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (testGLError())
		LOGGER.logErr("Could not regenerate OGL Texture mip map levels");
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------
