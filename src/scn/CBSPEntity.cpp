//|-------------------------------------------------------------------------
//| File:        CBSPEntity.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CBSPEntity.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

CBSPEntity::CBSPEntity() 
	: OK(false), Type(EBSPET_UNKNOWN), ModelMesh(NULL) 
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CBSPEntity");
#endif
}

//----------------------------------------------------------------------------

CBSPEntity::CBSPEntity(c8 *str) 
	: OK(false), Type(EBSPET_UNKNOWN), ModelMesh(NULL), EntityString(str) 
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CBSPEntity");
#endif
}	

//----------------------------------------------------------------------------

void CBSPEntity::set(CBSPEntity& e)
{
	OK = e.OK;
	EntityString = e.EntityString;

	Keys.set_used( e.Keys.size() );

	for (u32 k=0; k<e.Keys.size(); k++)
		Keys[k] = e.Keys[k].c_str();

	KeyValues.set_used( e.KeyValues.size() );

	for (u32 kv=0; kv<e.KeyValues.size(); kv++)
		KeyValues[kv] = e.KeyValues[kv].c_str();

	ClassName = e.ClassName;

	ModelMesh = e.ModelMesh;
	Type = e.Type;
}

//----------------------------------------------------------------------------

const c8* CBSPEntity::getKeyValue(c8 *key_name)
{
    for (u32 k=0; k<Keys.size(); k++)
    {
		 core::stringc key = key_name;
		 core::stringc _key = "_";
		 _key.append(key_name);

        if (Keys[k]==key || Keys[k]==_key)
        {
			if (KeyValues[k].size()>0)
				return KeyValues[k].c_str();
			else 
				break;
        }
    }

	return "";
}

//----------------------------------------------------------------------------

s32 CBSPEntity::getKeyValueAsInt(c8 *key_name)
{
    core::stringc key_value = getKeyValue(key_name);

    if (key_value.size()>0)     
        return atoi(key_value.c_str());     
    
    return 0;
}

//----------------------------------------------------------------------------

f32 CBSPEntity::getKeyValueAsFloat(c8 *key_name)
{
    core::stringc key_value = getKeyValue(key_name);

    if (key_value.size()>0)     
        return atof(key_value.c_str()); 
    
    return 0;
}

//----------------------------------------------------------------------------

core::vector3df CBSPEntity::getKeyValueAsVector(c8 *key_name)
{
	core::stringc key_value = getKeyValue(key_name);

	f32 x=0.0f, y=0.0f, z=0.0f;

	if (key_value.size()>=5)
	{
		c8 *tmp = (c8*)key_value.c_str();

		c8 *strx = strtok(tmp, " ");
		c8 *stry = strtok(NULL, " ");
		c8 *strz = strtok(NULL, "\n");

		if (strx) x = atof(strx);
		if (stry) y = atof(stry);
		if (strz) z = atof(strz);
	}

	return core::vector3df(x, y, z);
}

//----------------------------------------------------------------------------

img::SColor CBSPEntity::getKeyValueAsColor(c8 *key_name)
{
	core::stringc key_value = getKeyValue(key_name);

	f32 r=1.0f, g=1.0f, b=1.0f;

	if (key_value.size()>=5)
	{
		c8 *tmp = (c8*)key_value.c_str();

		c8 *strr = strtok(tmp,  " ");
		c8 *strg = strtok(NULL, " ");
		c8 *strb = strtok(NULL, "\n");

		if (strr) r = atof(strr);
		if (strg) g = atof(strg);
		if (strb) b = atof(strb);
	}

	return img::SColor(255, s32(r*255.0f), s32(g*255.0f), s32(b*255.0f));
}

//----------------------------------------------------------------------------

scn::IAnimatedMesh* CBSPEntity::getModelMesh() 
{ 
	return ModelMesh; 
}

//----------------------------------------------------------------------------

bool CBSPEntity::isOK()
{
	return OK;
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
