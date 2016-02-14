//|-------------------------------------------------------------------------
//| File:        MyCEGUIRenderer.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef MyCEGUIRendererHPP
#define MyCEGUIRendererHPP
//------------------------------------------------------------------------------------

#include <gui/MyCEGUISystem.h>
#include "MyEngineResourceProvider.h"
#include <CEGUIRenderer.h>
#include <CEGUIInputEvent.h>
#include <my_inc.h>

using namespace my;

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

class EventPusher;
class WindowTarget;
class RenderingRoot;
class MyCEGUIWindowTarget;

//! class implementing the interface for Renderer objects with 
//! the MyEngine graphics engine.
class MYENGINE_GUIRENDERER_API MyCEGUIRenderer : public Renderer
{
public:

	MyCEGUIRenderer();
    virtual ~MyCEGUIRenderer();

	void initializeLuaScripter(void *lua_scripter_ptr);
	u32 getSystemTime();

	// Renderer interface implementation
    virtual RenderingRoot& getDefaultRenderingRoot();
    virtual GeometryBuffer& createGeometryBuffer();
    virtual void destroyGeometryBuffer(const GeometryBuffer& buffer);
	virtual void destroyAllGeometryBuffers();
    virtual TextureTarget* createTextureTarget();
    virtual void destroyTextureTarget(TextureTarget* target);
    virtual void destroyAllTextureTargets();
    virtual Texture& createTexture();
    virtual Texture& createTexture(
		const String& filename, const String& resourceGroup);
    virtual Texture& createTexture(const Size& size);
	virtual void destroyTexture(Texture& texture);
    virtual void destroyAllTextures();
    virtual void beginRendering();
    virtual void endRendering();
    virtual void setDisplaySize(const Size& size);
    virtual const Size& getDisplaySize() const;
    virtual const Vector2& getDisplayDPI() const;
    virtual uint getMaxTextureSize() const;
    virtual const String& getIdentifierString() const;

private:

	vid::IVideoDriver	&m_Driver;
	os::ITimer			&m_Timer;

	core::hash_table <Texture*, Texture*> m_Textures;
	core::list <GeometryBuffer*> m_Buffers;

    // convert cegui colour to MyEngine scolor
    inline my::img::SColor toMyEngineColor(CEGUI::ulong cecolor);

	class LuaScriptModule *m_LuaScriptModule;

	String m_IdentifierString;

	MyCEGUIWindowTarget	*m_DefaultTarget;
	RenderingRoot	*m_DefaultRoot;

	bool m_SupportsNSquareTextures, m_SupportsNPOTTextures;
};

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------

#endif // #ifndef MyCEGUIRendererHPP
