//|-------------------------------------------------------------------------
//| File:        CImageDevIL.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <io/ILogger.h>
#include <io/IReadFile.h>
#include <io/IWriteFile.h>
#include <io/IFileSystem.h>

#include "CImageDevIL.h"

#include <IL/il.h>
#include <IL/ilu.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

class ilInitializer
{
public:
	ilInitializer()
	{
		ilInit();
		iluInit();
		ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT3);
		//ilSetInteger(IL_TGA_RLE, IL_TRUE); // not working at all
		ilSetInteger(IL_BMP_RLE, IL_TRUE);
	}
	virtual ~ilInitializer()
	{
		ilShutDown();
	}
}
_ilInitializer;

class ilImage
{
public:
	ilImage();
	virtual ~ilImage();

	void free();
	ILboolean texImage(IImage *image);
	ILboolean load(ILenum type, ILHANDLE file);
	ILboolean save(ILenum type, ILHANDLE file);
	void bind();
	const void* data() const;

protected:
	ILuint Id;
};

ilImage::ilImage()
{
	this->Id = 0;
	this->bind();
}

ilImage::~ilImage()
{
	free();
}

void ilImage::bind()
{
	if (!Id)
		ilGenImages(1, &Id);
	ilBindImage(this->Id);
}

ILboolean ilImage::load(ILenum type, ILHANDLE file)
{
	bool ret = false;

	free();
	bind();

	ret = !!ilLoadF(type, file);
	
	if (ret)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_LOWER_LEFT)
		// flip vertical
			iluFlipImage();
	}
	return ret;
}

ILboolean ilImage::save(ILenum type, ILHANDLE file)
{
	bind();
	return ilSaveF(type, file);
}

void ilImage::free()
{
	if (Id)
	{
		ilDeleteImages(1, &this->Id);
		Id = 0;
	}
}

//----------------------------------------------------------------------------

CImageDevIL::CImageDevIL()
	: CImage()
{  
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImageDevIL CImage");
#endif
	_defaultInit();

	m_ILImage = new ilImage();
}

//----------------------------------------------------------------------------

CImageDevIL::~CImageDevIL()
{   
	delete m_ILImage;
}

//----------------------------------------------------------------------------

bool CImageDevIL::imageFrom(IImage *image)
{
	if (!image)
		return false;

	ILint format;

	switch (image->getColorFormat())
	{
	case img::ECF_DXT1:
		format = IL_DXT1;
		break;
	case img::ECF_DXT3:
		format = IL_DXT3;
		break;
	case img::ECF_DXT5:
		format = IL_DXT5;
		break;
	case img::ECF_R8G8B8:
		format = IL_RGB;
		break;
	case img::ECF_A8R8G8B8:
		format = IL_BGRA;
		break;
	case img::ECF_A8B8G8R8:
		format = IL_RGBA;
		break;
	default:
		void *data = image->getData();
		image->convertTo(ECF_A8B8G8R8);
		format = IL_RGBA;
		break;
	}

	ILint width = image->getDimension().Width;
	ILint height = image->getDimension().Height;
	ILint depth = image->getDepth() ? image->getDepth() : 1;
	ILint bpp = image->getBytesPerPixel();
	void *data = image->getData();

	m_ILImage->bind();
	if (format != IL_DXT1 && format != IL_DXT3 && format != IL_DXT5)
		ilTexImage(width, height, depth, bpp, format, IL_UNSIGNED_BYTE, data);
	else
	{
		ilTexImageDxtc(width, height, depth, format, (const ILubyte*)data);
		ilDxtcDataToImage();
	}
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_LOWER_LEFT)
		// flip vertical
			iluFlipImage();
	}
	_defaultInit();
	{
		bool res = true;
		ILint format = ilGetInteger(IL_IMAGE_FORMAT);
		ILint dxtc   = ilGetInteger(IL_DXTC_DATA_FORMAT);		
		ILint width  = ilGetInteger(IL_IMAGE_WIDTH);
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);

		switch (dxtc)
		{
		case IL_DXT1:
		case IL_DXT3:
		case IL_DXT5:
			res = false;
			break;
		default:
			{
				switch (format)
				{
				case IL_BGR:
				case IL_RGB:
					m_Format = img::ECF_R8G8B8;
					break;
				case IL_BGRA:
					m_Format = img::ECF_A8R8G8B8;
					break;
				case IL_RGBA:
					m_Format = img::ECF_A8B8G8R8;
					break;
				default:
					res = false;
					break;
				}
			}
		}
		if (res)
		{
			m_Data = ilGetData();
			m_DataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			m_BytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			m_Dim.set(width, height);
		}
	}

	return true;
}

//----------------------------------------------------------------------------

bool CImageDevIL::loadFrom(io::IReadFile *file)
{
	if (!file)
		return false;
	const c8 *file_name = file->getFileName();
	bool res = false;

	_defaultInit();

	if (strstr(file_name, ".tga") != 0)
		res = !!m_ILImage->load(IL_TGA, (FILE*)file->getHandle());

	if (res)
	{
		m_ILImage->bind();

		ILint format = ilGetInteger(IL_IMAGE_FORMAT);
		ILint dxtc   = ilGetInteger (IL_DXTC_DATA_FORMAT);		
		ILint width = ilGetInteger(IL_IMAGE_WIDTH);
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);

		switch (dxtc)
		{
		case IL_DXT1:
		case IL_DXT3:
		case IL_DXT5:
			// Don't use DevIL to load DXT compressed images,
			// because it non optimaly manages DXT image data
			// (it stores simmultaniously Data and DxtcData binary data).
			// And there is no public access to the DxtcData.
			res = false;
			break;
		default:
			{
				switch (format)
				{
				case IL_BGR:
				case IL_RGB:
					m_Format = img::ECF_R8G8B8;
					break;
				case IL_BGRA:
					m_Format = img::ECF_A8R8G8B8;
					break;
				case IL_RGBA:
					m_Format = img::ECF_A8B8G8R8;
					break;
				default:
					res = false;
					break;
				}
			}
		}
		if (res)
		{
			m_Data = ilGetData();
			m_DataSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			m_BytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			m_Dim.set(width, height);
		}
	}
	if (!res)
	{
		m_ILImage->free();
	}
	return res;
}

//----------------------------------------------------------------------------

bool CImageDevIL::saveTo(io::IWriteFile *file)
{
	if (!file)
		return false;
	const c8 *file_name = file->getFileName();
	bool res = false;

	if (strstr(file_name, ".tga") != 0)
		res = !!m_ILImage->save(IL_TGA, (FILE*)file->getHandle());
	else
	if (strstr(file_name, ".dds") != 0)
	{
		res = !!m_ILImage->save(IL_DDS, (FILE*)file->getHandle());
	}
	else
	if (strstr(file_name, ".jpg") != 0)
		res = !!m_ILImage->save(IL_JPG, (FILE*)file->getHandle());
	else
	if (strstr(file_name, ".bmp") != 0)
		res = !!m_ILImage->save(IL_BMP, (FILE*)file->getHandle());

	return res;
}

//-------------------------------------------------------------

bool CImageDevIL::isALoadableFileExtension(const c8* fileName)
{
	// Don't use DevIL to load DXT compressed images,
	// because it non optimaly manages DXT image data
	// (it stores simmultaniously Data and DxtcData binary data).
	// And there is no public access to the DxtcData.
    return false;//strstr(fileName, ".tga") != 0;
}

//-------------------------------------------------------------

bool CImageDevIL::isALoadableFileFormat(io::IReadFile* file)
{
    if (!file)
        return false;
	return false; 

	// read file header
	file->seek(0);
	{
	// tgs
		u8 type[3];
		file->read(&type, sizeof(u8)*3);
		if (type[2]==1 || type[2]==2 || type[2]==10)
			return true;
	}
    return false; 
}

//----------------------------------------------------------------------------

bool CImageDevIL::isASaveableFileExtension(const c8* fileName)
{
	return
		strstr(fileName, ".tga") != 0
		|| strstr(fileName, ".dds") != 0
		|| strstr(fileName, ".jpg") != 0
		|| strstr(fileName, ".bmp") != 0
		;
}

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------
