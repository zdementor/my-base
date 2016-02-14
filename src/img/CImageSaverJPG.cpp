//|-------------------------------------------------------------------------
//| File:        CImageSaverJPG.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageSaverJPG.h"
#include "CColorConverter.h"

#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! конструктор
CImageSaverJPG::CImageSaverJPG()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageSaverJPG");
#endif
}

//-----------------------------------------------------------------------------

//! деструктор
CImageSaverJPG::~CImageSaverJPG()
{   
	;
}

//-----------------------------------------------------------------------------

//! возвращает true если способен записать файл в таком формате
//! определяет это по расширению файла (e.g. ".bmp")
bool CImageSaverJPG::isASaveableFileExtension(const c8* fileName)
{   
	return strstr(fileName, ".jpg") !=0;
}

//---------------------------------------------------------------------------

#ifdef _IRR_COMPILE_WITH_LIBJPEG_

void CImageSaverJPG::my_error_exit(j_common_ptr cinfo)
{   
	// Always display the message 
    (*cinfo->err->output_message) (cinfo);

    // Let the memory manager delete any temp files before we die 
    jpeg_destroy(cinfo);
}

//-------------------------------------------------------------------------------

void CImageSaverJPG::my_emit_message (j_common_ptr cinfo, int msg_level)
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
void CImageSaverJPG::my_output_message (j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];

    // Create the message 
    (*cinfo->err->format_message) (cinfo, buffer);
}

//-------------------------------------------------------------------------------

//! Format a message string for the most recent JPEG error or message */
void CImageSaverJPG::my_format_message (j_common_ptr cinfo, char * buffer)
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
        msg_code <= err->last_addon_message) {
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
            if (*msgptr == 's') isstring = TRUE;
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

#endif

//-----------------------------------------------------------------------------

//! записывает картинку в файл
bool CImageSaverJPG::saveImage(IImage* image, io::IWriteFile* file)
{
#ifndef _IRR_COMPILE_WITH_LIBJPEG_
    return 0;
#else

    //проверяем есть ли куда записывать
    if (!file)
    {   LOGGER.log("CImageSaverJPG::saveImage - Wrong parameter file = NULL.", io::ELL_ERROR);
        return 0;
    }
    //проверяем есть ли откуда записывать
    if (!image)
    {   LOGGER.log("CImageSaverJPG::saveImage - Wrong parameter file image = NULL.", io::ELL_ERROR);
        return 0;
    }
    //проверяем можем ли записать в файл с этим расширением
    if (!isASaveableFileExtension(file->getFileName()))
    {   LOGGER.log("Wrong extention for JPG format.", io::ELL_ERROR);
        return 0;
    }
    //проверяем формат сохраняемой картинки
    E_COLOR_FORMAT Format = image->getColorFormat();
    if ( Format!=ECF_R8G8B8)
    {  
		LOGGER.log("Only 24-bit is availiavle to save in JPG format.", io::ELL_ERROR);
        return 0;
    }

    // allocate and initialize JPEG compression object
    jpeg_compress_struct cinfo;

    //структура для обработки ошибок
    jpeg_error_mgr jerr;

    // указатель на массив данных
    BYTE* Data= (BYTE*)image->getData();

    UINT image_width  = image->getDimension().Width;
    UINT image_height = image->getDimension().Height;
    int BPP=3; //24 bit

    s32 pitch = 0;
    f32 t = (image_width) * (BPP / 8.0f);
    s32 widthInBytes = (s32)t;
    t -= widthInBytes;
    if (t!=0.0f)
        ++widthInBytes;
    s32 lineData = widthInBytes + ((4-(widthInBytes%4)))%4;
    //выравнивание на границу двойного слова
    pitch = lineData - widthInBytes;

    //конвертируем формат данных
    switch (Format)
    {
        case ECF_R8G8B8:
            //24 bit -> 24 bit
            break;
        case ECF_A8R8G8B8:
            //32 bit -> 24 bit
            break;
    }

    JSAMPROW row_pointer[1];
    int row_stride;

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

    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, (FILE*)file->getHandle());

    // image width and height, in pixels
    cinfo.image_width  = image_width;
    cinfo.image_height = image_height;
    // # of color components per pixel
    cinfo.input_components = BPP;         
    // colorspace of input image
    cinfo.in_color_space = JCS_RGB;     

    int quality = 100; // 100% качество
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE );

    jpeg_start_compress(&cinfo, TRUE);

    // JSAMPLEs per row in image_buffer
    row_stride = image_width * 3;   

    while (cinfo.next_scanline < cinfo.image_height) 
    {
        row_pointer[0] = & Data[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);       

    // everithing is ok
    return 1;

#endif
}

//---------------------------------------------------------------------------

//! создает загрущик, который может загружать jpg файлы
IImageSaver* createImageSaverJPG()
{   
	return new CImageSaverJPG();
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------


