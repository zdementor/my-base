//|-------------------------------------------------------------------------
//| File:        ISurfaceSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISurfaceSaverHPP
#define ISurfaceSaverHPP
//-----------------------------------------------------------------------------

#include <IUnknown.h>
#include <img/SColor.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! class wich is able to save image data all kind of image formats
class __MY_IMG_LIB_API__ ISurfaceSaver : public IUnknown
{

public:

    //! constructor
    ISurfaceSaver() {;}

    //! destructor
    virtual ~ISurfaceSaver() {;}

    //! saves a surface to a file
    virtual bool saveImageToFile(IImage* image, io::IWriteFile* file) = 0;

    //! saves a surface to a file
    virtual bool saveImageToFile(IImage* image, const c8* fileName) = 0;

	//! adds an external surface Saver to the array of Savers
	virtual void addSurfaceSaver(IImageSaver* saver) = 0;
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

