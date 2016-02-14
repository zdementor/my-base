//|-------------------------------------------------------------------------
//| File:        MyCEGUITexture.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef _MYCEGUITEXTURE_H_INCLUDED_
#define _MYCEGUITEXTURE_H_INCLUDED_
//------------------------------------------------------------------------------------

#include <CEGUIBase.h>
#include <CEGUIRenderer.h>
#include <CEGUITexture.h>

#include <my_inc.h>
#include <vid/ITexture.h>
#include <dev/IDevice.h>
#include <gui/MyCEGUISystem.h>

using namespace my;

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

class MYENGINE_GUIRENDERER_API MyCEGUITexture : public Texture
{
	friend class MyCEGUIRenderer;
public:
 
    // returns the MyEngine texture
    vid::ITexture* getTexture();

	void setTexture(vid::ITexture *mytex);

	// Implementation of the CEGUI::Texture interface
    const Size& getSize() const;
    const Size& getOriginalDataSize() const;
    const Vector2& getTexelScaling() const;
    virtual void loadFromFile(const String& filename, const String& resourceGroup);
	virtual void loadFromMemory(
		const void* buffPtr, const Size& buffer_size, PixelFormat pixelFormat);
	void saveToMemory(void* buffer);
private:

	MyCEGUITexture(MyCEGUIRenderer &r);
	MyCEGUITexture(MyCEGUIRenderer &r, const Size& size);
	virtual ~MyCEGUITexture();

	//! updates cached scale value used to map pixels to texture co-ords.
    void _updateCachedScaleValues();
        
    // remove the texture from MyEngine textures
    void _freeTexture(bool remove = false);

    vid::ITexture		*m_Texture;
    vid::IVideoDriver	&m_Driver;
	MyCEGUIRenderer		&m_Renderer;

	CEGUI::Size m_Size, m_DataSize;
	CEGUI::Vector2 m_TexelScaling;
};

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------

#endif 
