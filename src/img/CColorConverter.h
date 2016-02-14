//|-------------------------------------------------------------------------
//| File:        CColorConverter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CColorConverterHPP
#define CColorConverterHPP
//---------------------------------------------------------------------------
#include <my_inc.h>
//---------------------------------------------------------------------------
namespace my {
namespace img {
//---------------------------------------------------------------------------
class CColorConverter
{
public:

    //! converts a 4 bit palettized image into R5G5B5
    static void convert4BitTo16BitFlipMirror(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

    //! converts a 8 bit palettized image into R5G5B5
    static void convert8BitTo16BitFlipMirror(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

    //! converts a 8 bit palettized image into R5G5B5
    static void convert8BitTo16Bit(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch, const s32* palette);

    //! converts a monochrome bitmap to A1R5G5B5 data
    static void convert1BitTo16BitFlipMirror(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! converts R8G8B8 24 bit data to A1R5G5B5 data, and flips and
    //! mirrors the image during the process.
    static void convert24BitTo16BitFlipMirror(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! converts R8G8B8 16 bit data to A1R5G5B5 data, and flips and
    //! mirrors the image during the process.
    static void convert16BitTo16BitFlipMirror(
		const s16* in, s16* out, s32 width, s32 height, s32 pitch);

    //! copies R8G8B8 24 bit data to 24 data, and flips and
    //! mirrors the image during the process.
    static void convert24BitTo24BitFlipMirrorColorShuffle(
		const c8* in, c8* out, s32 width, s32 height, s32 pitch_in);

    //! �������� ������ A8R8G8B8 32 bit � 24 bit
    static void convert32BitTo24Bit(
		const c8* in, c8* out, s32 width, s32 height, s32 pitch);

    //! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
    //! accepts colors in different order.
    static void convert24BitTo16BitColorShuffle(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! converts R8G8B8 24 bit data to A1R5G5B5 data (used e.g for JPG to A1R5G5B5)
    //! accepts colors in different order.
    static void convert24BitTo16BitFlipColorShuffle(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
    //! accepts colors in different order.
    static void convert32BitTo16BitColorShuffle(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! converts X8R8G8B8 32 bit data to A1R5G5B5 data, and flips and
    //! mirrors the image during the process, accepts colors in different order.
    static void convert32BitTo16BitFlipMirrorColorShuffle(
		const c8* in, s16* out, s32 width, s32 height, s32 pitch);

    //! Resizes the surface to a new size and converts it at the same time
    //! to an A8R8G8B8 format, returning the pointer to the new buffer.
    //! The returned pointer has to be deleted.
    static void convert16bitToA8R8G8B8andResize(
		const s16* in, s32* out, s32 newWidth, s32 newHeight, s32 currentWidth, s32 currentHeight);

    //! copies X8R8G8B8 32 bit data, and flips and
    //! mirrors the image during the process.
    static void convert32BitTo32BitFlipMirror(
		const s32* in, s32* out, s32 width, s32 height, s32 pitch);

	//! copies X8R8G8B8 32 bit data, and flips and 
	//! mirrors the image during the process.
	static void convert32BitTo32Bit(
		const s32* in, s32* out, s32 width, s32 height, s32 linepad,
		bool flip=false);

	//! converts R8G8B8 16 bit data to A1R5G5B5 data
	static void convert16BitTo16Bit(const s16* in, s16* out, s32 width, s32 height, s32 linepad=0,
		bool flip=false);

	//! copies R8G8B8 24 bit data to 24 data, and flips and 
	//! mirrors the image during the process.
	static void convert24BitTo24Bit(
		const u8* in, u8* out, s32 width, s32 height, s32 linepad=0,
		bool flip=false, bool bgr=false);

	//! functions for converting one image format to another efficiently
	//! and hopefully correctly.
	//!
	//! \param sP pointer to source pixel data
	//! \param sN number of source pixels to copy
	//! \param dP pointer to destination data buffer. must be big enough
	//! to hold sN pixels in the output format.
	static void convert_A1R5G5B5toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_A8R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP);

	static void convert_R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP);

	static void convert_A8B8G8R8toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8B8G8R8toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_A8B8G8R8toR5G6B5(const void* sP, s32 sN, void* dP);
};

//---------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//---------------------------------------------------------------------------

#endif

