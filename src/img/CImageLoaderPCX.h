//|-------------------------------------------------------------------------
//| File:        CImageLoaderPCX.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderPCXHPP
#define CImageLoaderPCXHPP
//-----------------------------------------------------------------------
#include "CImageLoader.h"
//-----------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------
// byte-align structures
#ifdef _MSC_VER
#   pragma pack( push, packing )
#   pragma pack( 1 )
#   define PACK_STRUCT
#elif defined( __GNUC__ )
#   define PACK_STRUCT  __attribute__((packed))
#elif defined( __BORLANDC__ )
#   pragma pack( push, packing )
#   pragma pack( 1 )
#   define PACK_STRUCT
#else
#   error compiler not supported
#endif
//-----------------------------------------------------------------------
struct SPCXHeader
{   u8  Manufacturer;
    u8  Version;
    u8  Encoding;
    u8  BitsPerPixel;
    u16 XMin;
    u16 YMin;
    u16 XMax;
    u16 YMax;
    u16 HorizDPI;
    u16 VertDPI;
    u8  Palette[48];
    u8  Reserved;
    u8  Planes;
    u16 BytesPerLine;
    u16 PaletteType;
    u16 HScrSize;
    u16 VScrSize;
    u8  Filler[54];
} PACK_STRUCT;
//-----------------------------------------------------------------------
// Default alignment
#ifdef _MSC_VER
    #pragma pack( pop, packing )
#elif defined (__BORLANDC__)
    #pragma pack( pop, packing )
#endif

#undef PACK_STRUCT
//-----------------------------------------------------------------------
/*!
    Image Loader for Windows PCX bitmaps.
    This loader was written and sent in by Dean P. Macri. I modified
    only some small bits of it.
*/
class CImageLoaderPCX : public CImageLoader
{
public:

    //! constructor
    CImageLoaderPCX();

    //! destructor
    virtual ~CImageLoaderPCX();

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

    c8* PCXData;
    s32* PaletteData;
};
//-----------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------
#endif

