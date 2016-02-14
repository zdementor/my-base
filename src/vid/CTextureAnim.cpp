//|-------------------------------------------------------------------------
//| File:        CTextureAnim.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include "CTextureAnim.h"

#include <dev/IDevice.h>
#include <io/ILogger.h>
#include <vid/IVideoDriver.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! constructor
CTextureAnim::CTextureAnim() 
	: FirstTime(true), m_Format(ETF_TEXTURE_2D)
{	
#if MY_DEBUG_MODE 
	setClassName("CTextureAnim");
#endif
}

//---------------------------------------------------------------------------

CTextureAnim::CTextureAnim(ITextureAnim* other) 
	: FirstTime(true), m_Format(ETF_TEXTURE_2D)
{	
#if MY_DEBUG_MODE 
	setClassName("CTextureAnim");
#endif

	if (other)
	{
		setParameters(other->getParameters());
		s32 fr_cnt = other->getFramesCount();
		for (s32 f=0; f<fr_cnt; f++)
			addFrame(other->getFrame(f));
	}
}

//---------------------------------------------------------------------------

CTextureAnim::~CTextureAnim()
{
	clear();
}

//---------------------------------------------------------------------------

void CTextureAnim::clear()
{
	for (u32 t=0; t<Frames.size(); t++)
		SAFE_DROP(Frames[t]);
	Frames.clear();
}

//---------------------------------------------------------------------------

ITexture* CTextureAnim::_getCurrentTexture()
{
	if (Frames.size()==0)
		return NULL;

	s32 now = DEVICE.getDeviceSystemTime();

	if (FirstTime)
	{
		StartTime = now;
		FirstTime = false;
	}

	u32 time_per_frame = AnimatedTextureParameters.TimePerFrame;

	CHECK_NO_ZERO(time_per_frame);
	
	u32 texidx = ((now-StartTime)/time_per_frame)%Frames.size();

	if (texidx>=0 && texidx<Frames.size())
		return Frames[texidx];

	return NULL;
}

//---------------------------------------------------------------------------

void* CTextureAnim::getHardwareTexture()
{
	ITexture* cur_tex = _getCurrentTexture();
	if (cur_tex)
		return cur_tex->getHardwareTexture();
	return NULL;
}

//---------------------------------------------------------------------------

bool CTextureAnim::loadFromFile(io::IReadFile* file)
{
	if (!file)
		return false;

	io::IXMLReader* xml = FILE_SYSTEM.createXMLReader(file);

	if (!xml)
		return false;

	c8 ch[255];
	sprintf(ch, "Loading animated texture from %s", file->getFileName());
	LOGGER.log(ch, io::ELL_INFORMATION);		

	bool ani_tex_tag_founded = false; 

	core::array<core::stringc> filenames;

	while (xml->read())
	{
		if (xml->getNodeType()== io::EXN_ELEMENT)
		{
			core::stringw xml_node_name = xml->getName();

			if (core::stringw("AnimatedTexture") == xml_node_name)
			{
				ani_tex_tag_founded = true;	
				
				AnimatedTextureParameters.TimePerFrame = xml->getAttributeValueAsInt(L"time_per_frame");
				CHECK_RANGE(AnimatedTextureParameters.TimePerFrame, 10, 10000);

				AnimatedTextureParameters.Loop = xml->getAttributeValueAsInt(L"loop")==0?false:true;
			}
			else
			if (ani_tex_tag_founded && core::stringw("Frame") == xml_node_name)
			{
				core::stringc filename = xml->getAttributeValue(L"filename");
				
				if (filename.size()>0)
				{
					filenames.push_back(filename);
				}
			}
		}
	}

	xml->drop();

	s32 n_tex=0;

	if (filenames.size()>0)
	{
		// loading frames

		LOGGER.increaseFormatLevel();	

		core::stringc filepath = core::extractFilePath(core::stringc(file->getFileName()));

		for (u32 t=0; t<filenames.size(); t++)
		{
			core::stringc full_filename = filepath;
			
			full_filename.append(filenames[t]);

			vid::ITexture* tex = 
				VIDEO_DRIVER.getTexture(full_filename.c_str());

			if (tex)
			{
				addFrame(tex);

				n_tex++;
			}
		}

		LOGGER.decreaseFormatLevel();
	}	
	
	if (n_tex<=0)
	{
		LOGGER.log("Can't load animated texture, there is no any frames in it.", io::ELL_ERROR);		

		return false;
	}
	
	LOGGER.log("Loaded animated texture.", io::ELL_INFORMATION);

	return true;
}

//---------------------------------------------------------------------------

SAnimatedTextureParams& CTextureAnim::getParameters()
{
	return AnimatedTextureParameters;
}

//---------------------------------------------------------------------------

void CTextureAnim::setParameters(SAnimatedTextureParams& params)
{
	AnimatedTextureParameters = params;	
}

//---------------------------------------------------------------------------

bool CTextureAnim::addFrame(ITexture* tex)
{
	if (!tex)
		return false;

	if (Frames.size()!=0 && (
			OriginalSize!= tex->getOriginalSize() ||
			Size		!= tex->getSize() ||
			ColorFormat	!= tex->getColorFormat() ||
			Pitch		!= tex->getPitch() ||
			m_Format	!= tex->getFormat())
		)
		return false;

	SAFE_GRAB(tex);

	Frames.push_back(tex);

	OriginalSize= tex->getOriginalSize();
	Size		= tex->getSize();
	ColorFormat	= tex->getColorFormat();
	Pitch		= tex->getPitch();
	m_Format	= tex->getFormat();

	return true;
}

//---------------------------------------------------------------------------

ITexture* CTextureAnim::getFrame(s32 idx)
{
	if (idx>=0 && idx<(s32)Frames.size())
	{
		return Frames[idx];
	}

	return 0;
}

//---------------------------------------------------------------------------

s32 CTextureAnim::getFramesCount()
{
	return Frames.size();
}

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------