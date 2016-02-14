//|-------------------------------------------------------------------------
//| File:        CImageLoaderPNG.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "../CompileConf.h"

#ifdef _IRR_COMPILE_WITH_LIBPNG_
#include <png.h>
#endif

#include "CImageLoaderPNG.h"
#include <string.h>
#include "CColorConverter.h"
#include "CImage.h"
#include "../io/CReadFile.h"
#include <io/ILogger.h>

//--------------------------------------------------------------------
namespace my { 
namespace img { 
//--------------------------------------------------------------------    
#ifdef _IRR_COMPILE_WITH_LIBPNG_

//--------------------------------------------------------------------
// PNG function for error handling 
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg) 
{ 
   if(png_ptr) 
   { 
      char temp[256]; 
      sprintf(temp,"PNG FATAL ERROR: png error - %s",msg); 
	  LOGGER.log(temp, io::ELL_ERROR);
   } 
} 

//--------------------------------------------------------------------
// PNG function for file reading 
void user_read_data_fcn(png_structp png_ptr,png_bytep data, png_size_t length) 
{ 
   png_size_t check; 

   // changed by zola { 
   io::IReadFile* file=(io::IReadFile*)png_ptr->io_ptr; 
   check=(png_size_t) file->read((void*)data, (s32)length); 
   // } 

   if (check != length) 
	   png_error(png_ptr, "Read Error"); 
} 
#endif // _IRR_COMPILE_WITH_LIBPNG_

//--------------------------------------------------------------------
CImageLoaderPng::CImageLoaderPng()
{ 
   // do something? 
} 
//--------------------------------------------------------------------
CImageLoaderPng::~CImageLoaderPng() 
{ 
   // do something? 
} 

//--------------------------------------------------------------------
//! returns true if the file maybe is able to be loaded by this class 
//! based on the file extension (e.g. ".tga") 
bool CImageLoaderPng::isALoadableFileExtension(const c8* fileName) 
{ 
#ifdef _IRR_COMPILE_WITH_LIBPNG_

	// jox: added fix for file extension check by jox
	const c8* ext = strrchr(fileName, '.');

	if (ext == 0) return false;
	
	return (strstr(ext, ".PNG") != 0) || (strstr(ext, ".png") != 0); 

#else

	return false;

#endif // #ifdef _IRR_COMPILE_WITH_LIBPNG_
} 

//--------------------------------------------------------------------
//! returns true if the file maybe is able to be loaded by this class 
bool CImageLoaderPng::isALoadableFileFormat(io::IReadFile* file) 
{ 
#ifdef _IRR_COMPILE_WITH_LIBPNG_

	if (!file)
		return false; 

	// Read the first few bytes of the PNG file 
	if (file->read(&g_png_load_buffer, 8) != 8) 
		return false; 

	// CHeck if it really is a PNG file 
	if (!png_check_sig((png_bytep)g_png_load_buffer, 8)) 
		return false; 

	return true; //if we are here then it must be a png 

#else 

	return false;

#endif // #ifdef _IRR_COMPILE_WITH_LIBPNG_
} 

//--------------------------------------------------------------------
// load in the image data 
IImage* CImageLoaderPng::loadImage(io::IReadFile* file) 
{ 
#ifdef _IRR_COMPILE_WITH_LIBPNG_

	if (!file) 
		return 0; 

	bool alphaSupport = true; 

	png_structp png_ptr; 
	png_infop info_ptr; 

	// Read the first few bytes of the PNG file 
	if( file->read(&g_png_load_buffer, 8) != 8 ) 
	{ 
		LOGGER.log("LOAD PNG: can't read file\n", file->getFileName(), io::ELL_ERROR);
		return 0; 
	} 

	// CHeck if it really is a PNG file 
	if( !png_check_sig((png_bytep)g_png_load_buffer,8) ) 
	{ 
		LOGGER.log("LOAD PNG: not really a png\n", file->getFileName(), io::ELL_ERROR);
		return 0; 
	} 

	// Allocate the png read struct 
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,(png_error_ptr)png_cpexcept_error, (png_error_ptr)0 ); 
	if (!png_ptr) 
	{ 
		LOGGER.log("LOAD PNG: Internal PNG create read struct failure\n", file->getFileName(), io::ELL_ERROR);
		return 0;
	} 

	// Allocate the png info struct 
	info_ptr = png_create_info_struct(png_ptr); 
	if (!info_ptr) 
	{ 
		LOGGER.log("LOAD PNG: Internal PNG create info struct failure\n", file->getFileName(), io::ELL_ERROR);
		png_destroy_read_struct(&png_ptr, 0, 0); 
		return 0; 
	} 

	//png_init_io(png_ptr,fp);   // Init png 
	// changed by zola so we don't need to have public FILE pointers 
	png_set_read_fn(png_ptr, file, user_read_data_fcn); 

	png_set_sig_bytes(png_ptr, 8);   // Tell png that we read the signature 

	png_read_info(png_ptr, info_ptr);   // Read the info section of the png file 

	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, 
		(png_uint_32*)&height, &bitdepth, &colortype, &interlace,
		&compression, &filter);   // Extract info 

	if ( bitdepth != 8)
	{
		LOGGER.log("PNG LOAD: Failure - Only 8 bits per color supported", io::ELL_ERROR); 
		if(png_ptr) 
			png_destroy_read_struct(&png_ptr,&info_ptr, 0);   // Clean up memory 
		return 0;
	}
	
	if (colortype==PNG_COLOR_TYPE_RGBA) 
		alphaSupport = true; 
	else 
	if (colortype==PNG_COLOR_TYPE_RGB) 
		alphaSupport = false; 
	//else
	//if (colortype==PNG_COLOR_TYPE_PALETTE) 
//		alphaSupport = false; 
    else 
	{
		LOGGER.log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", io::ELL_ERROR); 
		if(png_ptr) 
			png_destroy_read_struct(&png_ptr,&info_ptr, 0);   // Clean up memory 
		return 0;
	}
	
	if ( interlace!= PNG_INTERLACE_NONE)
	{
		LOGGER.log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", io::ELL_ERROR); 
		if(png_ptr) 
			png_destroy_read_struct(&png_ptr,&info_ptr, 0);   // Clean up memory 
		return 0;
	}

	// Update the changes 
	png_read_update_info(png_ptr, info_ptr); 

	png_get_IHDR(
		png_ptr, info_ptr, 
		(png_uint_32*)&width, (png_uint_32*)&height, 
		&bitdepth,&colortype, &interlace, &compression, 
		&filter
		);   // Extract info 

	// Set up the data transformations you want.  Note that these are all
	// optional.  Only call them if you want/need them.  Many of the
	// transformations only work on specific types of images, and many
	// are mutually exclusive.

	// tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);

	// Strip alpha bytes from the input data without combining with the
    // background (not recommended).
	png_set_strip_alpha(png_ptr);

	// Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	// byte into separate bytes (useful for paletted and grayscale images).
	png_set_packing(png_ptr);

	// Change the order of packed pixels to least significant bit first
    // (not useful if you are using png_set_packing). */
	png_set_packswap(png_ptr);

	// Expand paletted colors into true RGB triplets
	if (colortype == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// Check the number of bytes per row 
	int bytes_per_row = png_get_rowbytes(png_ptr, info_ptr); 

	if( bytes_per_row >= (int)sizeof( g_png_load_buffer ) )
	{
		LOGGER.log("PNG LOAD: Failure - Format not supported - must be 24 or 32 bits per pixel", io::ELL_ERROR); 
		if(png_ptr) 
			png_destroy_read_struct(&png_ptr,&info_ptr, 0);   // Clean up memory 
		return 0;
	} 

	//now read the png stucture into a simple bitmap array 
	IImage* image = 0; 
	if (alphaSupport) 
		image = new CImage(ECF_A8R8G8B8, core::dimension2d<s32>(width, height));
	else 
		image = new CImage(ECF_R8G8B8, core::dimension2d<s32>(width, height)); 

	const unsigned char* pSrc; 
	const unsigned char inc = alphaSupport ? 4 : 3; 
	unsigned char* data = (unsigned char*)image->getData();

	for ( unsigned int y = 0; y < this->height; y++ ) 
	{ 
		//read in a row of pixels 
		pSrc = this->ReadRow(png_ptr); 

		for (unsigned int x = 0; x < this->width; x++ ) 
		{ 
			//loop through the row of pixels 
			if (!alphaSupport) 
			{ 
				data[y*width*inc + x*inc] = *(pSrc); //red 
				data[y*width*inc + x*inc + 1] = *(pSrc+1); //green 
				data[y*width*inc + x*inc + 2] = *(pSrc+2); //blue 
			} 
			else 
			{ 
				data[y*width*inc + x*inc]     = *(pSrc+2); //red 
				data[y*width*inc + x*inc + 1] = *(pSrc+1); //green 
				data[y*width*inc + x*inc + 2] = *(pSrc); //blue 
				data[y*width*inc + x*inc + 3] = *(pSrc+3); //alpha 
			} 

			pSrc+=inc; //move to next pixel (24 or 32 bits - depends on alpha) 
		} 
	} 

	if (png_ptr) 
		png_destroy_read_struct(&png_ptr,&info_ptr, 0);   // Clean up memory 

	return image; 

#else

	return 0;

#endif #ifdef _IRR_COMPILE_WITH_LIBPNG_
} 

//--------------------------------------------------------------------
const unsigned char* CImageLoaderPng::ReadRow(void *row_ptr) 
{ 
#ifdef _IRR_COMPILE_WITH_LIBPNG_

   png_read_row((png_structp)row_ptr, (png_bytep)g_png_load_buffer, 0); 
   return (const unsigned char*)g_png_load_buffer; 

#else

	return 0;

#endif // #ifdef _IRR_COMPILE_WITH_LIBPNG_
} 

//--------------------------------------------------------------------
IImageLoader* createImageLoaderPNG()
{
	return new CImageLoaderPng();
}

//--------------------------------------------------------------------
}// end namespace img
}//end namespace my
//--------------------------------------------------------------------

