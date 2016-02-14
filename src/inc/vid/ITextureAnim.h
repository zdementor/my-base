//|-------------------------------------------------------------------------
//| File:        ITextureAnim.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ITextureAnimHPP
#define ITextureAnimHPP
//---------------------------------------------------------------------------

#include <vid/ITexture.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

struct SAnimatedTextureParams
{
	SAnimatedTextureParams()
		: TimePerFrame(100.0f), Loop(true)
	{
	}

	SAnimatedTextureParams(s32 time_per_frame, bool loop)
		: TimePerFrame(time_per_frame), Loop(Loop)
	{
	}

	~SAnimatedTextureParams()
	{
	}
	
	s32 TimePerFrame;
	bool Loop;
};

//---------------------------------------------------------------------------

//! Interface for a Animated Texture
class __MY_VID_LIB_API__ ITextureAnim : public ITexture
{
public:

	virtual ~ITextureAnim() {}	

	virtual bool loadFromFile(io::IReadFile* file) = 0;

	virtual SAnimatedTextureParams& getParameters() = 0;

	virtual void setParameters(SAnimatedTextureParams& params) = 0;

	virtual bool addFrame(ITexture* tex) = 0;

	virtual ITexture* getFrame(s32 idx) = 0;

	virtual s32 getFramesCount() = 0;
};

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef ITextureAnimHPP

