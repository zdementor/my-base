//|-------------------------------------------------------------------------
//| File:        CImageLoaderJPG.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageLoaderJPG.h"
#include "CColorConverter.h"
#include "CImage.h"

#include <io/ILogger.h>

//---------------------------------------------------------------------------
namespace my {
namespace img {
//---------------------------------------------------------------------------

//! constructor
CImageLoaderJPG::CImageLoaderJPG()
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImageLoaderJPG");
#endif
}

//---------------------------------------------------------------------------

//! destructor
CImageLoaderJPG::~CImageLoaderJPG()
{
}

//---------------------------------------------------------------------------

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderJPG::isALoadableFileExtension(const c8* fileName)
{
    return (strstr(fileName, ".jpg") != 0) || (strstr(fileName, ".jpeg") != 0);
}

//---------------------------------------------------------------------------

#ifdef _IRR_COMPILE_WITH_LIBJPEG_

void CImageLoaderJPG::init_source (j_decompress_ptr cinfo)
{
    // DO NOTHING
}

//---------------------------------------------------------------------------

u8 CImageLoaderJPG::fill_input_buffer (j_decompress_ptr cinfo)
{
    // DO NOTHING
    return 1;
}

//---------------------------------------------------------------------------

void CImageLoaderJPG::skip_input_data (j_decompress_ptr cinfo, long count)
{
    jpeg_source_mgr * src = cinfo->src;
    if(count > 0)
    {
        src->bytes_in_buffer -= count;
        src->next_input_byte += count;
    }
}

//---------------------------------------------------------------------------

void CImageLoaderJPG::resync_to_restart (j_decompress_ptr cinfo, long desired)
{
    // DO NOTHING
}

//---------------------------------------------------------------------------

void CImageLoaderJPG::term_source (j_decompress_ptr cinfo)
{
    // DO NOTHING
}

//-------------------------------------------------------------------------------

void CImageLoaderJPG::my_error_exit(j_common_ptr cinfo)
{   
	// Always display the message 
    (*cinfo->err->output_message) (cinfo);

    // Let the memory manager delete any temp files before we die 
    jpeg_destroy(cinfo);
}

//-------------------------------------------------------------------------------

void CImageLoaderJPG::my_emit_message (j_common_ptr cinfo, int msg_level)
{   struct jpeg_error_mgr * err = cinfo->err;

    if (msg_level < 0) 
	{
		// It's a warning message.  Since corrupt files may generate many warnings,
		// the policy implemented here is to show only the first warning,
		// unless trace_level >= 3.

        if (err->num_warnings == 0 || err->trace_level >= 3)
            (*err->output_message) (cinfo);

        // Always count warnings in num_warnings. 
        err->num_warnings++;
    } 
	else 
	{
        // It's a trace message.  Show it if trace_level >= msg_level.
        if (err->trace_level >= msg_level)
            (*err->output_message) (cinfo);
    }
}

//-------------------------------------------------------------------------------

//! Routine that actually outputs a trace or error message 
void CImageLoaderJPG::my_output_message (j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];

    // Create the message 
    (*cinfo->err->format_message) (cinfo, buffer);

    MessageBox (NULL, buffer, "Error!!!", MB_OK);

}

//-------------------------------------------------------------------------------

// Format a message string for the most recent JPEG error or message 
void CImageLoaderJPG::my_format_message (j_common_ptr cinfo, char * buffer)
{
    struct jpeg_error_mgr * err = cinfo->err;
    int msg_code = err->msg_code;
    const char * msgtext = NULL;
    const char * msgptr;
    char ch;
    boolean isstring;

    // Look up message string in proper table 
    if (msg_code > 0 && msg_code <= err->last_jpeg_message) 
	{
        msgtext = err->jpeg_message_table[msg_code];
    } 
	else if (
		err->addon_message_table != NULL &&
        msg_code >= err->first_addon_message &&
        msg_code <= err->last_addon_message
		) 
	{
        msgtext = err->addon_message_table[msg_code - err->first_addon_message];
    }

    // Defend against bogus message number 
    if (msgtext == NULL) 
	{
        err->msg_parm.i[0] = msg_code;
        msgtext = err->jpeg_message_table[0];
    }

    // Check for string parameter, as indicated by %s in the message text 
    isstring = FALSE;
    msgptr = msgtext;
    while ((ch = *msgptr++) != '\0') 
	{
        if (ch == '%') 
		{
            if (*msgptr == 's') 
				isstring = TRUE;

            break;
        }
    }

    // Format the message into the passed buffer 
    if (isstring)
        sprintf(buffer, msgtext, err->msg_parm.s);
    else
        sprintf(buffer, msgtext,
        err->msg_parm.i[0], err->msg_parm.i[1],
        err->msg_parm.i[2], err->msg_parm.i[3],
        err->msg_parm.i[4], err->msg_parm.i[5],
        err->msg_parm.i[6], err->msg_parm.i[7]);

}

#endif // _IRR_COMPILE_WITH_LIBJPEG_


//---------------------------------------------------------------------------
//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderJPG::isALoadableFileFormat(io::IReadFile* file)
{
#ifndef _IRR_COMPILE_WITH_LIBJPEG_    
    return false;    
#else
    
    if (!file)
        return false;

    s32 jfif = 0;
    file->seek(6);
    file->read(&jfif, sizeof(s32));
    return (jfif == 0x4a464946 || jfif == 0x4649464a);

#endif
}

//---------------------------------------------------------------------------

//! creates a surface from the file
IImage* CImageLoaderJPG::loadImage(io::IReadFile* file)
{
#ifndef _IRR_COMPILE_WITH_LIBJPEG_
    return 0;
#else

    //! return if the header is false
    if (!isALoadableFileFormat(file))
    {   LOGGER.log("Неправильный заголовок JPG файла.", io::ELL_ERROR);
        return 0;
    }

    file->seek(0);
    u8* input = new u8[file->getSize()];
    file->read(input, file->getSize());

    // allocate and initialize JPEG decompression object
    jpeg_decompress_struct cinfo;

    //структура для обработки ошибок
    jpeg_error_mgr jerr;

    //We have to set up the error handler first, in case the initialization
    //step fails.  (Unlikely, but it could happen if you are out of memory.)
    //This routine fills in the contents of struct jerr, and returns jerr's
    //address which we place into the link field in cinfo.
    cinfo.err = jpeg_std_error(&jerr);

    //обработчики ошибок
    jerr.error_exit     = my_error_exit;
    jerr.emit_message   = my_emit_message;
    jerr.output_message = my_output_message;
    jerr.format_message = my_format_message;

    // Now we can initialize the JPEG decompression object.
    jpeg_create_decompress(&cinfo);

    // specify data source
    jpeg_source_mgr jsrc;
                                                             
    // Set up data pointer                                               
    jsrc.bytes_in_buffer = file->getSize();
    jsrc.next_input_byte = (JOCTET*)input;
    cinfo.src = &jsrc;

    jsrc.init_source       = init_source;
    jsrc.fill_input_buffer = fill_input_buffer;
    jsrc.skip_input_data   = skip_input_data;
    jsrc.resync_to_restart = jpeg_resync_to_restart;                                                    
    jsrc.term_source       = term_source;
                                                                                             
    // Decodes JPG input from whatever source
    // Does everything AFTER jpeg_create_decompress
    // and BEFORE jpeg_destroy_decompress
    // Caller is responsible for arranging these + setting up cinfo

    // read file parameters with jpeg_read_header() 
    (void) jpeg_read_header(&cinfo, TRUE);

    // check image data format
	if (cinfo.jpeg_color_space==JCS_GRAYSCALE || cinfo.jpeg_color_space == JCS_UNKNOWN)
	{	
		jpeg_destroy_decompress(&cinfo);

		if (cinfo.jpeg_color_space==JCS_GRAYSCALE)
			LOGGER.log("Unable to load jpeg-image, grayscale not supported.", io::ELL_WARNING);
		else if (cinfo.jpeg_color_space==JCS_UNKNOWN)
			LOGGER.log("Unable to load jpeg-image, unknown format.", io::ELL_WARNING);

		return NULL;
	}
                                                                                 
    // Start decompressor                                                                        
    (void) jpeg_start_decompress(&cinfo);
                                                                          
    // Get image data                                                   
    u16 rowspan = cinfo.image_width * cinfo.num_components;
    unsigned width = cinfo.image_width;                                   
    unsigned height = cinfo.image_height;

    bool has_alpha= false;  //(JPEG never has alpha)
    bool greyscale;

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
        greyscale = true;
    else
        greyscale = false;

    // Allocate memory for buffer
    u8 *output = new u8[rowspan * height];

    // Here we use the library's state variable cinfo.output_scanline as the
    // loop counter, so that we don't have to keep track ourselves.
    // Create array of row pointers for lib
    u8 **rowPtr = new u8 * [height];

    for( unsigned i = 0; i < height; i++ )
        rowPtr[i] = &output[ i * rowspan ];

    unsigned rowsRead = 0;

    while( cinfo.output_scanline < cinfo.output_height )
        rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

    delete [] rowPtr;
    // Finish decompression

    (void) jpeg_finish_decompress(&cinfo);

    // Release JPEG decompression object

    // This is an important step since it will release a good deal of memory.
    jpeg_destroy_decompress(&cinfo);

    // convert image
    IImage* image = new CImage(
		ECF_R8G8B8,
        core::dimension2d<s32>(width, height), output
		);

    delete [] input;

    return image;

#endif
}

//---------------------------------------------------------------------------

//! загружает изображение из файла
IImage* CImageLoaderJPG::loadImage(const c8* fileName)
{   
	return CImageLoader::loadImage(fileName);
}

//---------------------------------------------------------------------------

//! creates a loader which is able to load jpeg images
IImageLoader* createImageLoaderJPG()
{
    return new CImageLoaderJPG();
}

//---------------------------------------------------------------------------
} // end namespace video
} // end namespace irr
//---------------------------------------------------------------------------
