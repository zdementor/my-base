//|-------------------------------------------------------------------------
//| File:        CImageSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageSaverHPP
#define CImageSaverHPP
//-----------------------------------------------------------------------------

#include <img/IImage.h>
#include <img/IImageSaver.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! Базовый не абстрактный файл для записи изображений в файл
class CImageSaver : public IImageSaver
{
public:

	//! write image to file
	virtual bool saveImage(IImage* image, io::IWriteFile* file)
		{ return false; } 

	//! write image to file
	virtual bool saveImage(IImage *image, const c8* fileName);
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

