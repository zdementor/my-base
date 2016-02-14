//|-------------------------------------------------------------------------
//| File:        CImageSaverBMP.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageSaverBMP.h"
#include "CColorConverter.h"
#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! конструктор
CImageSaverBMP::CImageSaverBMP()
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageSaverBMP");
#endif
}

//-----------------------------------------------------------------------------

//! деструктор
CImageSaverBMP::~CImageSaverBMP()
{   	
}

//-----------------------------------------------------------------------------

//! возвращает true если способен записать файл в таком формате
//! определяет это по расширению файла (e.g. ".bmp")
bool CImageSaverBMP::isASaveableFileExtension(const c8* fileName)
{   
	return strstr(fileName, ".bmp") !=0;
}

//-----------------------------------------------------------------------------

//! записывает битмап в файл
bool CImageSaverBMP::saveImage(IImage* image, io::IWriteFile* file)
{
	// we always write 24-bit color because nothing really reads 32-bit

	SBMPHeader imageHeader;
	imageHeader.Id = 0x4d42;
	imageHeader.Reserved = 0;
	imageHeader.BitmapDataOffset = sizeof(imageHeader);
	imageHeader.BitmapHeaderSize = 0x28;
	imageHeader.Width = image->getDimension().Width;
	imageHeader.Height = image->getDimension().Height;
	imageHeader.Planes = 1;
	imageHeader.BPP = 24;
	imageHeader.Compression = 0;
	imageHeader.PixelPerMeterX = 0;
	imageHeader.PixelPerMeterY = 0;
	imageHeader.Colors = 0;
	imageHeader.ImportantColors = 0;

    s32 widthInBytes = image->getDimension().Width * 3;
    s32 lineData = widthInBytes + ((4-(widthInBytes%4)))%4;

	// data size is rounded up to nearest 4 bytes
	imageHeader.BitmapDataSize = lineData*image->getDimension().Height + 2;

	// file size is data size plus offset to data
	imageHeader.FileSize = imageHeader.BitmapDataOffset + imageHeader.BitmapDataSize;

	// bitmaps are stored upside down and padded so we always do this
	void (*CColorConverter_convertFORMATtoFORMAT)(const void*, s32, void*) = 0; 
	switch(image->getColorFormat())
	{
	case ECF_R8G8B8:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_R8G8B8toR8G8B8;
		break;
	case ECF_A8R8G8B8:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_A8R8G8B8toR8G8B8;
	case ECF_A8B8G8R8:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_A8B8G8R8toR8G8B8;
		break;
	case ECF_A1R5G5B5:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_A1R5G5B5toR8G8B8;
		break;
	case ECF_R5G6B5:
		CColorConverter_convertFORMATtoFORMAT
			= CColorConverter::convert_R5G6B5toR8G8B8;
		break;
	}

	// couldn't find a color converter
	if (!CColorConverter_convertFORMATtoFORMAT)
		return false;

	// write the bitmap header
	if (file->write(&imageHeader, sizeof(imageHeader)) != sizeof(imageHeader))
		return false;

	u8* scan_lines = (u8*)image->getData();
	if (!scan_lines)
		return false;

	// size of one pixel in bytes
	u32 pixel_size = image->getBytesPerPixel();

	// length of one row of the source image in bytes
	u32 row_stride = (pixel_size * imageHeader.Width);

	// length of one row in bytes, rounded up to nearest 4-byte boundary
	s32 row_size = ((3 * imageHeader.Width) + 3) & ~3;

	// allocate and clear memory for our scan line
	u8* row_pointer = new u8[row_size];
	memset(row_pointer, 0, row_size);

	u8* flipped_row_pointer = new u8[row_size];
	memset(flipped_row_pointer, 0, row_size);

	// convert the image to 24-bit RGB and flip it over
	s32 y;
	for (y = imageHeader.Height - 1; 0 <= y; --y)
	{
		// convert the image to 24-bit RGB
		CColorConverter_convertFORMATtoFORMAT(&scan_lines[y * row_stride], imageHeader.Width, row_pointer);

		// flip color
		CColorConverter::convert_R8G8B8toB8G8R8(row_pointer, imageHeader.Width, flipped_row_pointer);

		if (file->write(flipped_row_pointer, row_size) < (u32)row_size)
			break;
	}

	// final two bytes
	u16 word=0;
	file->write(&word, 2);

	// clean up our scratch area
	delete [] row_pointer;
	delete [] flipped_row_pointer;

	return y < 0;
}

//---------------------------------------------------------------------------

//! создает загрущик, который может загружать bmp файлы
IImageSaver* createImageSaverBMP()
{   
	return new CImageSaverBMP();
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
