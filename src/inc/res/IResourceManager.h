//|-------------------------------------------------------------------------
//| File:        IResourceManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IResourceManagerHPP
#define IResourceManagerHPP
//----------------------------------------------------------------------------

#include <IUnknown.h>
#include <ISingleton.h>

//----------------------------------------------------------------------------
namespace my {
namespace res {
//----------------------------------------------------------------------------

class __MY_RES_LIB_API__ IResourceManager :
	public IUnknown, public ISingleton < IResourceManager >
{
public:
	//! sets relative media directory
	virtual void setMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir) = 0;
	virtual void setCommonMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir) = 0;

	//! return relative media directory
	virtual const c8* getMediaDirRel(E_MEDIA_TYPE mt) = 0;
	virtual const c8* getCommonMediaDirRel(E_MEDIA_TYPE mt) = 0;

	//! return full media directory 
	virtual const c8* getMediaDirFull(E_MEDIA_TYPE mt) = 0;
	virtual const c8* getCommonMediaDirFull(E_MEDIA_TYPE mt) = 0;

	//! return relative file name to the loaded hardware texture
	virtual const c8* getTextureFileNameRel(vid::ITexture *texture) = 0;
	virtual const c8* getCommonTextureFileNameRel(vid::ITexture *texture) = 0;

	//! return relative file name to the media resource
	virtual const c8* getMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name) = 0;	
	virtual const c8* getCommonMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name) = 0;

    //! Return singleton object
    static IResourceManager& getSingleton();

    //! Return pointer to singleton object
    static IResourceManager* getSingletonPtr();
};

//----------------------------------------------------------------------------

__MY_RES_LIB_API__ IResourceManager* createResourceManager();

//----------------------------------------------------------------------------
} // end namespace res
} // end namespace my
//----------------------------------------------------------------------------

#endif

