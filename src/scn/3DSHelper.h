//|-------------------------------------------------------------------------
//| File:        3DSHelper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef MD2HelperHPP
#define MD2HelperHPP
//---------------------------------------------------------------------------

#include <my_inc.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

// byte-align structures
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

struct ChunkHeader
{
	u16 id;
	s32 length;
} PACK_STRUCT;

// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

struct ChunkData
{
	ChunkData() : read(0) {}

	ChunkHeader header;
	s32 read;
};

enum e3DSChunk
{
	// Primary chunk
	C3DS_MAIN3DS = 0x4D4D,

	// Main Chunks
	C3DS_EDIT3DS = 0x3D3D,
	C3DS_KEYF3DS = 0xB000,
	C3DS_VERSION = 0x0002,
	C3DS_MESHVERSION = 0x3D3E,

	// sub chunks of C3DS_EDIT3DS
	C3DS_EDIT_MATERIAL = 0xAFFF,
	C3DS_EDIT_OBJECT   = 0x4000,

	// sub chunks of C3DS_EDIT_MATERIAL
	C3DS_MATNAME       = 0xA000,
	C3DS_MATAMBIENT    = 0xA010,
	C3DS_MATDIFFUSE    = 0xA020,
	C3DS_MATSPECULAR   = 0xA030,
	C3DS_MATSHININESS  = 0xA040,
	C3DS_MATSHIN2PCT   = 0xA041,
	C3DS_TRANSPARENCY  = 0xA050,
	C3DS_TRANSPARENCY_FALLOFF  = 0xA052,
	C3DS_REFL_BLUR     = 0xA053,
	C3DS_TWO_SIDE      = 0xA081,
	C3DS_WIRE          = 0xA085,
	C3DS_SHADING       = 0xA100,
	C3DS_MATTEXMAP     = 0xA200,
	C3DS_MATSPECMAP    = 0xA204,
	C3DS_MATOPACMAP    = 0xA210,
	C3DS_MATREFLMAP    = 0xA220,
	C3DS_MATBUMPMAP    = 0xA230,
	C3DS_MATMAPFILE    = 0xA300,
	C3DS_MAT_TEXTILING = 0xA351,
	C3DS_MAT_USCALE    = 0xA354,
	C3DS_MAT_VSCALE    = 0xA356,
	C3DS_MAT_UOFFSET   = 0xA358,
	C3DS_MAT_VOFFSET   = 0xA35A,

	// subs of C3DS_EDIT_OBJECT
	C3DS_OBJTRIMESH    = 0x4100,

	// subs of C3DS_OBJTRIMESH
	C3DS_TRIVERT       = 0x4110,
	C3DS_POINTFLAGARRAY= 0x4111,
	C3DS_TRIFACE       = 0x4120,
	C3DS_TRIFACEMAT    = 0x4130,
	C3DS_TRIUV         = 0x4140,
	C3DS_TRISMOOTH     = 0x4150,
	C3DS_TRIMATRIX     = 0x4160,
	C3DS_MESHCOLOR     = 0x4165,
	C3DS_DIRECT_LIGHT  = 0x4600,
	C3DS_DL_INNER_RANGE= 0x4659,
	C3DS_DL_OUTER_RANGE= 0x465A,
	C3DS_DL_MULTIPLIER = 0x465B,
	C3DS_CAMERA        = 0x4700,
	C3DS_CAM_SEE_CONE  = 0x4710,
	C3DS_CAM_RANGES    = 0x4720,

	// subs of C3DS_KEYF3DS
	C3DS_KF_HDR        = 0xB00A,
	C3DS_AMBIENT_TAG   = 0xB001,
	C3DS_OBJECT_TAG    = 0xB002,
	C3DS_CAMERA_TAG    = 0xB003,
	C3DS_TARGET_TAG    = 0xB004,
	C3DS_LIGHTNODE_TAG = 0xB005,
	C3DS_KF_SEG        = 0xB008,
	C3DS_KF_CURTIME    = 0xB009,
	C3DS_KF_NODE_HDR   = 0xB010,
	C3DS_PIVOTPOINT    = 0xB013,
	C3DS_BOUNDBOX      = 0xB014,
	C3DS_MORPH_SMOOTH  = 0xB015,
	C3DS_POS_TRACK_TAG = 0xB020,
	C3DS_ROT_TRACK_TAG = 0xB021,
	C3DS_SCL_TRACK_TAG = 0xB022,
	C3DS_NODE_ID       = 0xB030,

	// Viewport definitions
	C3DS_VIEWPORT_LAYOUT = 0x7001,
	C3DS_VIEWPORT_DATA   = 0x7011,
	C3DS_VIEWPORT_DATA_3 = 0x7012,
	C3DS_VIEWPORT_SIZE   = 0x7020,

	// different color chunk types
	C3DS_COL_RGB    = 0x0010,
	C3DS_COL_TRU    = 0x0011,
	C3DS_COL_LIN_24 = 0x0012,
	C3DS_COL_LIN_F  = 0x0013,

	// percentage chunk types
	C3DS_PERCENTAGE_I = 0x0030,
	C3DS_PERCENTAGE_F = 0x0031,

	C3DS_CHUNK_MAX		= 0xFFFF
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef 3DSHPP