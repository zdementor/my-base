//|-------------------------------------------------------------------------
//| File:        IImageSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IImageSaverHPP
#define IImageSaverHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>
#include <io/IWriteFile.h>
#include <img/IImage.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//!	Interface for class wich is able to storeing image data into file
class __MY_IMG_LIB_API__ IImageSaver : public IUnknown
{
public:

	//! destructor
	virtual ~IImageSaver() {};

	//! return true if this writer can write a file with the given extension
	virtual bool isASaveableFileExtension(const c8* fileName) = 0;

	//! write image to file
	virtual bool saveImage(IImage *image, const c8* fileName) = 0;  
	
	//! write image to file
    virtual bool saveImage(IImage* image, io::IWriteFile* file) = 0; 
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

