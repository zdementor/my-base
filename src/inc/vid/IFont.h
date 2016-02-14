//|-------------------------------------------------------------------------
//| File:        IFont.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IFontHPP
#define IFontHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! Font interface.
class __MY_VID_LIB_API__ IFont : public IUnknown
{
public:

    //! Destructor
    virtual ~IFont() {}

    //! set size (only for ttf-font)
	virtual bool setSize(u32 size) = 0;

    //! get size (only for ttf-font, for graphic always return 1)
	virtual s32 getSize() = 0;

    //! Draws an text and clips it to the specified rectangle if wanted
    //! \param text: Text to draw
    //! \param position: Ractangle spceifying position where to draw the text.
    //! \param color: Color of the text
    //! \param hcenter: Specifiies if the text should be centered horizontally into the rectangle.
    //! \param vcenter: Specifiies if the text should be centered vertically into the rectangle.
    //! If the pointer is null, no clipping will be done.
    virtual void registerForRendering(
		const wchar_t *text, u32 text_len, const core::recti& position, 
        img::SColor color, bool hcenter = false, bool vcenter = false,
		const core::recti *clipRect = NULL) = 0;
    virtual void registerForRendering(
		const c8 *text, u32 text_len, const core::recti& position, 
        img::SColor color, bool hcenter = false, bool vcenter = false,
		const core::recti *clipRect = NULL)
	{
		m_ConvBuffer = text;
		registerForRendering(m_ConvBuffer.c_str(), m_ConvBuffer.size(),
			position, color, hcenter, vcenter, clipRect);
	}

    //! Calculates the dimension of a text.
    //! \return Returns width and height of the area covered by the text if it would be
    //! drawn.
    virtual core::dimension2di getDimension(const wchar_t *text) = 0;
	virtual core::dimension2di getDimension(const c8 *text)
	{
		m_ConvBuffer = text;
		return getDimension(m_ConvBuffer.c_str());
	}
	virtual core::dimension2di getDimension(u32 text_len) = 0;

    //! Calculates the index of the character in the text which is on a specific position.
    //! \param text: Text string.
    //! \param pixel_x: X pixel position of which the index of the character will be returned.
    //! \return Returns zero based index of the character in the text, and -1 if no no character
    //! is on this position. (=the text is too short).
    virtual s32 getCharacterFromPos(const wchar_t *text, s32 pixel_x) = 0;
	virtual s32 getCharacterFromPos(const c8 *text, s32 pixel_x)
	{
		m_ConvBuffer = text;
		return getCharacterFromPos(m_ConvBuffer.c_str(), pixel_x);
	}

	//! returns font filename
	virtual const c8* getFileName() = 0;

	//! turn on/off font monospace mode
	virtual void setMonospaceMode(bool on) = 0;

	//! return true if font monospace mode turned on
	virtual bool isMonospaceMode() = 0;

private:

	core::stringw m_ConvBuffer;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif

