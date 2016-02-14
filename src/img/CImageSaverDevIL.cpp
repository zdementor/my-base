//|-------------------------------------------------------------------------
//| File:        CImageSaverDevIL.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImageSaverDevIL.h"
#include "CImageDevIL.h"

#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

CImageSaverDevIL::CImageSaverDevIL()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageSaverDevIL");
#endif
}

//-----------------------------------------------------------------------------

CImageSaverDevIL::~CImageSaverDevIL()
{   
	;
}

//-----------------------------------------------------------------------------

bool CImageSaverDevIL::isASaveableFileExtension(const c8* fileName)
{
	return CImageDevIL::isASaveableFileExtension(fileName);
}

//-----------------------------------------------------------------------------

bool CImageSaverDevIL::saveImage(IImage* image, io::IWriteFile* file)
{
	bool res = false;
	CImageDevIL *image_devil = new CImageDevIL();
	image_devil->imageFrom(image);
	res = image_devil->saveTo(file);
	SAFE_DROP(image_devil);
	return res;
}

//-----------------------------------------------------------------------------

IImageSaver* createImageSaverDevIL()
{
	return new CImageSaverDevIL;
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------