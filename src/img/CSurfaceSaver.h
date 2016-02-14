//|-------------------------------------------------------------------------
//| File:        CSurfaceSaver.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CSurfaceSaverHPP
#define CSurfaceSaverHPP
//-----------------------------------------------------------------------------

#include <img/ISurfaceSaver.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! class wich is able to save image data all kind of image formats
class CSurfaceSaver : public ISurfaceSaver
{
public:

    //! constructor
    CSurfaceSaver();

    //! destructor
    virtual ~CSurfaceSaver();

    //! saves a surface to a file
    virtual bool saveImageToFile(IImage* image, io::IWriteFile* file);

    //! saves a surface to a file
    virtual bool saveImageToFile(IImage* image, const c8* fileName);

	//! adds an external surface Saver to the array of Savers
	virtual void addSurfaceSaver(IImageSaver* saver);

private:

	core::array<img::IImageSaver*> SurfaceSaver;
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

