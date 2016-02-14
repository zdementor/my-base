//|-------------------------------------------------------------------------
//| File:        MyCEGUIRenderer.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "MyCEGUIRenderer.h" 
#include "MyCEGUITexture.h"
#include "MyCEGUIWindowTarget.h" 
#include "MyCEGUIGeometryBuffer.h" 

#include <CEGUIRenderingRoot.h>
#include <CEGUIDefaultResourceProvider.h>
#include <CEGUILua.h>
#include <CEGUISystem.h>
#include <sstream>

#include <vid/IVideoDriver.h>
#include <io/ILogger.h>
#include <os/ITimer.h>

//------------------------------------------------------------------------------------
namespace CEGUI { // Start of CEGUI namespace section
//------------------------------------------------------------------------------------

MyCEGUIRenderer::MyCEGUIRenderer()
	: Renderer(),
	m_Driver(vid::IVideoDriver::getSingleton()),
	m_Timer(os::ITimerMain::getSingleton()),
	m_DefaultTarget(new MyCEGUIWindowTarget(*this)),
	m_DefaultRoot(new RenderingRoot(*m_DefaultTarget)),
	m_SupportsNSquareTextures(false),
    m_SupportsNPOTTextures(false)
{
	m_LuaScriptModule = NULL;

	m_Driver.grab();
	m_Timer.grab();

	m_IdentifierString = "MyEngine CEGUI Renderer";

	LOGGER.logInfo("My CEGUI Renderer created.");
}

//------------------------------------------------------------------------------------

MyCEGUIRenderer::~MyCEGUIRenderer()
{
	// destroy a script module.
	if (m_LuaScriptModule)
		LuaScriptModule::destroy(*m_LuaScriptModule);

    destroyAllTextures();
	destroyAllGeometryBuffers();
	destroyAllTextureTargets();

    delete m_DefaultRoot;
    delete m_DefaultTarget;

	m_Timer.drop();
	m_Driver.drop();

	LOGGER.logInfo("My CEGUI Renderer destroyed.");
};

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::initializeLuaScripter(void *lua_scripter_ptr)
{
	if (!lua_scripter_ptr)
	{
		LOGGER.logErr("Can't initialize MyCEGUIRenderer lua scripter, "
			"NULL pointer detected!");
		return;
	}

	// create a script module.
	m_LuaScriptModule = &LuaScriptModule::create((lua_State*)lua_scripter_ptr);

    // tell CEGUI to use this scripting module
    System::getSingleton().setScriptingModule(m_LuaScriptModule);
}

//------------------------------------------------------------------------------------

u32 MyCEGUIRenderer::getSystemTime()
{ return m_Timer.getSystemTime(); }

//------------------------------------------------------------------------------------

my::img::SColor MyCEGUIRenderer::toMyEngineColor(CEGUI::ulong cecolor)
{
    return my::img::SColor(
        ((cecolor >> 24)),
        (((cecolor & 0x00FF0000) >> 16)),
        (((cecolor & 0x0000FF00) >> 8)),
        ((cecolor & 0x000000FF)));
}

//------------------------------------------------------------------------------------

RenderingRoot& MyCEGUIRenderer::getDefaultRenderingRoot()
{ return *m_DefaultRoot; }

//------------------------------------------------------------------------------------

GeometryBuffer& MyCEGUIRenderer::createGeometryBuffer()
{
	MyCEGUIGeometryBuffer *b = new MyCEGUIGeometryBuffer(*this);
    m_Buffers.push_back(b);
	b->m_ListIterator = m_Buffers.get_last();
    return *b;
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyGeometryBuffer(const GeometryBuffer& buffer)
{
	MyCEGUIGeometryBuffer *b = (MyCEGUIGeometryBuffer*)&buffer;
	m_Buffers.erase(b->m_ListIterator);
	delete b;
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyAllGeometryBuffers()
{
	core::list <GeometryBuffer*>::iterator iter = m_Buffers.begin();
    for(;iter != m_Buffers.end();)
    {
        GeometryBuffer* b = (*iter);
        delete b;
        iter = m_Buffers.erase(iter);
    }
}

//------------------------------------------------------------------------------------

TextureTarget* MyCEGUIRenderer::createTextureTarget()
{
	return (TextureTarget*)NULL;
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyTextureTarget(TextureTarget* target)
{
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyAllTextureTargets()
{
}

//------------------------------------------------------------------------------------

Texture& MyCEGUIRenderer::createTexture()
{
    MyCEGUITexture *t = new MyCEGUITexture(*this);
	m_Textures.set_value(t, t);
    return *t;
}

//------------------------------------------------------------------------------------

Texture& MyCEGUIRenderer::createTexture(
    const String& filename, const String& resourceGroup)
{
    MyCEGUITexture *t = new MyCEGUITexture(*this);
	m_Textures.set_value(t, t);
    t->loadFromFile(filename, resourceGroup);
    return *t;
}

//------------------------------------------------------------------------------------

Texture& MyCEGUIRenderer::createTexture(const Size& size)
{
    MyCEGUITexture *t = new MyCEGUITexture(*this, size);
	m_Textures.set_value(t, t);
    return *t;
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyTexture(Texture &texture)
{
	MyCEGUITexture *t = (MyCEGUITexture *)m_Textures.get_value((Texture*)&texture);
	if (!t)
	{
		LOGGER.logErr("Try to delete unexistent CEGUI texture %p", &texture);
	}
	else
		m_Textures.erase_value(t);
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::destroyAllTextures(void)
{
    core::hash_table<Texture*, Texture*>::iterator iter = m_Textures.begin();
    for (;iter != m_Textures.end();)
    {
        Texture *t = (*iter);
		m_Textures.erase_value(t);
    }
	m_Textures.clear();
}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::beginRendering() {}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::endRendering() {}

//------------------------------------------------------------------------------------

void MyCEGUIRenderer::setDisplaySize(const Size& size) {}

//------------------------------------------------------------------------------------

const Size& MyCEGUIRenderer::getDisplaySize() const
{
	core::dimension2di displaysize = m_Driver.getScreenSize();
	static Size size;
	size = Size((f32)displaysize.Width, (f32)displaysize.Height);
	return size;
}

//------------------------------------------------------------------------------------

const Vector2& MyCEGUIRenderer::getDisplayDPI() const
{
	static Vector2 dpi(96, 96);
	return dpi;
}

//------------------------------------------------------------------------------------

uint MyCEGUIRenderer::getMaxTextureSize() const
{ return 2048; }

//------------------------------------------------------------------------------------

const String& MyCEGUIRenderer::getIdentifierString() const
{ return m_IdentifierString; }

//------------------------------------------------------------------------------------
} // End of  CEGUI namespace section
//------------------------------------------------------------------------------------
