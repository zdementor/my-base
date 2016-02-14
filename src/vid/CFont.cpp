//|-------------------------------------------------------------------------
//| File:        CFont.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CFont.h"

#include <io/ILogger.h>
#include <img/IImageLibrary.h>
#include <res/IResourceManager.h>

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

//! constructor
CFont::CFont() 
	: Positions(382), Texture(0), WrongCharacter(0), m_FileName("")
{
#if MY_DEBUG_MODE  
	setClassName("CFont");
#endif
}

//----------------------------------------------------------------------------

//! destructor
CFont::~CFont()
{
    SAFE_DROP(Texture);
}

//----------------------------------------------------------------------------

//! loads a font file
bool CFont::load ( io::IReadFile* file )
{
	if (!file)
        return false;

	vid::IVideoDriver & driver = IVideoDriver::getSingleton();

	core::stringc filename = file->getFileName();

	core::stringc fname = core::extractFileName(filename);
	core::stringc fpath = core::extractFilePath(filename);
	if (fpath!="")
	{	
		core::stringc fonts_path = 
			RESOURCE_MANAGER.getMediaDirFull(res::EMT_FONTS);  

		core::stringc rel_path = core::getRelativePathBetweenDirectories(fonts_path, fpath);

		m_FileName=rel_path;
		m_FileName.append(fname);
	}
	else
	{
		m_FileName=filename;
	}

	img::IImage* img = IMAGE_LIBRARY.getImage(file);
    
	if (img)
	{
		bool flgmip = driver.getTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS);

		driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS,false);

		vid::ITexture *tex = driver.addTexture(file->getFileName(), img);
	
		driver.setTextureCreationFlag(vid::ETCF_AUTOGEN_MIP_MAPS,flgmip);

		return loadTexture(tex);
	}

	return false;
}

//----------------------------------------------------------------------------

//! loads a font file
bool CFont::load(const c8* _filename)
{
	vid::IVideoDriver & driver = IVideoDriver::getSingleton();

	core::stringc filename=_filename;

	core::stringc fname = core::extractFileName(filename);
	core::stringc fpath = core::extractFilePath(filename);
	core::stringc fonts_path = 
		RESOURCE_MANAGER.getMediaDirFull(res::EMT_FONTS);  

	core::stringc rel_path = core::getRelativePathBetweenDirectories(fonts_path, fpath);

	m_FileName=rel_path;
	m_FileName.append(fname);
    
    return loadTexture(driver.getTexture(_filename));
}

//----------------------------------------------------------------------------

//! set size (only for ttf-font)
bool CFont::setSize(u32 size)
{
	LOGGER.log("Can't change size for graphic font.", io::ELL_WARNING);   
    return false;
}

//----------------------------------------------------------------------------

//! get size (only for ttf-font, for graphic always return 1)
s32 CFont::getSize()
{   return 1; }

//----------------------------------------------------------------------------

//! load & prepare font from ITexture
bool CFont::loadTexture(vid::ITexture* texture)
{
    if (!texture)
        return false;

    Texture = texture;
    Texture->grab();

    s32 lowerRightPostions = 0;

    switch(texture->getColorFormat())
    {
    case img::ECF_A1R5G5B5:
        readPositions16bit(texture, lowerRightPostions);
        break;                                                  
    case img::ECF_A8R8G8B8:
        readPositions32bit(texture, lowerRightPostions);
        break;
    default:
        LOGGER.log("Unsupported font texture color format.", io::ELL_ERROR);
        return false;
    }

    if (Positions.size() > 127)
        WrongCharacter = 127;

    return (!Positions.empty() && lowerRightPostions);
}

//----------------------------------------------------------------------------

void CFont::readPositions32bit(
	vid::ITexture* texture, s32& lowerRightPostions
	)
{
	s32 pitch = texture->getPitch();

    core::dimension2d<s32> size = texture->getOriginalSize();

    s32* p = (s32*)texture->lock();
    if (!p)
    {
		LOGGER.log("Could not lock texture while preparing texture for a font.", io::ELL_ERROR);
        return;
    }

    s32 colorTopLeft = *p;
    s32 colorLowerRight = *(p+1);
    s32 colorBackGround = *(p+2);
    s32 colorBackGroundWithAlphaTrue = (0xFF<<24) | (~(0xFF<<24) & colorBackGround);
    s32 colorBackGroundWithAlphaFalse = (0x00<<24) | (~(0xFF<<24) & colorBackGround);
    s32 colorFont = (0xFF<<24) | (~(0xFF<<24) & 0x00FFFFFF);


    *(p+1) = colorBackGround;
    *(p+2) = colorBackGround;

    // start parsing

    core::position2d<s32> pos(0,0);

    c8* row = (c8*)((void*)p);

    for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
    {
        p = (s32*)((void*)row);

        for (pos.X=0; pos.X<size.Width; ++pos.X)
        {
			if (*p == colorTopLeft)
            {
				*p = colorBackGroundWithAlphaFalse;
                Positions.push_back(core::rect<s32>(pos.X-1,pos.Y, pos.X,pos.Y));
            }
            else if (*p == colorLowerRight)
            {
				if (Positions.size()<=(u32)lowerRightPostions)
                {
					texture->unlock();
                    lowerRightPostions = 0;
                    return;
                }
                *p= colorBackGroundWithAlphaFalse;
                Positions[lowerRightPostions].LowerRightCorner = pos;
                ++lowerRightPostions;
            }
            else
            if (*p == colorBackGround)
                *p = colorBackGroundWithAlphaFalse;
            else
                *p = colorFont;

			++p;
        }

        row += pitch;
    }

    // Positions parsed.

    texture->unlock();

    // output warnings
    if (!lowerRightPostions || !Positions.size())
        LOGGER.log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", io::ELL_ERROR);
    else
    if (lowerRightPostions != (s32)Positions.size())
        LOGGER.log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", io::ELL_ERROR);
}

//----------------------------------------------------------------------------

void CFont::readPositions16bit(vid::ITexture* texture, s32& lowerRightPostions)
{
    s32 pitch = texture->getPitch();
    core::dimension2d<s32> size = texture->getOriginalSize();

	img::IImage* tex_img = texture->lock();
	s16* p = NULL;

	if (tex_img)
	{
		p = (s16*)tex_img->getData();
	}

    if (!p)
    {
        LOGGER.log("Could not lock texture while preparing texture for a font.", io::ELL_ERROR);
        return;
    }

    s16 colorTopLeft = *p;
    s16 colorLowerRight = *(p+1);
    s16 colorBackGround = *(p+2);
    s16 colorBackGroundWithAlphaTrue = (0x1<<15) | (~(0x1<<15) & colorBackGround);
    s16 colorBackGroundWithAlphaFalse = (0x0<<15) | (~(0x1<<15) & colorBackGround);
    s16 colorFont = (0x1<<15) | (~(0x1<<15) & img::RGB16(255,255,255));

    *(p+1) = colorBackGround;
    *(p+2) = colorBackGround;

    // start parsing

    core::position2d<s32> pos(0,0);

    c8* row = (c8*)((void*)p);

    for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
    {
        p = (s16*)((void*)row);

        for (pos.X=0; pos.X<size.Width; ++pos.X)
        {
            if (*p == colorTopLeft)
            {
				*p = colorBackGroundWithAlphaFalse;
                Positions.push_back(core::rect<s32>(pos, pos));
            }
            else
            if (*p == colorLowerRight)
            {
                if (Positions.size()<=(u32)lowerRightPostions)
                {
                    texture->unlock();
                    lowerRightPostions = 0;
                    return;
                }

                *p = colorBackGroundWithAlphaFalse;
                Positions[lowerRightPostions].LowerRightCorner = pos;
                ++lowerRightPostions;
            }
            else 
            if (*p == colorBackGround)
                *p = colorBackGroundWithAlphaFalse;
            else
                *p = colorFont;


            ++p;
        }

        row += pitch;
    }

    // Positions parsed.

    texture->unlock();

    // output warnings
    if (!lowerRightPostions || !Positions.size())
        LOGGER.log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", io::ELL_ERROR);
    else
    if (lowerRightPostions != (s32)Positions.size())
        LOGGER.log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", io::ELL_ERROR);
}

//----------------------------------------------------------------------------

core::dimension2di CFont::getDimension(const wchar_t* text)
{
    core::dimension2d<s32> dim(0, Positions[0].getHeight());

    u32 n;

    for(const wchar_t* p = text; *p; ++p)
    {
        n = (*p) - 32;
        if ( n > Positions.size())
            n = WrongCharacter;

        dim.Width += Positions[n].getWidth();
    }

    return dim;
}

//----------------------------------------------------------------------------

core::dimension2di CFont::getDimension(u32 text_len)
{
    static core::dimension2di dim(0, 0);

	dim.Height= Positions[0].getHeight();
	dim.Width = getWidthFromCharacter(L'A') * text_len;

    return dim;
}

//----------------------------------------------------------------------------

inline s32 CFont::getWidthFromCharacter(wchar_t c)
{
    u32 n = c - 32;
    if (n > Positions.size())
        n = WrongCharacter;

    return Positions[n].getWidth();
}

//----------------------------------------------------------------------------

void CFont::registerForRendering (
	const wchar_t* text, u32 size, const core::rect<s32>& position,
	img::SColor color, bool hcenter, bool vcenter,
	const core::recti * clipRect )
{
	vid::IVideoDriver & driver = IVideoDriver::getSingleton();

    core::dimension2d<s32> textDimension;
    core::position2d<s32> offset = position.UpperLeftCorner;

    if (hcenter || vcenter)
    {
        textDimension = getDimension(size);

        if (hcenter)
            offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;

        if (vcenter)
            offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
    }

    u32 n;
	core::dimension2di texDim = Texture->getOriginalSize();
	core::rectf texRect;

	core::rectf drawRect;
	drawRect.UpperLeftCorner.Y	= offset.Y;
	drawRect.LowerRightCorner.Y	= offset.Y + textDimension.Height;

    while(*text)
    {
        n = (*text) - 32;
        if ( n > Positions.size())
            n = WrongCharacter;

		texRect.set (
			( Positions[n].UpperLeftCorner.X + 0.5f ) / (f32)texDim.Width,
			( Positions[n].UpperLeftCorner.Y + 0.5f ) / (f32)texDim.Height,
			( Positions[n].LowerRightCorner.X + 0.5f ) / (f32)texDim.Width,
			( Positions[n].LowerRightCorner.Y + 0.5f ) / (f32)texDim.Height );

		u32 pw = Positions[n].getWidth();
		
		drawRect.UpperLeftCorner.X	= offset.X;
		drawRect.LowerRightCorner.X	= offset.X + pw;
		
		driver.registerGUIImageForRendering(
			Texture, 0.0f, drawRect, texRect, color, false, true);

		//driver.register2DImageForRendering (
		//	Texture, drawRect, texRect, color, false, true );
     
        offset.X += pw;

        ++text;
    }
}

//----------------------------------------------------------------------------

//! Calculates the index of the character in the text which is on a specific position.
s32 CFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x)
{
    s32 x = 0;
    s32 idx = 0;

    while (text[idx])
    {
        x += getWidthFromCharacter(text[idx]);

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
