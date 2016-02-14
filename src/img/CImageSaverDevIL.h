//|-------------------------------------------------------------------------
//| File:        CImageSaverDevIL.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageSaverTGAHPP
#define CImageSaverTGAHPP

//-----------------------------------------------------------------------------

#include "CImageLoaderTGA.h"
#include "CImageSaver.h"

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

class CImageSaverDevIL : public CImageSaver
{
public:

    //! constructor
    CImageSaverDevIL();

    //! destructor
    virtual ~CImageSaverDevIL();

    //! return true if this writer can write a file with the given extension
    virtual bool isASaveableFileExtension(const c8* fileName);

    //! write image to file
    virtual bool saveImage(IImage* image, io::IWriteFile* file);
};

//-----------------------------------------------------------------------------
} // namespace img
} // namespace my
//-----------------------------------------------------------------------------

#endif // #ifndef CImageSaverTGAHPP
