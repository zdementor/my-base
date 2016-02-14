//|-------------------------------------------------------------------------
//| File:        CImageLoaderJPG.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderJPGHPP
#define CImageLoaderJPGHPP
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
//---------------------------------------------------------------------------
#include <my_inc.h>
#include "../CompileConf.h"
//---------------------------------------------------------------------------
#ifdef _IRR_COMPILE_WITH_LIBJPEG_
#ifdef FAR
    #undef FAR  //в файле jmoreconfig.h будет определена
#endif
#define XMD_H
extern "C" { 
#include <jpeglib.h>
}
#endif
//---------------------------------------------------------------------------
#include "CImageLoader.h"
//---------------------------------------------------------------------------
namespace my {
namespace img {
//---------------------------------------------------------------------------
//! Surface Loader fow JPG images
class CImageLoaderJPG : public CImageLoader
{
public:

    //! constructor
    CImageLoaderJPG();

    //! destructor
    virtual ~CImageLoaderJPG();

    //! returns true if the file maybe is able to be loaded by this class
    //! based on the file extension (e.g. ".tga")
    virtual bool isALoadableFileExtension(const c8* fileName);

    //! returns true if the file maybe is able to be loaded by this class
    virtual bool isALoadableFileFormat(io::IReadFile* file);

    //! creates a surface from the file
    virtual IImage* loadImage(io::IReadFile* file);

    //! загружает изображение из файла
    virtual IImage* loadImage(const c8* fileName);
private:

    #ifdef _IRR_COMPILE_WITH_LIBJPEG_

    /*  Initialize source.  This is called by jpeg_read_header() before any
    data is actually read.  Unlike init_destination(), it may leave
    bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
    will occur immediately). */
    static void init_source (j_decompress_ptr cinfo);

    /*  This is called whenever bytes_in_buffer has reached zero and more
    data is wanted.  In typical applications, it should read fresh data
    into the buffer (ignoring the current state of next_input_byte and
    bytes_in_buffer), reset the pointer & count to the start of the
    buffer, and return TRUE indicating that the buffer has been reloaded.
    It is not necessary to fill the buffer entirely, only to obtain at
    least one more byte.  bytes_in_buffer MUST be set to a positive value
    if TRUE is returned.  A FALSE return should only be used when I/O
    suspension is desired (this mode is discussed in the next section). */
    static u8 fill_input_buffer (j_decompress_ptr cinfo);

    /* Skip num_bytes worth of data.  The buffer pointer and count should
    be advanced over num_bytes input bytes, refilling the buffer as
    needed.  This is used to skip over a potentially large amount of
    uninteresting data (such as an APPn marker).  In some applications
    it may be possible to optimize away the reading of the skipped data,
    but it's not clear that being smart is worth much trouble; large
    skips are uncommon.  bytes_in_buffer may be zero on return.
    A zero or negative skip count should be treated as a no-op. */
    static void skip_input_data (j_decompress_ptr cinfo, long num_bytes);

    /* This routine is called only when the decompressor has failed to find
    a restart (RSTn) marker where one is expected.  Its mission is to
    find a suitable point for resuming decompression.  For most
    applications, we recommend that you just use the default resync
    procedure, jpeg_resync_to_restart().  However, if you are able to back
    up in the input data stream, or if you have a-priori knowledge about
    the likely location of restart markers, you may be able to do better.
    Read the read_restart_marker() and jpeg_resync_to_restart() routines
    in jdmarker.c if you think you'd like to implement your own resync
    procedure. */
    static void resync_to_restart (j_decompress_ptr cinfo, long desired);

    /* Terminate source --- called by jpeg_finish_decompress() after all
    data has been read.  Often a no-op. */
    static void term_source (j_decompress_ptr cinfo);

    /* */
    static void my_error_exit(j_common_ptr cinfo);

    /* */
    static void my_emit_message (j_common_ptr cinfo, int msg_level);

    /* Routine that actually outputs a trace or error message */
    static void my_output_message (j_common_ptr cinfo);

    /* Format a message string for the most recent JPEG error or message */
    static void my_format_message (j_common_ptr cinfo, char * buffer);

    #endif
};
//---------------------------------------------------------------------------
} // end namespace video
} // end namespace irr
//---------------------------------------------------------------------------
#endif

