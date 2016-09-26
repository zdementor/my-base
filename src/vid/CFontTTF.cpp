//|-------------------------------------------------------------------------
//| File:        CFontTTF.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CFontTTF.h"

#ifndef __MY_BUILD_VID_NULL_LIB__

#include <img/IImage.h>
#include <io/ILogger.h>
#include <img/IImageLibrary.h>
#include <res/IResourceManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

CGlyphTTF::CGlyphTTF()
{
#if MY_DEBUG_MODE  
	setClassName("CGlyphTTF");
#endif

	tex = NULL;
}

//----------------------------------------------------------------------------

CGlyphTTF::~CGlyphTTF()
{
	SAFE_DROP(tex);
}

//----------------------------------------------------------------------------

u32 CGlyphTTF::getWidth(const CGlyphTTF &glyph)
{
	return glyph.texw;
}

//----------------------------------------------------------------------------

void CGlyphTTF::cache(u32 idx)
{
	FT_Set_Pixel_Sizes(*face, 0, size);

	if (0 == FT_Load_Char(*face, idx,
			FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL))
	{
		FT_GlyphSlot glyph = (*face)->glyph;
		FT_Bitmap  bits;

		bits = glyph->bitmap;
		u8 *pt = bits.buffer;

		top = glyph->bitmap_top;
		left = glyph->bitmap_left;
		texw = bits.width;
		texh = bits.rows;

		u32 *texd = new u32[texw * texh];
		memset(texd, 0, texw*texh*sizeof(u32));
		u32 *texp = texd;
		offset = size - bits.rows;

		for (s32 i = 0; i < bits.rows; i++)
        {	
			u32 *rowp = texp;
			for (s32 j = 0; j < bits.width; j++)
            {	
				if (*pt)
                {	
					*rowp = *pt << 24;
					*rowp |= 0xffffff;							
				} 
				else 
					*rowp = 0;
				pt++;
				rowp++;
			}
			texp += texw;
		}

		c8 name[128];
		sprintf(name,"TTF-Font Glyph %d",idx);

		img::IImage *img = IMAGE_LIBRARY.createImageFromData(
			img::ECF_A8R8G8B8, core::dimension2d<s32>(texw,texh), 0, texd);
		
		vid::IVideoDriver &driver = VIDEO_DRIVER;
		bool flgmip = driver.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);   
		CFontTTF::STTFTexture & ttftex = font->m_Textures[size];

		vid::ITexture *_tex = ttftex.Textures.size() ?
			ttftex.Textures[ttftex.Textures.size()-1] : NULL;

		driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, false);

		core::dimension2di dim(texw, texh);
		core::position2di pos;
		if (!_tex || !ttftex.DataNode.place(NULL, dim, &pos))
		{
			/*
			if (_tex)
			{
				img::IImage * _img = _tex->lock();
				sprintf(name,"TTF-Font_%d.tga",idx);
				IMAGE_LIBRARY.saveImageToFile(_img, name);
				_tex->unlock();
			}
			*/

			_tex = driver.addTexture(name, ttftex.getTexDim(), img::ECF_A8R8G8B8);  
			ttftex.Textures.push_back(_tex);
			SAFE_GRAB(_tex);
								
			ttftex.DataNode.reset();
			if (!ttftex.DataNode.place(NULL, dim, &pos))
			{
				LOGGER.logErr("Can't cache %d glyph in ttf-font", idx);
				return;
			}
		}
		tex = _tex;

		img::IImage * _img = tex->lock();
		img->copyTo(_img, pos, core::recti(0, 0, texw - 1, texh - 1));
		img->drop();
		tex->unlock();

		driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS, flgmip);

		core::dimension2di texsize = tex->getSize();
		tcoord.set(
			f32(pos.X) / (f32)(texsize.Width),
			f32(pos.Y) / (f32)(texsize.Height),
			f32(pos.X + texw) / (f32)(texsize.Width),
			f32(pos.Y + texh) / (f32)(texsize.Height));

		if (ttftex.CharDim.Width < (s32)texw)
			ttftex.CharDim.Width = (s32)texw;
		if (ttftex.CharDim.Height < (s32)texh)
			ttftex.CharDim.Height = (s32)texh;

		cached = true;
	}

	SAFE_GRAB(tex);
}

//----------------------------------------------------------------------------

CFontTTF::CFontTTF()
	: TransParency(true), Size(-1), m_MonospaceMode(false)
{
#if MY_DEBUG_MODE  
	setClassName("CFontTTF");
#endif
}

//----------------------------------------------------------------------------

CFontTTF::~CFontTTF() {}

//----------------------------------------------------------------------------

CFaceTTF::CFaceTTF()
{
	if (FT_Init_FreeType(&library))
    {	
		LOGGER.log("Failed to initialise the FreeType library!", io::ELL_ERROR);
	}
}

//----------------------------------------------------------------------------

CFaceTTF::~CFaceTTF()
{
	FT_Done_FreeType(library);
}

//----------------------------------------------------------------------------

bool CFaceTTF::load(const c8 * filename)
{	
	if (FT_New_Face(library, filename, 0, &face))
		return	false;
	return true;
}

//----------------------------------------------------------------------------

bool CFontTTF::setSize(u32 size)
{   
	if (Size == size)
		return true;

	CHECK_RANGE(size, 1, FONT_TTF_MAX_SIZE - 1);

    Size = size;

	core::array <CGlyphTTF> & glyphs = m_Textures[Size].Glyphs;

	if (glyphs.size() != TT_face.face->num_glyphs)
	{
		glyphs.reallocate ( TT_face.face->num_glyphs );
		glyphs.set_used ( TT_face.face->num_glyphs );
		for (s32 i = 0; i < TT_face.face->num_glyphs; i++)
		{
			glyphs[i].font		= this;
			glyphs[i].size		= Size;
			glyphs[i].face		= &(TT_face.face);
			glyphs[i].cached	= false;
		}
	}

	// to cache first symbol
	_getGlyphByChar(L'A');

	return	true;
}

//----------------------------------------------------------------------------

s32 CFontTTF::getSize()
{ return Size; }

//----------------------------------------------------------------------------

bool CFontTTF::load(const c8* _filename)
{   
	core::stringc filename = _filename;

	core::stringc fname = core::extractFileName(filename);
	core::stringc fpath = core::extractFilePath(filename);
	core::stringc fonts_path = 
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_FONTS);  

	core::stringc rel_path =
		core::getRelativePathBetweenDirectories(fonts_path, fpath);

	m_FileName=rel_path;
	m_FileName.append(fname);

    if (!TT_face.load(_filename))
		return false;

	u32 i;
	for (i = 0; i < FONT_TTF_MAX_SIZE; i++)
		m_Textures[i].clear();

    setSize(12);

    return true;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE u32 CFontTTF::_getGlyphByChar(wchar_t c)
{
	u32 idx = m_Textures[Size].GlyphsHash.get_value(c);
	if (idx)
		return idx;
	idx = (u16)c;
	if (idx)
	{
		CGlyphTTF & glyph = m_Textures[Size].Glyphs[idx - 1];
		if (!glyph.cached)
			glyph.cache(idx);
		m_Textures[Size].GlyphsHash.set_value(c, idx);
	}
	return idx;
}

//----------------------------------------------------------------------------

core::dimension2di CFontTTF::getDimension(const wchar_t* text)
{
	core::dimension2d<s32> dim ( 0 ,0 );

	if ( Size <= 0 || Size >= FONT_TTF_MAX_SIZE )
		return dim;

	dim.Height = m_Textures[Size].Glyphs[0].size;
	for (const wchar_t* p = text; *p; ++p)
		dim.Width += _getWidthFromCharacter(*p);

	return dim;
}

//----------------------------------------------------------------------------

core::dimension2di CFontTTF::getDimension(u32 text_len)
{
	static core::dimension2d<s32> dim;

	dim.Height = dim.Width = 0;

	if (Size <= 0 || Size >= FONT_TTF_MAX_SIZE)
		return dim;

	dim.Height= m_Textures[Size].Glyphs[0].size;
	dim.Width = _getWidthFromCharacter(L'A') * text_len;

	return dim;
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 CFontTTF::_getWidthFromCharacterIdx(u32 n)
{
	core::array < CGlyphTTF > & glyphs = m_Textures[Size].Glyphs;
	if (n > 0)
		return CGlyphTTF::getWidth(glyphs[n - 1]);
	return glyphs[0].size / 2;	
}

//----------------------------------------------------------------------------

MY_FORCEINLINE s32 CFontTTF::_getWidthFromCharacter(wchar_t c)
{
	if (m_MonospaceMode)
		return m_Textures[Size].CharDim.Width;
	u32 n = _getGlyphByChar(c);
	return _getWidthFromCharacterIdx(n);
}

//----------------------------------------------------------------------------

void CFontTTF::registerForRendering(
    const wchar_t* text, u32 size, const core::recti& position, 
    img::SColor color, bool hcenter, bool vcenter,
	const core::recti * clipRect)
{
	if (Size <= 0 || Size >= FONT_TTF_MAX_SIZE)
		return;

	vid::IVideoDriver & driver = IVideoDriver::getSingleton();

	static core::dimension2di textDimension;
	static core::position2df offset;

	offset = position.UpperLeftCorner;

	if (!TransParency) 
		color.color |= 0xff000000; 

    if (hcenter || vcenter)
	{	
		textDimension = getDimension(size);
		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;
		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
	}

	static core::rectf drawRect;

	while (*text)
	{
		u32 n = _getGlyphByChar(*text);

		if (*text != L' ' && n > 0)
        {	
			CGlyphTTF & glyph = m_Textures[Size].Glyphs[n-1];
			f32 w;

			if (m_MonospaceMode)
				w = m_Textures[Size].CharDim.Width;
			else
				w = _getWidthFromCharacterIdx(n) + 1;

			if (glyph.tex)
            {
				f32 offx = offset.X;
				f32 offy = offset.Y + (f32)(glyph.size - glyph.top);

				drawRect.set(offx, offy,
					offx + (f32)glyph.texw, offy + (f32)glyph.texh);

				driver.registerGUIImageForRendering(
					glyph.tex, 0.0f, drawRect, glyph.tcoord, color, true, true);
			}
			offset.X += w;
		}
		else
			offset.X += m_Textures[Size].CharDim.Width;;
		++text;
	}
}

//----------------------------------------------------------------------------

s32 CFontTTF::getCharacterFromPos(const wchar_t* text, s32 pixel_x)
{
	if ( Size <= 0 || Size >= FONT_TTF_MAX_SIZE )
		return -1;

	s32 x = 0, idx = 0;

	while (text[idx])
	{
		x += _getWidthFromCharacter(text[idx]);
		if (x >= pixel_x)
			return idx;
		++idx;
	}
	return -1;
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef __MY_BUILD_VID_NULL_LIB__
