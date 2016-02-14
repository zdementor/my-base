//|-------------------------------------------------------------------------
//| File:        CFontTTF.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CFontTTFHPP
#define CFontTTFHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <vid/IVideoDriver.h>
#include <vid/IFont.h>

#include <ft2build.h>
#include <freetype/freetype.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

#define FONT_TTF_MAX_SIZE 64

class CFontTTF;

//----------------------------------------------------------------------------

class CFaceTTF : public IUnknown
{
public:

	CFaceTTF();
	~CFaceTTF();

	FT_Library	library;
	FT_Face		face;
	bool load(const c8* filename);
};

//----------------------------------------------------------------------------

class CGlyphTTF : public IUnknown
{
public:
	CGlyphTTF();
	virtual ~CGlyphTTF();
	void cache ( u32 idx );
	bool cached;
	FT_Face *face;
	u32 size;
	s32 top, left;
	s32 texw, texh;
	core::rectf tcoord;
	vid::ITexture *tex;
	s32 offset;
	CFontTTF *font;

	static u32 getWidth(const CGlyphTTF &glyph);
};

//----------------------------------------------------------------------------

class CFontTTF : public IFont
{
	friend CGlyphTTF;

public:

	CFontTTF ();

	virtual ~CFontTTF ();

	virtual bool setSize ( u32 size );

	virtual s32 getSize ();

	virtual void registerForRendering(
        const wchar_t* text, u32 text_len, const core::rect<s32>& position, 
        img::SColor color, bool hcenter = false, bool vcenter = false,
		const core::recti * clipRect = NULL );

	virtual core::dimension2di getDimension(const wchar_t* text);
	virtual core::dimension2di getDimension(u32 text_len);

	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x);

	virtual const c8* getFileName()
	{ return m_FileName.c_str(); }

	virtual void setMonospaceMode(bool on) { m_MonospaceMode = on; }
	virtual bool isMonospaceMode() { return m_MonospaceMode; }

	//! loads a truetype font file
	bool load ( const c8* filename );

private:

	bool TransParency;

	s32 _getWidthFromCharacterIdx(u32 n);
	s32 _getWidthFromCharacter(wchar_t c);
	u32 _getGlyphByChar(wchar_t c);

    CFaceTTF TT_face;

    s32 Size;

	struct SDataNode 
	{
		SDataNode()
			: Orig(core::position2di(0, 0)), Dim(core::dimension2di(0, 0)),
			Left(NULL), Right(NULL), Data(NULL), Empty(true) {}
		SDataNode(const core::position2di &orig, const core::dimension2di &dim)
			: Orig(orig), Dim(dim), Left(NULL), Right(NULL),
			Data(NULL), Empty(true) {}
		~SDataNode()
		{
			SAFE_DELETE(Left);
			SAFE_DELETE(Right);
		}
		void reset()
		{
			if (Left) Left->reset();
			if (Right) Right->reset();
			Data = NULL;
			Empty = true;
		}
		bool place(void *data, const core::dimension2di& dim,
			core::position2di *out_pos)
		{
			if (Empty && (
					Dim.Width <= dim.Width || Dim.Height <= dim.Height))
				return false;
			if (Empty)
			{
				Empty = false;
				Data = data, DataDim = dim;
				DataDim.Width++;
				DataDim.Height++;
				core::position2di orig_l, orig_r;
				core::dimension2di dim_l, dim_r;
				if (Dim.Width <= Dim.Height)
				{
					orig_l.set	(Orig.X		+ DataDim.Width,	Orig.Y			);
					dim_l.set	(Dim.Width	- DataDim.Width,	DataDim.Height	);
					orig_r.set	(Orig.X,	Orig.Y		+ DataDim.Height		);
					dim_r.set	(Dim.Width, Dim.Height	- DataDim.Height		);
				}
				else
				{
					orig_l.set	(Orig.X,		Orig.Y		+ DataDim.Height	);
					dim_l.set	(DataDim.Width,	Dim.Height	- DataDim.Height	);
					orig_r.set	(Orig.X		+ DataDim.Width, Orig.Y				);
					dim_r.set	(Dim.Width	- DataDim.Width, Dim.Height			);
				}
				if (!Left)
					Left = new SDataNode(orig_l, dim_l);
				else
				{
					Left->Orig = orig_l;
					Left->Dim = dim_l;
				}
				if (!Right)
					Right = new SDataNode(orig_r, dim_r);
				else
				{
					Right->Orig = orig_r;
					Right->Dim = dim_r;
				}
				*out_pos = Orig;
				return true;
			}			
			if (Left->place(data, dim, out_pos))
				return true;
			if (Right->place(data, dim, out_pos))
				return true;
			return false;
		}
		void setDim(const core::dimension2di &dim)
		{
			if (Dim == dim)
				return;
            Dim = dim;
			reset();
		}

	private:
		core::position2di	Orig;
		core::dimension2di	Dim, DataDim;
		SDataNode	*Left, *Right;
		void		*Data;
		bool		Empty;
	};

	struct STTFTexture
	{
		core::array <CGlyphTTF>			Glyphs;
		core::array <vid::ITexture*>	Textures;
		core::hash_table <wchar_t, u32>	GlyphsHash;
		SDataNode						DataNode;
		core::dimension2di				CharDim;

		STTFTexture()
			: CharDim(core::dimension2di(0, 0))
		{
			DataNode.setDim(getTexDim());
		}

		~STTFTexture()
		{ clear(); }

		void clear()
		{
			GlyphsHash.clear();
			Glyphs.clear();
			u32 texcnt = Textures.size();
			for (u32 t = 0; t < texcnt; t++)
				SAFE_DROP(Textures[t]);
			Textures.clear();
		}

		core::dimension2di getTexDim()
		{ return core::dimension2di(128, 128); }
	}
	m_Textures[FONT_TTF_MAX_SIZE];	

	core::stringc m_FileName;

	bool m_MonospaceMode;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif

