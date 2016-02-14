//|-------------------------------------------------------------------------
//| File:        CBSPEntity.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <my_inc.h>
#include <scn/IBSPEntity.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CBSPEntity : public IBSPEntity
{
protected:	

	bool OK;
	core::stringc EntityString;
	core::array< core::stringc > Keys;
	core::array< core::stringc > KeyValues;
	core::stringc ClassName;	

public:

	scn::IAnimatedMesh *ModelMesh;
	E_BSP_ENTITY_TYPE Type;

	CBSPEntity();
	CBSPEntity(c8 *str);

	void set(CBSPEntity& e);

	virtual const c8* getKeyValue(c8 *key_name);

	virtual s32             getKeyValueAsInt   (c8 *key_name);
	virtual f32             getKeyValueAsFloat (c8 *key_name);
	virtual core::vector3df getKeyValueAsVector(c8 *key_name);
	virtual img::SColor     getKeyValueAsColor (c8 *key_name);

	virtual scn::IAnimatedMesh *getModelMesh();

	virtual bool isOK();
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------