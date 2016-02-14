//|-------------------------------------------------------------------------
//| File:        CImageLoaderBMP.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderBMPHPP
#define CImageLoaderBMPHPP
//-----------------------------------------------------------------------------
#include "CImageLoader.h"
//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
//BMP хедер
struct SBMPHeader
{   u16 Id;                 //  BM (0x4d42)- Windows 3.1x, 95, NT, 98, 2000, ME, XP
                            //  BA         - OS/2 Bitmap Array
                            //  CI         - OS/2 Color Icon
                            //  CP         - OS/2 Color Pointer
                            //  IC         - OS/2 Icon
                            //  PT         - OS/2 Pointer
    u32 FileSize;
    u32 Reserved;
    u32 BitmapDataOffset;
    u32 BitmapHeaderSize;   // should be 28h for windows bitmaps or
                            // 0Ch for OS/2 1.x or F0h for OS/2 2.x
    u32 Width;
    u32 Height;
    u16 Planes;
    u16 BPP;                // 1: Monochrome bitmap
                            // 4: 16 color bitmap
                            // 8: 256 color bitmap
                            // 16: 16bit (high color) bitmap
                            // 24: 24bit (true color) bitmap
                            // 32: 32bit (true color) bitmap

    u32  Compression;       // 0: none (Also identified by BI_RGB)
                            // 1: RLE 8-bit / pixel (Also identified by BI_RLE4)
                            // 2: RLE 4-bit / pixel (Also identified by BI_RLE8)
                            // 3: Bitfields  (Also identified by BI_BITFIELDS)

    u32  BitmapDataSize;    // Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
    u32  PixelPerMeterX;
    u32  PixelPerMeterY;
    u32  Colors;
    u32  ImportantColors;
} PACK_STRUCT;
//-----------------------------------------------------------------------------
// Default alignment
#ifdef _MSC_VER
    #pragma pack( pop, packing )
#elif defined (__BORLANDC__)
    #pragma pack( pop, packing )
#endif

#undef PACK_STRUCT
//-----------------------------------------------------------------------------
//!  Surface Loader fow Windows bitmaps
class CImageLoaderBMP : public CImageLoader
{
public:
    //! constructor
    CImageLoaderBMP();

    //! destructor
    virtual ~CImageLoaderBMP();

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

    void decompress8BitRLE(c8*& BmpData, s32 size, s32 width, s32 height, s32 pitch);

    void decompress4BitRLE(c8*& BmpData, s32 size, s32 width, s32 height, s32 pitch);

    c8* BmpData;
    s32* PaletteData;

};
//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
#endif

