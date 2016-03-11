//|-------------------------------------------------------------------------
//| File:        CoreVideo.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

#include "CoreSingletonsTable.h"

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ E_DRIVER_TYPE getDriverType()
{
	return core::g_SingletonsTable.Ptrs.VideoDriver->
		getDriverType();	
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ E_DRIVER_FAMILY getDriverFamily()
{
	return core::g_SingletonsTable.Ptrs.VideoDriver->
		getDriverFamily();	
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ u32 getMaximalTextureUnitsAmount()
{
	return core::g_SingletonsTable.Ptrs.VideoDriver->
		getMaximalTextureUnitsAmount();	
}

//----------------------------------------------------------------------------

static core::list_fast<STextureLayer*> _TextureLayers;

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void onCreateTextureLayer(core::list_node<STextureLayer*> *layer)
{
	_TextureLayers.push_back(layer);
}

//---------------------------------------------------------------------------

__MY_CORE_LIB_API__ void onDestroyTextureLayer(core::list_node<STextureLayer*> *layer)
{
	_TextureLayers.erase(layer);
}

//----------------------------------------------------------------------------

static core::list_fast<SRenderPass*> _RenderPasses;

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ void onCreateRenderPass(core::list_node<SRenderPass*> *pass)
{
	_RenderPasses.push_back(pass);
}

//---------------------------------------------------------------------------

__MY_CORE_LIB_API__ void onDestroyRenderPass(core::list_node<SRenderPass*> *pass)
{
	_RenderPasses.erase(pass);
}

//---------------------------------------------------------------------------

__MY_CORE_LIB_API__ void releaseResources()
{
	{
		core::list_fast<STextureLayer*>::iterator it = _TextureLayers.begin();
		for (; it != _TextureLayers.end(); ++it)
		{
			STextureLayer *layer = *it;
			layer->setTexture(NULL);
		}
	}

	{
		core::list_fast<SRenderPass*>::iterator it = _RenderPasses.begin();
		for (; it != _RenderPasses.end(); ++it)
		{
			SRenderPass *pass = *it;
			pass->setGPUProgram(NULL);
		}
	}
}

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------



