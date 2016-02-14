//|-------------------------------------------------------------------------
//| File:        CImageLoaderTGA.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageLoaderTGAHPP
#define CImageLoaderTGAHPP
//-------------------------------------------------------------
#include "CImageLoader.h"
//-------------------------------------------------------------
namespace my {
namespace img {
//-------------------------------------------------------------
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

//-------------------------------------------------------------

// tga header
struct STGAHeader
{   
	u8 IdLength;
    u8 ColorMapType;
    u8 ImageType;
    u8 FirstEntryIndex[2];
    u16 ColorMapLength;
    u8 ColorMapEntrySize;
    u8 XOrigin[2];
    u8 YOrigin[2];
    u16 ImageWidth;
    u16 ImageHeight;
    u8 PixelDepth;
    u8 ImageDescriptor;
} 
PACK_STRUCT;

//-------------------------------------------------------------

struct STGAFooter
{
	u32 ExtensionOffset;
	u32 DeveloperOffset;
	c8  Signature[18];
} 
PACK_STRUCT;

//-------------------------------------------------------------
// Default alignment
#ifdef _MSC_VER
    #pragma pack( pop, packing )
#elif defined (__BORLANDC__)
    #pragma pack( pop, packing )
#endif

#undef PACK_STRUCT
//-------------------------------------------------------------
//! Surface Loader for targa images
class CImageLoaderTGA : public CImageLoader
{
public:

    //! constructor
    CImageLoaderTGA();

    //! destructor
    virtual ~CImageLoaderTGA();

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

    //! loads a compressed tga. Was written and sent in by Jon Pry, thank you very much!
    c8* loadCompressedImage(io::IReadFile *file, STGAHeader *header);

};
//-------------------------------------------------------------
} // end namespace img
} // end namespace my
//-------------------------------------------------------------
#endif

