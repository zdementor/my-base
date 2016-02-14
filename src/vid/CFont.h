//|-------------------------------------------------------------------------
//| File:        CFont.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CFontHPP
#define CFontHPP
//----------------------------------------------------------------------------

#include <my_inc.h>     
#include <vid/IFont.h>
#include <vid/IVideoDriver.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

class CFont : public IFont
{
public:

    CFont ();

    virtual ~CFont ();

	virtual bool setSize(u32 size);

	virtual s32 getSize();

    virtual void registerForRendering(
		const wchar_t* text, u32 text_len, const core::rect<s32>& position,
		img::SColor color, bool hcenter = false, bool vcenter = false,
		const core::recti * clipRect = NULL);

    virtual core::dimension2di getDimension(const wchar_t* text);
	virtual core::dimension2di getDimension(u32 tex_len);

    virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x);

	virtual const c8* getFileName ()
	{ return m_FileName.c_str(); }

	virtual void setMonospaceMode(bool on) {}
	virtual bool isMonospaceMode() { return true; }

	//! loads a font file
    bool load ( const c8* filename );

    //! loads a font file
    bool load ( io::IReadFile* file );

private:

    //! load & prepare font from ITexture
    bool loadTexture(vid::ITexture* texture);

    void readPositions16bit(vid::ITexture* texture, s32& lowerRightPositions);
    void readPositions32bit(vid::ITexture* texture, s32& lowerRightPositions);

    inline s32 getWidthFromCharacter(wchar_t c);

    core::array< core::rect<s32> > Positions;
    vid::ITexture* Texture;                                                                          
    s32 WrongCharacter;

	core::stringc m_FileName;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif

