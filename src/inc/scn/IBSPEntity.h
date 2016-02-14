//|-------------------------------------------------------------------------
//| File:        IBSPEntity.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IBSPEntityHPP
#define IBSPEntityHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

class __MY_SCN_LIB_API__ IBSPEntity : public IUnknown
{
public:

    //! destructor
    virtual ~IBSPEntity() {}

	virtual const c8* getKeyValue(c8 *key_name) = 0;

	virtual s32             getKeyValueAsInt   (c8 *key_name) = 0;
	virtual f32             getKeyValueAsFloat (c8 *key_name) = 0;
	virtual core::vector3df getKeyValueAsVector(c8 *key_name) = 0;
	virtual img::SColor     getKeyValueAsColor (c8 *key_name) = 0;

	virtual scn::IAnimatedMesh *getModelMesh() = 0;

	virtual bool isOK() = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IBSPEntityHPP

