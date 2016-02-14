//|-------------------------------------------------------------------------
//| File:        CImageSaverJPG.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageSaverJPGHPP
#define CImageSaverJPGHPP
//-----------------------------------------------------------------------------

#include "CImageLoaderJPG.h"
#include "CImageSaver.h"

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------
//! Класс который может сохранять изображение в файл 
class CImageSaverJPG : public CImageSaver
{
private:
    #ifdef _IRR_COMPILE_WITH_LIBJPEG_
    /* */
    static void my_error_exit(j_common_ptr cinfo);

    /* */
    static void my_emit_message (j_common_ptr cinfo, int msg_level);

    /* Routine that actually outputs a trace or error message */
    static void my_output_message (j_common_ptr cinfo);

    /* Format a message string for the most recent JPEG error or message */
    static void my_format_message (j_common_ptr cinfo, char * buffer);

    #endif

public:

    //! constructor
    CImageSaverJPG();

    //! destructor
    virtual ~CImageSaverJPG();

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

