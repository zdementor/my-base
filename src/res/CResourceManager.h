//|-------------------------------------------------------------------------
//| File:        CResourceManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CResourceManagerHPP
#define CResourceManagerHPP
//----------------------------------------------------------------------------

#include <res/IResourceManager.h>
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace res {
//----------------------------------------------------------------------------

class CResourceManager : public IResourceManager
{
public:

    CResourceManager();
    virtual ~CResourceManager();

	virtual void		setMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir);
	virtual void		setCommonMediaDirRel(E_MEDIA_TYPE mt, const c8 *dir);

	virtual const c8*	getMediaDirRel(E_MEDIA_TYPE mt);
	virtual const c8*	getCommonMediaDirRel(E_MEDIA_TYPE mt);

	virtual const c8*	getMediaDirFull(E_MEDIA_TYPE mt);
	virtual const c8*	getCommonMediaDirFull(E_MEDIA_TYPE mt);

	virtual const c8*	getTextureFileNameRel(vid::ITexture *texture);
	virtual const c8*	getCommonTextureFileNameRel(vid::ITexture *texture);

	virtual const c8*	getMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name);	
	virtual const c8*	getCommonMediaFileNameRel(E_MEDIA_TYPE mt, const c8 *full_file_name);

private:

	core::stringc m_MediaDirRel[E_MEDIA_TYPE_COUNT];
	core::stringc m_MediaDirFull[E_MEDIA_TYPE_COUNT];

	core::stringc m_CommonMediaDirRel[E_MEDIA_TYPE_COUNT];
	core::stringc m_CommonMediaDirFull[E_MEDIA_TYPE_COUNT];
};

//----------------------------------------------------------------------------
} // end namespace res
} // end namespace my
//----------------------------------------------------------------------------

#endif

