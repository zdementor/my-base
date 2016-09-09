//|-------------------------------------------------------------------------
//| File:        COpenGLTexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef COpenGLTextureHPP
#define COpenGLTextureHPP
//---------------------------------------------------------------------------

#include "../CNullHardwareTexture.h"

#include "COpenGLDriver.h"

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

class COpenGLTexture : public CNullHardwareTexture
{
public:

    COpenGLTexture(img::IImage* surface, u32 flags);
	COpenGLTexture(core::dimension2di &size, img::E_COLOR_FORMAT format, u32 flags);
    virtual ~COpenGLTexture();

	virtual void clear();

    virtual img::IImage* lock(u32 level);
    virtual void unlock(u32 level);

	virtual bool freeImageData();

	virtual void regenerateMipMapLevels();

	virtual void* getHardwareTexture() { return (void*)m_TextureName.v; }

protected:

	COpenGLTexture();

	virtual bool createHardwareTexture(bool renderTarget);

	virtual bool createTextureLevel(u32 level, void *data, u32 dataSize, img::E_COLOR_FORMAT format);

	void _regenerateMipMapLevels(const u8 *imageData);

    u8 *m_ImageData[MY_TEXTURE_MAX_MIP_LEVELS];

	union
	{
		GLuint	u;
		void*	v;
	}
	m_TextureName;

	GLint m_InternalFormat;
	GLenum m_PixelFormat;
	GLenum m_PixelType;

	bool m_Created;
};

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif 
