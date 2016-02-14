//|-------------------------------------------------------------------------
//| File:        CImageDDS.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CImageDDSHPP
#define CImageDDSHPP
//-----------------------------------------------------------------------------

#include "CImage.h"

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
//  Structure defines and constants from nvdds
//////////////////////////////////////////////////////////////////////

// surface description flags
const unsigned long DDSF_CAPS           = 0x00000001l;
const unsigned long DDSF_HEIGHT         = 0x00000002l;
const unsigned long DDSF_WIDTH          = 0x00000004l;
const unsigned long DDSF_PITCH          = 0x00000008l;
const unsigned long DDSF_PIXELFORMAT    = 0x00001000l;
const unsigned long DDSF_MIPMAPCOUNT    = 0x00020000l;
const unsigned long DDSF_LINEARSIZE     = 0x00080000l;
const unsigned long DDSF_DEPTH          = 0x00800000l;

// pixel format flags
const unsigned long DDSF_ALPHAPIXELS    = 0x00000001l;
const unsigned long DDSF_FOURCC         = 0x00000004l;
const unsigned long DDSF_RGB            = 0x00000040l;
const unsigned long DDSF_RGBA           = 0x00000041l;

// dwCaps1 flags
const unsigned long DDSF_COMPLEX         = 0x00000008l;
const unsigned long DDSF_TEXTURE         = 0x00001000l;
const unsigned long DDSF_MIPMAP          = 0x00400000l;

// dwCaps2 flags
const unsigned long DDSF_CUBEMAP         = 0x00000200l;
const unsigned long DDSF_CUBEMAP_POSITIVEX  = 0x00000400l;
const unsigned long DDSF_CUBEMAP_NEGATIVEX  = 0x00000800l;
const unsigned long DDSF_CUBEMAP_POSITIVEY  = 0x00001000l;
const unsigned long DDSF_CUBEMAP_NEGATIVEY  = 0x00002000l;
const unsigned long DDSF_CUBEMAP_POSITIVEZ  = 0x00004000l;
const unsigned long DDSF_CUBEMAP_NEGATIVEZ  = 0x00008000l;
const unsigned long DDSF_CUBEMAP_ALL_FACES  = 0x0000FC00l;
const unsigned long DDSF_VOLUME          = 0x00200000l;

// compressed texture types
const unsigned long FOURCC_UNKNOWN       = 0;

#ifndef MAKEFOURCC
#define MAKEFOURCC(c0,c1,c2,c3) \
	((unsigned long)(unsigned char)(c0)| \
	((unsigned long)(unsigned char)(c1) << 8)| \
	((unsigned long)(unsigned char)(c2) << 16)| \
	((unsigned long)(unsigned char)(c3) << 24))
#endif

const unsigned long FOURCC_R8G8B8        = 20;
const unsigned long FOURCC_A8R8G8B8      = 21;
const unsigned long FOURCC_X8R8G8B8      = 22;
const unsigned long FOURCC_R5G6B5        = 23;
const unsigned long FOURCC_X1R5G5B5      = 24;
const unsigned long FOURCC_A1R5G5B5      = 25;
const unsigned long FOURCC_A4R4G4B4      = 26;
const unsigned long FOURCC_R3G3B2        = 27;
const unsigned long FOURCC_A8            = 28;
const unsigned long FOURCC_A8R3G3B2      = 29;
const unsigned long FOURCC_X4R4G4B4      = 30;
const unsigned long FOURCC_A2B10G10R10   = 31;
const unsigned long FOURCC_A8B8G8R8      = 32;
const unsigned long FOURCC_X8B8G8R8      = 33;
const unsigned long FOURCC_G16R16        = 34;
const unsigned long FOURCC_A2R10G10B10   = 35;
const unsigned long FOURCC_A16B16G16R16  = 36;

const unsigned long FOURCC_L8            = 50;
const unsigned long FOURCC_A8L8          = 51;
const unsigned long FOURCC_A4L4          = 52;
const unsigned long FOURCC_DXT1          = (MAKEFOURCC('D','X','T','1'));
const unsigned long FOURCC_DXT2          = (MAKEFOURCC('D','X','T','2'));
const unsigned long FOURCC_DXT3          = (MAKEFOURCC('D','X','T','3'));
const unsigned long FOURCC_DXT4          = (MAKEFOURCC('D','X','T','4'));
const unsigned long FOURCC_DXT5          = (MAKEFOURCC('D','X','T','5'));
const unsigned long FOURCC_ATI1          = (MAKEFOURCC('A','T','I','1'));
const unsigned long FOURCC_ATI2          = (MAKEFOURCC('A','T','I','2'));

const unsigned long FOURCC_D16_LOCKABLE  = 70;
const unsigned long FOURCC_D32           = 71;
const unsigned long FOURCC_D24X8         = 77;
const unsigned long FOURCC_D16           = 80;

const unsigned long FOURCC_D32F_LOCKABLE = 82;

const unsigned long FOURCC_L16           = 81;

// Floating point surface formats

// s10e5 formats (16-bits per channel)
const unsigned long FOURCC_R16F          = 111;
const unsigned long FOURCC_G16R16F       = 112;
const unsigned long FOURCC_A16B16G16R16F = 113;

// IEEE s23e8 formats (32-bits per channel)
const unsigned long FOURCC_R32F          = 114;
const unsigned long FOURCC_G32R32F       = 115;
const unsigned long FOURCC_A32B32G32R32F = 116;

struct DXTColBlock
{
	u16 col0;
	u16 col1;
	u8 row[4];
};

struct DXT3AlphaBlock
{
	u16 row[4];
};

struct DXT5AlphaBlock
{
	u8 alpha0;
	u8 alpha1;
	u8 row[6];
};

struct DDS_PIXELFORMAT
{
    unsigned long dwSize;
    unsigned long dwFlags;
    unsigned long dwFourCC;
    unsigned long dwRGBBitCount;
    unsigned long dwRBitMask;
    unsigned long dwGBitMask;
    unsigned long dwBBitMask;
    unsigned long dwABitMask;
};

struct DDS_HEADER
{
    unsigned long dwSize;
    unsigned long dwFlags;
    unsigned long dwHeight;
    unsigned long dwWidth;
    unsigned long dwPitchOrLinearSize;
    unsigned long dwDepth;
    unsigned long dwMipMapCount;
    unsigned long dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    unsigned long dwCaps1;
    unsigned long dwCaps2;
    unsigned long dwReserved2[3];
};

//-----------------------------------------------------------------------------

//! Class for working with DDS images
class CImageDDS : public CImage
{
	friend class CImageLoaderDDS;
public:

	CImageDDS();
	CImageDDS(E_COLOR_FORMAT format, const core::dimension2di &size, s32 depth);
	CImageDDS(E_COLOR_FORMAT format, const core::dimension2di &size,
		void *data, bool own_data);
	CImageDDS(E_COLOR_FORMAT format, const core::dimension2di &size,
		u32 data_size, void *data, bool own_data);
	virtual ~CImageDDS();

	virtual void* getLevelData(u32 level = 0, u32 face = 0);
	virtual u32 getLevelDataSizeBytes(u32 level = 0) const;
	virtual u32 getLevelCount() const { return m_LevelCount; }
	virtual u32 getFaceCount() const { return m_Faces; }

	virtual u32 getDepth() const { return m_Depth; }

    virtual bool isCubeMap() const { return m_Faces > 0; }
    virtual bool isVolume() const { return m_Depth > 0; }

	void flip();

protected:

	u32					m_Depth;
	u32					m_LevelCount;
	u32					m_Faces;

	//pointers to the levels
	core::array <u8*> m_LevelData;

	void _freeData();

	void _flipSurface(u8 *surf, s32 width, s32 height, s32 depth);

	// flip a DXT1 color block
	static void flip_blocks_dxtc1(u8 *ptr, u32 numBlocks);

	// flip a DXT3 color block
	static void flip_blocks_dxtc3(u8 *ptr, u32 numBlocks);

	// flip a DXT5 alpha block
	static void flip_dxt5_alpha(DXT5AlphaBlock *block);

	// flip a DXT5 color block
	static void flip_blocks_dxtc5(u8 *ptr, u32 numBlocks);
};

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------
#endif

