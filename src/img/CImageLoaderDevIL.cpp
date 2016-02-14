//|-------------------------------------------------------------------------
//| File:        CImageLoaderDevIL.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImageLoaderDevIL.h"
#include "CImageDevIL.h"

#include <img/SColor.h>
#include <io/ILogger.h>

//-------------------------------------------------------------
namespace my {
namespace img {
//-------------------------------------------------------------

CImageLoaderDevIL::CImageLoaderDevIL()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageLoaderDevIL");
#endif
}

//-------------------------------------------------------------

CImageLoaderDevIL::~CImageLoaderDevIL()
{
}

//-------------------------------------------------------------

bool CImageLoaderDevIL::isALoadableFileExtension(const c8 *fileName)
{
	return CImageDevIL::isALoadableFileExtension(fileName);
}

//-------------------------------------------------------------

bool CImageLoaderDevIL::isALoadableFileFormat(io::IReadFile* file)
{
    return CImageDevIL::isALoadableFileFormat(file);
}

//-------------------------------------------------------------

IImage* CImageLoaderDevIL::loadImage(io::IReadFile* file)
{
	CImageDevIL *image = new CImageDevIL();
	if (!image->loadFrom(file))
		SAFE_DROP(image);
	return image;
}

//-------------------------------------------------------------

IImageLoader* createImageLoaderDevIL()
{
    return new CImageLoaderDevIL();
}

//-------------------------------------------------------------
} // end namespace img
} // end namespace my
//-------------------------------------------------------------
