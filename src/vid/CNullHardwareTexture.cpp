//|-------------------------------------------------------------------------
//| File:        CNullHardwareTexture.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CNullHardwareTexture.h"

#include <io/ILogger.h>
#include <img/IImageLibrary.h>
#include <res/IResourceManager.h>

//----------------------------------------------------------------------------
namespace my { 
namespace vid {
//----------------------------------------------------------------------------

CNullHardwareTexture::CNullHardwareTexture() 
	: m_Driver((CNullDriver*)VIDEO_DRIVER_PTR),
	m_TextureSize(0,0), m_ImageSize(0,0),
	m_Pitch(0), m_ColorFormat(img::ECF_A8R8G8B8), m_MaxMipMapLevels(1),
	m_AutogenMipMaps(false), m_IsCompressed(false),
	m_BytesPerPixel(0), m_Type(ETT_COLOR_MAP), m_Format(ETF_TEXTURE_2D)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CNullHardwareTexture"); 
#endif
	memset(m_LockImage,
		0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_LockImage));
	memset(m_ImageDataSizeBytes,
		0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageDataSizeBytes));
}

//----------------------------------------------------------------------------

CNullHardwareTexture::CNullHardwareTexture(u32 flags)
	: m_Driver((CNullDriver*)VIDEO_DRIVER_PTR),
	m_CreationFlags(flags), m_TextureSize(0,0), m_ImageSize(0,0),
	m_Pitch(0), m_ColorFormat(img::ECF_A8R8G8B8), m_MaxMipMapLevels(1),
	m_AutogenMipMaps(false), m_IsCompressed(false),
	m_BytesPerPixel(0), m_Type(ETT_COLOR_MAP), m_Format(ETF_TEXTURE_2D)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CNullHardwareTexture");    
#endif
	memset(m_LockImage,
		0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_LockImage));
	memset(m_ImageDataSizeBytes,
		0, MY_TEXTURE_MAX_MIP_LEVELS * sizeof(*m_ImageDataSizeBytes));
}

//----------------------------------------------------------------------------

CNullHardwareTexture::~CNullHardwareTexture()
{
	clear();
}

//----------------------------------------------------------------------------

void CNullHardwareTexture::clear()
{
	for (u32 i = 0; i < MY_TEXTURE_MAX_MIP_LEVELS; i++)
		SAFE_DROP(m_LockImage[i]);
}

//----------------------------------------------------------------------------

bool CNullHardwareTexture::createEmptyTexture(
	const core::dimension2di &size, img::E_COLOR_FORMAT format, bool renderTarget)
{
	if (!m_Driver->queryFeature(EVDF_COMPRESSED_TEXTURES) && (
			format == img::ECF_DXT1 ||
			format == img::ECF_DXT3 ||
			format == img::ECF_DXT5))
	{
		LOGGER.logErr("Unsupported data format %s by your hardware.",
			img::getColorFormatName(m_ColorFormat));
		return false;
	}

	m_TextureSize = m_ImageSize = size;

	if (!renderTarget)
	{
		if ((m_CreationFlags & ETCF_CREATE_POWER_OF_TWO))
		{
			m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(m_ImageSize.Width);
			m_TextureSize.Height = core::math::GetNearestPowerOfTwo(m_ImageSize.Height);
		}
		m_AutogenMipMaps = (m_CreationFlags & ETCF_AUTOGEN_MIP_MAPS)!=0;
	}
	else
	{
		m_AutogenMipMaps = false;
	}

	m_ColorFormat = format;
	m_IsCompressed = false;
	m_MaxMipMapLevels = 1;
	m_BytesPerPixel = img::getBitsPerPixelFromFormat(format) / 8;
	if (m_ColorFormat == img::ECF_R8G8B8 && (
			VIDEO_DRIVER.getDriverFamily() == EDF_DIRECTX ||
			m_TextureSize != m_ImageSize))
	{
		m_ColorFormat = img::ECF_R5G6B5;
		m_BytesPerPixel = 2;
	}
	m_Pitch = m_TextureSize.Width * m_BytesPerPixel;
	for (u32 level = 0; level < getMaxMipMapLevels(); level++)
		m_ImageDataSizeBytes[level] = (m_Pitch * m_TextureSize.Height);

	bool res = createHardwareTexture(renderTarget);

	if (!renderTarget)
	{
		for (u32 level = 0; res && level < m_MaxMipMapLevels; level++)
		{
			img::IImage* tex_image = lock(level);
			if (!tex_image)
			{
				res = false;
				break;
			}
			s32 *dest = (s32*)tex_image->getLevelData(level);
			memset(dest, 0, tex_image->getLevelDataSizeBytes(level));
			unlock(level);
		}
	}

    return res;
}

//----------------------------------------------------------------------------

bool CNullHardwareTexture::createTextureFrom(img::IImage *image)
{
	if (!image)
	{
		LOGGER.logErr("Cant create hardware texture from NULL image");
		return false;
	}

	if (!m_Driver->queryFeature(EVDF_COMPRESSED_TEXTURES) && (
			m_ColorFormat == img::ECF_DXT1 ||
			m_ColorFormat == img::ECF_DXT3 ||
			m_ColorFormat == img::ECF_DXT5))
	{
		LOGGER.logErr("Unsupported data format %s by your hardware.",
			img::getColorFormatName(m_ColorFormat));
		return false;
	}

	m_TextureSize = m_ImageSize = image->getDimension();

	if ((m_CreationFlags & ETCF_CREATE_POWER_OF_TWO))
	{
		m_TextureSize.Width  = core::math::GetNearestPowerOfTwo(m_ImageSize.Width);
		m_TextureSize.Height = core::math::GetNearestPowerOfTwo(m_ImageSize.Height);
	}

	m_AutogenMipMaps = (m_CreationFlags & ETCF_AUTOGEN_MIP_MAPS)!=0;
	m_MaxMipMapLevels = 1;
	if (!m_AutogenMipMaps)
	{
		m_MaxMipMapLevels = image->getLevelCount();
		CHECK_RANGE(m_MaxMipMapLevels, 1, MY_TEXTURE_MAX_MIP_LEVELS);
	}

	if ((m_CreationFlags & ETCF_CREATE_COLOR_KEY) != 0)
	{
		if (image->getColorFormat() != img::ECF_A1R5G5B5 &&
			image->getColorFormat() != img::ECF_A8R8G8B8 &&
			image->getColorFormat() != img::ECF_DXT1 &&
			image->getColorFormat() != img::ECF_DXT3 &&
			image->getColorFormat() != img::ECF_DXT5)
		{
			if (image->getColorFormat() == img::ECF_R5G6B5)
				image->convertTo(img::ECF_A1R5G5B5);
			else			 
				image->convertTo(img::ECF_A8R8G8B8);
			image->makeColorKey(core::position2di(0,0));
		}     
	}
	if ((m_CreationFlags & ETCF_CREATE_NORMAL_MAP) != 0)
	{
		if (image->getColorFormat() != img::ECF_A8R8G8B8)
			image->convertTo(img::ECF_A8R8G8B8);

		f32 amplitude = IVideoDriver::getSingleton().getNormalMapCreationAmplitude();

		image->makeNormalMap(amplitude);    
		
		core::stringc img_full_fname = 
			RESOURCE_MANAGER.getMediaDirFull( res::EMT_TEMP_DATA );		
		core::stringc img_tmp_fname = 			
			IMAGE_LIBRARY.findImageFileName(image);
		core::stringc img_fname = img_tmp_fname;
		img_fname.make_lower();

		s32 c = img_fname.find_last('.');

		if (c>5 &&
			img_tmp_fname[c-5]=='_'&&
			img_tmp_fname[c-4]=='h'&&
			img_tmp_fname[c-3]=='m'&&
			img_tmp_fname[c-2]=='a'&&
			img_tmp_fname[c-1]=='p'
			)
		{
			if (c>7 && (
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='0')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='1')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='2')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='3')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='4')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='5')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='6')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='7')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='8')||
					(img_tmp_fname[c-7]=='_'&&img_tmp_fname[c-6]=='9')
					)
				)
			{
				img_fname = img_tmp_fname.sub_string(0,c-7);
			}
			else
			{
				img_fname = img_tmp_fname.sub_string(0,c-5);
			}
		}

		img_fname.append("_nmap.tga");

		img_full_fname.append(img_fname);
		
		IMAGE_LIBRARY.saveImageToFile(
			image, img_full_fname.c_str());

		image->setOverridedFileName(img_full_fname.c_str());
	}

	m_ColorFormat = image->getColorFormat();
	m_BytesPerPixel = image->getBytesPerPixel();
	m_IsCompressed = image->isCompressed();

	if (m_ColorFormat == img::ECF_R8G8B8 && (
			VIDEO_DRIVER.getDriverFamily() == EDF_DIRECTX ||
			m_TextureSize != m_ImageSize))
	{
		m_ColorFormat = img::ECF_R5G6B5;
		m_BytesPerPixel = 2;
	}

	m_Pitch = m_TextureSize.Width * m_BytesPerPixel;

	for (u32 level = 0; level < getMaxMipMapLevels(); level++)
	{
		u32 sz = m_Pitch * m_TextureSize.Height;
		u32 imgSz = image->getLevelDataSizeBytes(level);

		m_ImageDataSizeBytes[level] = image->isCompressed() ? imgSz : sz;
	}

	bool res = createHardwareTexture(false);

	if (res)
	{
		if (m_TextureSize == image->getDimension())
		{
			for (u32 level = 0; res && level < m_MaxMipMapLevels; level++)
				res = createTextureLevel(level, image->getData(), image->getLevelDataSizeBytes(level), image->getColorFormat());
		}
		else
			res = false;
		if (!res)
			res = copyTextureFrom(image);
	}

	return res;
}

//----------------------------------------------------------------------------

bool CNullHardwareTexture::createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format)
{
	if (!m_Driver->queryFeature(EVDF_COMPRESSED_TEXTURES) && (
			format == img::ECF_DXT1 ||
			format == img::ECF_DXT3 ||
			format == img::ECF_DXT5))
	{
		LOGGER.logErr("Unsupported data format %s by your hardware.",
			img::getColorFormatName(m_ColorFormat));
		return false;
	}

	if (format != m_ColorFormat)
	{
		// Normal case
		return false;
	}

	if (data && dataSize != m_ImageDataSizeBytes[level])
	{
		LOGGER.logErr("Could not create texture from incorrect data size (req=%d, actual=%d)",
			m_ImageDataSizeBytes[level], dataSize);
		return false;
	}

	if (level >= getMaxMipMapLevels())
	{
		LOGGER.logErr("Could not create texture (level %d) > maximal (%d)", level, getMaxMipMapLevels());
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------

bool CNullHardwareTexture::copyTextureFrom(img::IImage *image)
{
	if (!image)
		return false;

	img::IImage* tex_image = lock();
	if (!tex_image)
		return false;

	IMAGE_LIBRARY.copy(image, tex_image);
	unlock();

    return true;
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------


