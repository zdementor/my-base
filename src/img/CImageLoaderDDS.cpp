//|-------------------------------------------------------------------------
//| File:        CImageLoaderDDS.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImageLoaderDDS.h"
#include "CColorConverter.h"
#include "CImageDDS.h"
#include <img/SColor.h>
#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

CImageLoaderDDS::CImageLoaderDDS()
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImageLoaderDDS");
#endif
}

//-----------------------------------------------------------------------------

CImageLoaderDDS::~CImageLoaderDDS()
{
}

//-----------------------------------------------------------------------------

bool CImageLoaderDDS::isALoadableFileExtension(const c8* fileName)
{
    return strstr(fileName, ".dds") != 0;
}

//-----------------------------------------------------------------------------

bool CImageLoaderDDS::isALoadableFileFormat(io::IReadFile* file)
{
	u32 headerID;
    file->read(&headerID, sizeof(u32));
    return headerID == 0x20534444; // "DDS "
}

//-----------------------------------------------------------------------------

IImage* CImageLoaderDDS::loadImage(io::IReadFile* file)
{
	CImageDDS *image = NULL;
	img::E_COLOR_FORMAT format;
	core::dimension2di dim;
	s32 depth;
	bool btcCompressed = false;
	s32 bytesPerElement = 0;

	if (!isALoadableFileFormat(file))
	{
		LOGGER.logErr("Incorrect header of the DDS file.");
		goto fail;
	}	

	// read in DDS header
	DDS_HEADER ddsh;
	file->read(&ddsh, sizeof(ddsh));

	// check if image is a volume texture
	if ((ddsh.dwCaps2 & DDSF_VOLUME) && (ddsh.dwDepth > 0))
		depth = ddsh.dwDepth;
	else
		depth = 0;

	// There are flags that are supposed to mark these fields as valid,
	// but some dds files don't set them properly
	dim.Width = ddsh.dwWidth;
	dim.Height = ddsh.dwHeight;

	// figure out what the image format is
	if (ddsh.ddspf.dwFlags & DDSF_FOURCC) 
	{
		switch(ddsh.ddspf.dwFourCC)
		{
			case FOURCC_DXT1:
				format = ECF_DXT1;
				bytesPerElement = 8;
				btcCompressed = true;
				break;
			case FOURCC_DXT2:
			case FOURCC_DXT3:
				format = ECF_DXT3;
				bytesPerElement = 16;
				btcCompressed = true;
				break;
			case FOURCC_DXT4:
			case FOURCC_DXT5:
				format = ECF_DXT5;
				bytesPerElement = 16;
				btcCompressed = true;
				break;
			//these are unsupported for now
			default:
				LOGGER.logErr("Unsupported DDS file color format.");
				goto fail;
		}
	}
	else if (ddsh.ddspf.dwRGBBitCount == 32)
	{
		format = ECF_A8R8G8B8;
		bytesPerElement = 4;
		btcCompressed = false;
	}
	else if (ddsh.ddspf.dwRGBBitCount == 24)
	{
		format = ECF_R8G8B8;
		bytesPerElement = 3;
		btcCompressed = false;
	}
	else 
	{
		LOGGER.logErr("Unsupported DDS file color format.");
		goto fail;
	}

	image = new CImageDDS(format, dim, depth);

	if (ddsh.dwFlags & DDSF_MIPMAPCOUNT)
		image->m_LevelCount = ddsh.dwMipMapCount;
	else
		image->m_LevelCount = 1;

	//check cube-map faces
	if (ddsh.dwCaps2 & DDSF_CUBEMAP)
	{
		//this is a cubemap, count the faces
		image->m_Faces = 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) ? 1 : 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) ? 1 : 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) ? 1 : 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) ? 1 : 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) ? 1 : 0;
		image->m_Faces += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) ? 1 : 0;

		//check for a complete cubemap
		if ((image->m_Faces != 6) || (dim.Width != dim.Height))
		{
			LOGGER.logErr("There is incomplete cubemap in DDS file.");
			goto fail;
		}
	}
	else
	{
		//not a cubemap
		image->m_Faces = 0;
	}

	image->m_BytesPerPixel = bytesPerElement;

	for (u32 face = 0; face < (image->m_Faces ? image->m_Faces : 1); face++)
	{
		int w = dim.Width, h = dim.Height, d = (image->m_Depth) ? image->m_Depth : 1;
		for (u32 level = 0; level < image->m_LevelCount; level++)
		{
			int bw = (btcCompressed) ? (w+3)/4 : w;
			int bh = (btcCompressed) ? (h+3)/4 : h;
			int size = bw*bh*d*bytesPerElement;

			u8 *data = new u8[size];

			file->read(data, size);

			image->m_LevelData.push_back(data);

			//reduce mip sizes
			w = ( w > 1) ? w >> 1 : 1;
			h = ( h > 1) ? h >> 1 : 1;
			d = ( d > 1) ? d >> 1 : 1;
		}
	}
fail:
	return image;
}

//-----------------------------------------------------------------------------

IImage* CImageLoaderDDS::loadImage(const c8* fileName)
{   
	return CImageLoader::loadImage(fileName);
}

//---------------------------------------------------------------------------

IImageLoader* createImageLoaderDDS()
{
    return new CImageLoaderDDS();
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

