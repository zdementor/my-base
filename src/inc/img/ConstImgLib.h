//|-------------------------------------------------------------------------
//| File:        ConstImgLib.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_imgHPP
#define my_const_imgHPP

//----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//! An enum for the color format of textures used by the Irrlicht Engine.
//! A color format specifies how color information is stored. 
enum E_COLOR_FORMAT
{ 
	// Empty color format
	ECF_NONE = 0,

	//! 16 bit color format used by the software driver, and thus preferred 
	//! by all other irrlicht engine video drivers. There are 5 bits for every  
	//! color component, and a single bit is left for alpha information.    
	ECF_A1R5G5B5,

	//! Standard 16 bit color format.   
	ECF_R5G6B5, 

	//! 24 bit color, no alpha channel, but 8 bit for red, green and blue.  
	ECF_R8G8B8, 

	//! Default 32 bit color format. 8 bits are used for every component:   
	//! alpha, red, green and blue. This is a Direct3D 32 bit color format.
	ECF_A8R8G8B8,

	//! OpenGL 32 bit color format. 8 bits are used for every component:   
	//! alpha blue, green and red. 
	ECF_A8B8G8R8,

	//! Compressed DDS formats
	ECF_DXT1,
    ECF_DXT3,
	ECF_DXT5,

	//! Luminance formats (for the heightmaps)
	ECF_LUMINANCE16,

	//! 16 bit per channel format
	ECF_A16B16G16R16,

	//! Depth formats
	ECF_DEPTH24_STENCIL8,

	//! Single channel 32 bit float format
	ECF_LUMINANCE32F,

	//! 16 bit per channel float format
	ECF_A16B16G16R16F,

	//! 32 bit per channel float format
	ECF_A32B32G32R32F,

	E_COLOR_FORMAT_COUNT,

	E_COLOR_FORMAT_FORCE_32BIT = 0xffffffff
};

static const c8* ColorFormatName[E_COLOR_FORMAT_COUNT] =
{
	"ECF_NONE",
	"ECF_A1R5G5B5",
	"ECF_R5G6B5", 
	"ECF_R8G8B8", 
	"ECF_A8R8G8B8",
	"ECF_A8B8G8R8",
	"ECF_DXT1",
    "ECF_DXT3",
	"ECF_DXT5",
	"ECF_LUMINANCE16",
	"ECF_A16B16G16R16",
	"ECF_DEPTH24_STENCIL8",
	"ECF_LUMINANCE32F",
	"ECF_A16B16G16R16F",
	"ECF_A32B32G32R32F",
};

static u32 ColorFormatBitsPerPixel[E_COLOR_FORMAT_COUNT] =
{ 
	0,  //ECF_NONE
	16, //ECF_A1R5G5B5
	16, //ECF_R5G6B5
	24, //ECF_R8G8B8
	32, //ECF_A8R8G8B8
	32, //ECF_A8B8G8R8
	0,  //ECF_DXT1
    0,  //ECF_DXT3
	0,  //ECF_DXT5
	16, //ECF_LUMINANCE16
	64, //ECF_A16B16G16R16
	32, //ECF_DEPTH24_STENCIL8
	32, //ECF_LUMINANCE32F
	64, //ECF_A16B16G16R16F
	128,//ECF_A32B32G32R32F
};

MY_FORCEINLINE const c8* getColorFormatName(E_COLOR_FORMAT fmt)
{ return ColorFormatName[fmt]; }

MY_FORCEINLINE u32 getBitsPerPixelFromFormat(E_COLOR_FORMAT fmt)
{ return ColorFormatBitsPerPixel[fmt]; }

//----------------------------------------------------------------------------

enum E_IMAGE_OPERATION
{
	EIO_ADD = 0,
	EIO_MUL,
	EIO_SUB,

	E_IMAGE_OPERATION_COUNT,

	E_IMAGE_OPERATION_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_imgHPP