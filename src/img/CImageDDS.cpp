//|-------------------------------------------------------------------------
//| File:        CImageDDS.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CImageDDS.h"
#include <img/SColor.h>
#include <io/ILogger.h>

//-----------------------------------------------------------------------------
namespace my {
namespace img {
//-----------------------------------------------------------------------------

CImageDDS::CImageDDS()
	: CImage(),
	m_Depth(0), m_LevelCount(0), m_Faces(0)
{
#if MY_DEBUG_MODE  
	IUnknown::setClassName("CImageDDS CImage");
#endif
}

//-----------------------------------------------------------------------------

CImageDDS::CImageDDS(E_COLOR_FORMAT format,
	const core::dimension2di& size, s32 depth
	) : CImage(format, size, NULL, false),
	m_Depth(0), m_LevelCount(0), m_Faces(0)
{}

//----------------------------------------------------------------------------

CImageDDS::CImageDDS(E_COLOR_FORMAT format,
	const core::dimension2di &size, void *data, bool own_data
	) : CImage(format, size, data, own_data),
	m_Depth(0), m_LevelCount(0), m_Faces(0)
{}

//----------------------------------------------------------------------------

CImageDDS::CImageDDS(E_COLOR_FORMAT format,
	const core::dimension2di &size, u32 data_size, void *data, bool own_data
	) : CImage(format, size, data_size, data, own_data),
	m_Depth(0), m_LevelCount(0), m_Faces(0)
{}

//-----------------------------------------------------------------------------

CImageDDS::~CImageDDS()
{
	_freeData();
}

//-----------------------------------------------------------------------------

u32 CImageDDS::getLevelDataSizeBytes(u32 level) const
{
    bool compressed = isCompressed();
	u32 w = getDimension().Width >> level;
    u32 h = getDimension().Height >> level;
    u32 d = m_Depth >> level;
    w = (w) ? w : 1;
    h = (h) ? h : 1;
    d = (d) ? d : 1;
    u32 bw = (compressed) ? ( w + 3 ) / 4 : w;
    u32 bh = (compressed) ? ( h + 3 ) / 4 : h;
    u32 elementSize = m_BytesPerPixel;
    return bw*bh*d*elementSize;
}

//-----------------------------------------------------------------------------

void* CImageDDS::getLevelData(u32 level, u32 face)
{
	if (level >= m_LevelCount)
		return NULL;
    if (face > 0 && face >= m_Faces)
		return NULL;
    if ( (face*m_LevelCount + level) >= (int)m_LevelData.size())
		return NULL;
    return m_LevelData[face*m_LevelCount + level];
}

//-----------------------------------------------------------------------------

void CImageDDS::_freeData()
{
	for (u32 i = 0; i < m_LevelData.size(); i++)
		delete m_LevelData[i];
	m_LevelData.clear();
}

//-----------------------------------------------------------------------------

void CImageDDS::flip()
{
	bool btcCompressed = isCompressed();
	u32 bytesPerElement = getBytesPerPixel();
	for (u32 face = 0; face < (m_Faces ? m_Faces : 1); face++)
	{
		int w = m_Dim.Width, h = m_Dim.Height, d = (m_Depth) ? m_Depth : 1;
		for (u32 level = 0; level < m_LevelCount; level++)
		{
			int bw = (btcCompressed) ? (w+3)/4 : w;
			int bh = (btcCompressed) ? (h+3)/4 : h;
			int size = bw*bh*d*bytesPerElement;

			u8 *data = (u8*)getLevelData(level, face);
			_flipSurface(data, w, h, d);

			//reduce mip sizes
			w = ( w > 1) ? w >> 1 : 1;
			h = ( h > 1) ? h >> 1 : 1;
			d = ( d > 1) ? d >> 1 : 1;
		}
	}
}

//-----------------------------------------------------------------------------

void CImageDDS::_flipSurface(u8 *surf, s32 width, s32 height, s32 depth)
{
	unsigned int lineSize;

	depth = (depth) ? depth : 1;

	if (!isCompressed())
	{
		lineSize = m_BytesPerPixel * width;
		unsigned int sliceSize = lineSize * height;

		u8 *tempBuf = new u8[lineSize];

		for ( int ii = 0; ii < depth; ii++)
		{
			u8 *top = surf + ii*sliceSize;
			u8 *bottom = top + (sliceSize - lineSize);
	
			for ( int jj = 0; jj < (height >> 1); jj++)
			{
				memcpy(tempBuf, top, lineSize);
				memcpy(top, bottom, lineSize);
				memcpy(bottom, tempBuf, lineSize);

				top += lineSize;
				bottom -= lineSize;
			}
		}

		delete []tempBuf;
	}
	else
	{
		void (*flipblocks)(u8*, unsigned int);
		width = (width + 3) / 4;
		height = (height + 3) / 4;
		u32 blockSize = 0;

		switch (getColorFormat())
		{
			case ECF_DXT1:
				blockSize = 8;
				flipblocks = &flip_blocks_dxtc1; 
				break;
			case ECF_DXT3: 
				blockSize = 16;
				flipblocks = &flip_blocks_dxtc3; 
				break;
			case ECF_DXT5: 
				blockSize = 16;
				flipblocks = &flip_blocks_dxtc5; 
				break;
			default:
				return;
		}

		lineSize = width * blockSize;
		u8 *tempBuf = new u8[lineSize];

		u8 *top = surf;
		u8 *bottom = surf + (height-1) * lineSize;

		for (u32 j = 0; j < max( (u32)height >> 1, (u32)1); j++)
		{
			if (top == bottom)
			{
				flipblocks(top, width);
				break;
			}

			flipblocks(top, width);
			flipblocks(bottom, width);

			memcpy( tempBuf, top, lineSize);
			memcpy( top, bottom, lineSize);
			memcpy( bottom, tempBuf, lineSize);

			top += lineSize;
			bottom -= lineSize;
		}
		delete []tempBuf;
	}
}

//-----------------------------------------------------------------------------

void CImageDDS::flip_blocks_dxtc1(u8 *ptr, u32 numBlocks)
{
	DXTColBlock *curblock = (DXTColBlock*)ptr;
	u8 temp;

	for (u32 i = 0; i < numBlocks; i++)
	{
		temp = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = temp;
		temp = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = temp;

		curblock++;
	}
}

//-----------------------------------------------------------------------------

void CImageDDS::flip_blocks_dxtc3(u8 *ptr, u32 numBlocks)
{
	DXTColBlock *curblock = (DXTColBlock*)ptr;
	DXT3AlphaBlock *alphablock;
	u16 tempS;
	u8 tempB;

	for (u32 i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT3AlphaBlock*)curblock;

		tempS = alphablock->row[0];
		alphablock->row[0] = alphablock->row[3];
		alphablock->row[3] = tempS;
		tempS = alphablock->row[1];
		alphablock->row[1] = alphablock->row[2];
		alphablock->row[2] = tempS;

		curblock++;

		tempB = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = tempB;
		tempB = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = tempB;

		curblock++;
	}
}

//-----------------------------------------------------------------------------

void CImageDDS::flip_dxt5_alpha(DXT5AlphaBlock *block)
{
	u8 gBits[4][4];
	
	const unsigned long mask = 0x00000007;          // bits = 00 00 01 11
	unsigned long bits = 0;
	memcpy(&bits, &block->row[0], sizeof(unsigned char) * 3);

	gBits[0][0] = (u8)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (u8)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (u8)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (u8)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (u8)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (u8)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (u8)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (u8)(bits & mask);

	bits = 0;
	memcpy(&bits, &block->row[3], sizeof(u8) * 3);

	gBits[2][0] = (u8)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (u8)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (u8)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (u8)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (u8)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (u8)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (u8)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (u8)(bits & mask);

	// clear existing alpha bits
	memset(block->row, 0, sizeof(u8) * 6);

	u32 *pBits = ((u32*) &(block->row[0]));

	*pBits = *pBits | (gBits[3][0] << 0);
	*pBits = *pBits | (gBits[3][1] << 3);
	*pBits = *pBits | (gBits[3][2] << 6);
	*pBits = *pBits | (gBits[3][3] << 9);

	*pBits = *pBits | (gBits[2][0] << 12);
	*pBits = *pBits | (gBits[2][1] << 15);
	*pBits = *pBits | (gBits[2][2] << 18);
	*pBits = *pBits | (gBits[2][3] << 21);

	pBits = ((u32*) &(block->row[3]));

	*pBits = *pBits | (gBits[1][0] << 0);
	*pBits = *pBits | (gBits[1][1] << 3);
	*pBits = *pBits | (gBits[1][2] << 6);
	*pBits = *pBits | (gBits[1][3] << 9);

	*pBits = *pBits | (gBits[0][0] << 12);
	*pBits = *pBits | (gBits[0][1] << 15);
	*pBits = *pBits | (gBits[0][2] << 18);
	*pBits = *pBits | (gBits[0][3] << 21);
}

//-----------------------------------------------------------------------------

void CImageDDS::flip_blocks_dxtc5(u8 *ptr, u32 numBlocks)
{
	DXTColBlock *curblock = (DXTColBlock*)ptr;
	DXT5AlphaBlock *alphablock;
	u8 temp;
	
	for (unsigned int i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT5AlphaBlock*)curblock;
	    
		flip_dxt5_alpha(alphablock);

		curblock++;

		temp = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = temp;
		temp = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = temp;

		curblock++;
	}
}

//-----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//-----------------------------------------------------------------------------

