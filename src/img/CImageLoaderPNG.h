//|-------------------------------------------------------------------------
//| File:        CImageLoaderPNG.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderPngHPP
#define CImageLoaderPngHPP
//--------------------------------------------------------------------
#include "CImageLoader.h"
//--------------------------------------------------------------------
namespace my {
namespace img {
//--------------------------------------------------------------------
//!  Surface Loader fow PNG files
class CImageLoaderPng : public CImageLoader 
{ 
public: 

   //! constructor 
   CImageLoaderPng(); 

   //! destructor 
   virtual ~CImageLoaderPng(); 

   //! returns true if the file maybe is able to be loaded by this class 
   //! based on the file extension (e.g. ".png") 
   virtual bool isALoadableFileExtension(const c8* fileName); 

   //! returns true if the file maybe is able to be loaded by this class 
   virtual bool isALoadableFileFormat(io::IReadFile* file); 

   //! creates a surface from the file 
   virtual IImage* loadImage(io::IReadFile* file); 

private: 
     
	//helper function.  reads in a row of pixels from the image 
	const unsigned char* ReadRow(void *row_ptr); 

	//some variables 
	unsigned int width;
	unsigned int height; 
	int bitdepth;
	int colortype;
	int interlace;
	int compression;
	int filter;

	// semi global buffer for reading in row data 
	unsigned char g_png_load_buffer[0x1000];   // 32768
}; 
//--------------------------------------------------------------------
} // end namespace img 
} // end namespace my 
//--------------------------------------------------------------------
#endif 
