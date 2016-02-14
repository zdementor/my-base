//|-------------------------------------------------------------------------
//| File:        CImageLoaderPCX.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CImageLoaderPCX.h"
#include "CColorConverter.h"
#include "CImage.h"

#include <img/SColor.h>
#include <io/ILogger.h>

//-----------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------

//! constructor
CImageLoaderPCX::CImageLoaderPCX()
: PCXData(0), PaletteData(0)
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CImageLoaderPCX");
#endif
}

//-----------------------------------------------------------------------

//! destructor
CImageLoaderPCX::~CImageLoaderPCX()
{
    if (PaletteData)
        delete [] PaletteData;

    if (PCXData)
        delete [] PCXData;
}

//-----------------------------------------------------------------------

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderPCX::isALoadableFileExtension(const c8* fileName)
{
    return (strstr(fileName, ".PCX") != 0) || (strstr(fileName, ".pcx") != 0);
}

//-----------------------------------------------------------------------

//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderPCX::isALoadableFileFormat(io::IReadFile* file)
{
    u8 headerID;
    file->read(&headerID, sizeof(headerID));
    return headerID == 0x0a;
}

//-----------------------------------------------------------------------

//! creates a image from the file
IImage* CImageLoaderPCX::loadImage(io::IReadFile* file)
{
    SPCXHeader header;

    file->seek(0);
    file->read(&header, sizeof(header));

    s32 pitch = 0;

    //! return if the header is wrong
    if (header.Manufacturer != 0x0a && header.Encoding != 0x01)
        return 0;

    // return if this isn't a supported type
    if( (header.BitsPerPixel < 8) || (header.BitsPerPixel > 24) )
    {
        LOGGER.log("Unsupported bits per pixel in PCX file.",
            file->getFileName(), io::ELL_WARNING);
        return 0;
    }

    // read palette
    if( header.BitsPerPixel >= 8 )
    {
        s32 pos = file->getPos();
        u8 palIndicator;
        file->seek( file->getSize()-256*3-1, false );

        file->read( &palIndicator, 1 );

        if( palIndicator != 12 )
        {
            LOGGER.log("Unsupported pal indicator in PCX file.",
                file->getFileName(), io::ELL_WARNING);
            return 0;
        }

        u8 *tempPalette = new u8[768];
        PaletteData = new s32[256];
        file->read( tempPalette, 768 );

        for( s32 i=0; i<256; i++ )
        {
            PaletteData[i] = (tempPalette[i*3+0] << 16) |
                             (tempPalette[i*3+1] << 8) | 
                             (tempPalette[i*3+2] );
        }

        delete [] tempPalette;

        file->seek( pos );
    }
    else if( header.BitsPerPixel == 4 )
    {
        PaletteData = new s32[16];
        for( s32 i=0; i<256; i++ )
        {
            PaletteData[i] = (header.Palette[i*3+0] << 16) |
                             (header.Palette[i*3+1] << 8) | 
                             (header.Palette[i*3+2]);
        }
    }

    // read image data
    s32 width, height, imagebytes;
    width = header.XMax - header.XMin + 1;
    height = header.YMax - header.YMin + 1;
    imagebytes = header.BytesPerLine * height * header.Planes * header.BitsPerPixel / 8;
    PCXData = new c8[imagebytes];

    c8 cnt, value;
    for( s32 offset = 0; offset < imagebytes; )
    {
        file->read( &cnt, 1 );
        if( !((cnt & 0xc0) == 0xc0) )
        {
            value = cnt;
            cnt = 1;
        }
        else
        {
            cnt &= 0x3f;
            file->read( &value, 1 );
        }
        while( cnt )
        {
            PCXData[offset] = value;
            cnt--;
            offset++;
        }
    }

    // create image
    IImage* image = 0;
    pitch = header.BytesPerLine - width * header.Planes * header.BitsPerPixel / 8;

    if (pitch < 0)
        pitch = -pitch;

    switch(header.BitsPerPixel)
    {
    case 8:
        image = new CImage(ECF_A1R5G5B5, core::dimension2d<s32>(width, height));
        CColorConverter::convert8BitTo16Bit(PCXData, (s16*)image->getData(), width, height, pitch, PaletteData);
        break;
    case 24:
        image = new CImage(ECF_R8G8B8, core::dimension2d<s32>(width, height));
        CColorConverter::convert24BitTo24BitFlipMirrorColorShuffle(PCXData, (c8*)image->getData(), width, height, pitch);
        break;
    };

    // clean up

    if( PaletteData )
        delete [] PaletteData;
    PaletteData = 0;

    if( PCXData )
        delete [] PCXData;
    PCXData = 0;

    return image;
}
//-----------------------------------------------------------------------------
//! загружает изображение из файла
IImage* CImageLoaderPCX::loadImage(const c8* fileName)
{   return CImageLoader::loadImage(fileName);
}
//-----------------------------------------------------------------------
//! creates a loader which is able to load tgas
IImageLoader* createImageLoaderPCX()
{
    return new CImageLoaderPCX();
}
//-----------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------
