//|-------------------------------------------------------------------------
//| File:        CoreVideo.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CoreVideoHPP
#define CoreVideoHPP

//----------------------------------------------------------------------------
namespace my {
namespace vid {
//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ E_DRIVER_TYPE getDriverType();

__MY_CORE_LIB_API__ E_DRIVER_FAMILY getDriverFamily();

__MY_CORE_LIB_API__ u32 getMaximalTextureUnitsAmount();

__MY_CORE_LIB_API__ void onCreateTextureLayer(core::list_node<STextureLayer*> *layer);
__MY_CORE_LIB_API__ void onDestroyTextureLayer(core::list_node<STextureLayer*> *layer);

__MY_CORE_LIB_API__ void onCreateRenderPass(core::list_node<SRenderPass*> *pass);
__MY_CORE_LIB_API__ void onDestroyRenderPass(core::list_node<SRenderPass*> *pass);

__MY_CORE_LIB_API__ void releaseResources();

//----------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CoreVideoHPP