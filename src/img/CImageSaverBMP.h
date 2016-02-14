//|-------------------------------------------------------------------------
//| File:        CImageSaverBMP.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageSaverBMPHPP
#define CImageSaverBMPHPP
//-----------------------------------------------------------------------------

#include "CImageLoaderBMP.h"
#include "CImageSaver.h"

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! Класс который может сохранять изображение в файл
class CImageSaverBMP : public CImageSaver
{
private:

public:

    //! constructor
    CImageSaverBMP();

    //! destructor
    virtual ~CImageSaverBMP();

    //! return true if this writer can write a file with the given extension
    virtual bool isASaveableFileExtension(const c8* fileName);

    //! write image to file
    virtual bool saveImage(IImage* image, io::IWriteFile* file);
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

#endif

