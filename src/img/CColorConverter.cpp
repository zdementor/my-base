//|-------------------------------------------------------------------------
//| File:        CColorConverter.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CColorConverter.h"
#include <img/SColor.h>
//---------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------
//! converts a 4 bit palettized image into R5G5B5
void CColorConverter::convert4BitTo16BitFlipMirror(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
    s32 shift = 0;
    out += width*height;
    s16* oout = out;

    for (s32 y=0; y<height; ++y)
    {
        shift = 4;

        out = oout - (y*width) - width;

        for (s32 x=0; x<width; ++x)
        {
            *out = X8R8G8B8toA1R5G5B5(palette[(u8)((*in >> shift) & 0xf)]);
            ++out;

            shift -= 4;
            if (shift<0)
            {
                shift = 4;
                ++in;
            }
        }

        if (shift !=4)
            ++in;

        in+=pitch;
    }
}
//----------------------------------------------------------------------------
//! converts a 8 bit palettized image into R5G5B5
void CColorConverter::convert8BitTo16Bit(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
    s32 lineWidth = width + pitch;
    const c8* p = in;

    for (s32 y=1; y<=height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            
            *out = X8R8G8B8toA1R5G5B5(palette[(u8)(*p)]);

            ++out;
            ++p;
        }

        p= in + y*lineWidth + pitch;
    }
}
//----------------------------------------------------------------------------
//! converts a 8 bit palettized image into R5G5B5
void CColorConverter::convert8BitTo16BitFlipMirror(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette)
{
    out += width * height;
    s32 lineWidth = width + pitch;
    const c8* p = in;

    for (s32 y=1; y<=height; ++y)
    {
        p= in + lineWidth*y - pitch;

        for (s32 x=0; x<width; ++x)
        {
            --out;
            --p;
            *out = X8R8G8B8toA1R5G5B5(palette[(u8)(*p)]);
        }
    }
}
//----------------------------------------------------------------------------
//! converts a monochrome bitmap to A1R5G5B5 data
void CColorConverter::convert1BitTo16BitFlipMirror(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    s16* p = out + width * height;

    for (s32 y=0; y<height; ++y)
    {
        s32 shift = 7;
        out = p - y * width - width;

        for (s32 x=0; x<width; ++x)
        {
            *out = *in>>shift & 0x01 ? (s16)0xffff : (s16)0x0000;
            ++out;

            --shift;
            if (shift<0)
            {
                shift=7;
                ++in;
            }
        }

        if (shift != 7)
            ++in;

        in += pitch;
    }
}
//----------------------------------------------------------------------------
//! converts R8G8B8 16 bit data to A1R5G5B5 data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert16BitTo16BitFlipMirror(
	const s16* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const s16 *p = in;
    const s32 lineWidth = width + pitch;
    out += width * height;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1);
            --out;
            *out = *p;
        }

        in += lineWidth;
    }
}
//----------------------------------------------------------------------------
//! converts R8G8B8 24 bit data to A1R5G5B5 data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert24BitTo16BitFlipMirror(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const c8 *p = in;
    const s32 lineWidth = 3 * width + pitch;
    out += width * height;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1)*3;
            --out;
            *out = RGB16(p[2], p[1], *p);
        }

        in += lineWidth;
    }
}
//----------------------------------------------------------------------------
//! copies R8G8B8 24 bit data to 24 data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert24BitTo24BitFlipMirrorColorShuffle(
	const c8* in, c8* out, s32 width, s32 height, s32 pitch)
{
	const c8 *p = in;
	const s32 lineWidth = 3 * width + pitch;
	out += width * height * 3;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			p = in + (width-x-1)*3;
			out -= 3;
			out[0] = p[2];
			out[1] = p[1];
			out[2] = p[0];
		}

		in += lineWidth;
	}
}
//----------------------------------------------------------------------------
//! �������� ������ A8R8G8B8 32 bit � 24 bit
void CColorConverter::convert32BitTo24Bit(
	const c8* in, c8* out, s32 width, s32 height, s32 pitch)
{   const s32 lineWidth = 3 * width + pitch;
    const s32 lineWidth_4 = 4*width;
    out += width * height * 3;

    for (s32 y=0; y<height; ++y)
    {   in+= lineWidth_4;
        for (s32 x=0; x<width; ++x)
        {   out -= 3;
            in-=4;
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
        }
        in+= lineWidth_4;
    }
}
//----------------------------------------------------------------------------
//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
void CColorConverter::convert24BitTo16BitColorShuffle(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const c8 *p = in;
    const s32 lineWidth = 3 * width + pitch;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1)*3;
            *out = RGB16(*p, p[1], p[2]);
            ++out;
        }

        in += lineWidth;
    }
}
//----------------------------------------------------------------------------
//! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
//! accepts colors in different order.
void CColorConverter::convert24BitTo16BitFlipColorShuffle(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const c8 *p = in;
    const s32 lineWidth = 3 * width + pitch;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (x)*3;
            *out = RGB16(*p, p[1], p[2]);
            ++out;
        }

        in += lineWidth;
    }
}
//----------------------------------------------------------------------------
//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert32BitTo16BitColorShuffle(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const c8 *p = in;
    const s32 lineWidth = 4 * width + pitch;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1)*4;
            *out = RGB16(p[2], p[1], p[0]);
            ++out;
        }

        in += lineWidth;
    }
}
//----------------------------------------------------------------------------
//! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert32BitTo16BitFlipMirrorColorShuffle(
	const c8* in, s16* out, s32 width, s32 height, s32 pitch)
{
    const c8 *p = in;
    const s32 lineWidth = 4 * width + pitch;
    out += (width+pitch) * height;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1)*4;
            --out;
            *out = RGB16(p[2], p[1], p[0]);
        }

        in += lineWidth;
    }
}

//----------------------------------------------------------------------------

//! Resizes the surface to a new size and converts it at the same time
//! to an A8R8G8B8 format, returning the pointer to the new buffer.
//! The returned pointer has to be deleted.
void CColorConverter::convert16bitToA8R8G8B8andResize(
	const s16* in, s32* out, s32 newWidth, s32 newHeight, s32 currentWidth, s32 currentHeight)
{
    if (!newWidth || !newHeight)
        return;

    // note: this is very very slow. (i didn't want to write a fast version.
    // but hopefully, nobody wants to convert surfaces every frame.

    s32 nDataSize = newWidth * newHeight;
    s32 nDataSizeInBytes = nDataSize * sizeof(s32);
    //s32* nData = new s32[nDataSize];

    f32 sourceXStep = (f32)currentWidth / (f32)newWidth;
    f32 sourceYStep = (f32)currentHeight / (f32)newHeight;
    f32 sy;
    s32 t;

    for (s32 x=0; x<newWidth; ++x)
    {
        sy = 0.0f;

        for (s32 y=0; y<newHeight; ++y)
        {
            t = in[(s32)(((s32)sy)*currentWidth + x*sourceXStep)];
            t = (((t >> 15)&0x1)<<31) | (((t >> 10)&0x1F)<<19) |
                (((t >> 5)&0x1F)<<11) | (t&0x1F)<<3;
            out[(s32)(y*newWidth + x)] = t;

            sy+=sourceYStep;
        }
    }
}

//----------------------------------------------------------------------------

//! converts 16bit data to 16bit data
void CColorConverter::convert16BitTo16Bit(
	const s16* in, s16* out, s32 width, s32 height, s32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;
		memcpy(out, in, width*sizeof(s16));

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}

//----------------------------------------------------------------------------

//! copies X8R8G8B8 32 bit data, and flips and
//! mirrors the image during the process.
void CColorConverter::convert32BitTo32BitFlipMirror(
	const s32* in, s32* out, s32 width, s32 height, s32 pitch)
{
    const s32 *p = in;
    const s32 lineWidth = width;
    out += height * width;

    for (s32 y=0; y<height; ++y)
    {
        for (s32 x=0; x<width; ++x)
        {
            p = in + (width-x-1);
            --out;
            *out = *p;
        }

        in += lineWidth;
    }
}

//---------------------------------------------------------------------------

//! copies X8R8G8B8 32 bit data
void CColorConverter::convert32BitTo32Bit(const s32* in, s32* out, s32 width, s32 height, s32 linepad, bool flip)
{
	if (!in || !out)
		return;

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;

		memcpy(out, in, width*sizeof(s32));

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}

//---------------------------------------------------------------------------

//! copies R8G8B8 24bit data to 24bit data
void CColorConverter::convert24BitTo24Bit(const u8* in, u8* out, s32 width, s32 height, s32 linepad, bool flip, bool bgr)
{
	if (!in || !out)
		return;

	const s32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;
		if (bgr)
		{
			for (s32 x=0; x<lineWidth; x+=3)
			{
				out[x+0] = in[x+2];
				out[x+1] = in[x+1];
				out[x+2] = in[x+0];
			}
		}
		else
		{
			memcpy(out,in,lineWidth);
		}
		if (!flip)
			out += lineWidth;
		in += lineWidth;
		in += linepad;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A1R5G5B5toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[0] = ((*sB >> 10) & 0x1f) << 3;
		dB[1] = ((*sB >>  5) & 0x1f) << 3;
		dB[2] = ((*sB >>  0) & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A1R5G5B5toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u32* dB = (u32 *)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toA8R8G8B8(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A1R5G5B5toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A1R5G5B5toR5G6B5(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 4);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u32* sB = (u32*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A8R8G8B8toA1R5G5B5(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[2] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 3);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u32* dB = (u32*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[0] = (0xff000000) | (sB[0] << 16) | (sB[1] << 8) | sB[2];

		sB += 3;
		dB += 1;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 3;
		s32 b = sB[2] >> 3;

		dB[0] = (0x8000) | (r << 10) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[2] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 3;
		dB += 1;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[2] = ((*sB >> 11) & 0x1f) << 3;
		dB[1] = ((*sB >>  5) & 0x3f) << 2;
		dB[0] = ((*sB >>  0) & 0x1f) << 3;

		sB += 4;
		dB += 3;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA8R8G8B8(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = R5G6B5toA1R5G5B5(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[0]=sB[2];
		dB[1]=sB[1];
		dB[2]=sB[0];

		sB += 3;
		dB += 3;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8B8G8R8toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		dB[0] = sB[3];
		dB[1] = sB[2];
		dB[2] = sB[1];

		sB += 4;
		dB += 3;
	}
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u32* sB = (u32*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A8B8G8R8toA8R8G8B8(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8B8G8R8toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u32* sB = (u32*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = A8B8G8R8toA1R5G5B5(*sB++);
}

//---------------------------------------------------------------------------

void CColorConverter::convert_A8B8G8R8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	s32 x;
	for (x = 0; x < sN; ++x)
	{
		s32 r = sB[3] >> 3;
		s32 g = sB[2] >> 2;
		s32 b = sB[1] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}

//---------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//---------------------------------------------------------------------------