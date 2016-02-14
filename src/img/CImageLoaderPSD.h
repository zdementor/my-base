//|-------------------------------------------------------------------------
//| File:        CImageLoaderPSD.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderPSDHPP
#define CImageLoaderPSDHPP
//--------------------------------------------------------------------
#include "CImageLoader.h"
//--------------------------------------------------------------------
namespace my {
namespace img {
//--------------------------------------------------------------------
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
//--------------------------------------------------------------------
struct PsdHeader
{   c8 signature [4];   // Always equal to 8BPS.
    u16 version;        // Always equal to 1
    c8 reserved [6];    // Must be zero
    u16 channels;   // Number of any channels inc. alphas
    u32 height;     // Rows Height of image in pixel
    u32 width;      // Colums Width of image in pixel
    u16 depth;      // Bits/channel
    u16 mode;       // Color mode of the file (Bitmap/Grayscale..)
} PACK_STRUCT;
//--------------------------------------------------------------------
// Default alignment
#ifdef _MSC_VER
    #pragma pack( pop, packing )
#elif defined (__BORLANDC__)
    #pragma pack( pop, packing )
#endif

#undef PACK_STRUCT
//--------------------------------------------------------------------
//! Surface Loader for psd images
class CImageLoaderPSD : public CImageLoader
{
public:

    //! constructor
    CImageLoaderPSD();

    //! destructor
    virtual ~CImageLoaderPSD();

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

    bool readRawImageData(io::IReadFile* file);
    bool readRLEImageData(io::IReadFile* file);
    c8 getShiftFromChannel(c8 channelNr); 

    inline u32 convert2le (u32 value)
    {
        value = (value >> 16) | (value << 16);
        value = ((value >> 8) & 0xFF00FF) | ((value << 8) & 0xFF00FF00);
        return value;
    }

    inline u16 convert2le (u16 value)
    {
        value = (value >> 8) | (value << 8);
        return value;
    }

    // member variables
    PsdHeader header;
    u32* imageData;
    bool error;

};
//--------------------------------------------------------------------
} // end namespace video
} // end namespace irr
//--------------------------------------------------------------------
#endif

