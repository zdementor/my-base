//|-------------------------------------------------------------------------
//| File:        CQ3Entity.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CQ3Entity.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

CQ3Entity::CQ3Entity() 
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CQ3Entity");
#endif
}

//----------------------------------------------------------------------------

CQ3Entity::CQ3Entity(c8 *str) 
	: CBSPEntity(str) 
{
#if MY_DEBUG_MODE 
	IUnknown::setClassName("CQ3Entity");
#endif
}	

//----------------------------------------------------------------------------

void CQ3Entity::parse()
{
    c8 *tmp = (c8*)EntityString.c_str();
    c8 *key = strtok(tmp, "\"");

    while(key)
    {
        key = strtok(NULL, "\"");           

        if (key)
        {
            c8 *key_value = strtok(NULL, "\"");

            if (key_value)
            {
                key_value = strtok(NULL, "\"");

                if (key_value)
                {
                    core::stringc Key = key;
                    core::stringc KeyValue = key_value; 

                    if (Key == "classname")
                    {
                        OK = true;

                        ClassName = KeyValue.c_str();
                    }
                    else
                    {
                        if (Key == "model")
                        {
                            if (KeyValue.size()>0 && KeyValue[0]=='*')
                            {
                                KeyValue[0] = '0';
                            }
                        }
						else
						if (Key == "_ambient;ambient")
						{
							Key = "ambient";
						}

                        Keys.push_back(Key.c_str());
                        KeyValues.push_back(KeyValue.c_str());
                    }

                    key = strtok(NULL, "\"");   
                }
            }
        }
    }
	
	if (ClassName == core::stringc("worldspawn"))
	{
		Type = EBSPET_WORLDSPAWN;
	}
	else
	if (ClassName == core::stringc("func_static"))
	{
		Type = EBSPET_UNKNOWN;
	}
	else
	if (ClassName == core::stringc("info_player_start") ||
		ClassName == core::stringc("info_player_deathmatch")
		)
	{
		Type = EBSPET_INFO_PLAYER_START;
	}
	else
	if (ClassName == core::stringc("info_null"))
	{
		Type = EBSPET_INFO_NULL;
	}	
	else
	if (ClassName == core::stringc("func_door"))
	{
		Type = EBSPET_FUNC_DOOR;
	}
	else
	if (ClassName == core::stringc("func_button"))
	{
		Type = EBSPET_FUNC_BUTTON;
	}
	else
	if (ClassName == core::stringc("func_transparent"))
	{
		Type = EBSPET_FUNC_TRANSPARENT;
	}
	else
	if (ClassName == core::stringc("light"))
	{
		Type = EBSPET_LIGHT;
	}	
}

//----------------------------------------------------------------------------

const c8* CQ3Entity::getKeyValue(c8 *key_name)
{
	static core::stringc key_value = "";
		
	key_value = CBSPEntity::getKeyValue(key_name);
    
	if (key_value!="")
		return key_value.c_str();

	// key values by default

	if (Type == EBSPET_WORLDSPAWN)
	{
		if (core::stringc(key_name) == core::stringc("gridsize") )
			return "64 64 128";
		else
		if (core::stringc(key_name) == core::stringc("ambient") )
			return "10"; 
	}
	else
	if (Type == EBSPET_INFO_PLAYER_START)
	{
		if (core::stringc(key_name) == core::stringc("origin") )
			return "0 0 0"; 
	}
	else
	if (Type == EBSPET_INFO_NULL)
	{
		if (core::stringc(key_name) == core::stringc("origin"))
			return "0 0 0"; 
	}	
	else
	if (Type == EBSPET_FUNC_DOOR)
	{
		if (core::stringc(key_name) == core::stringc("angle") )
			return "0"; 
		else
		if (core::stringc(key_name) == core::stringc("speed") )
			return "100"; 
		else
		if (core::stringc(key_name) == core::stringc("damage") )
			return "4"; 
		else
		if (core::stringc(key_name) == core::stringc("wait") )
			return "2"; 
		else
		if (core::stringc(key_name) == core::stringc("crusher") )
			return "0"; 
		else
		if (core::stringc(key_name) == core::stringc("spawnflags") )
			return "0"; 		
		else
		if (core::stringc(key_name) == core::stringc("lip") )
			return "16"; 
	}
	else
	if (Type == EBSPET_FUNC_TRANSPARENT)
	{
		if (core::stringc(key_name) == core::stringc("transp_dist") )
			return "150"; 
		else
		if (core::stringc(key_name) == core::stringc("spawnflags") )
			return "0"; 
	}
	else 
	if (Type == EBSPET_FUNC_BUTTON)
	{
		if (core::stringc(key_name) == core::stringc("angle") )
			return "0"; 
		else
		if (core::stringc(key_name) == core::stringc("speed") )
			return "20"; 
		else
		if (core::stringc(key_name) == core::stringc("wait") )
			return "4"; 
	}
	else 
	if (Type == EBSPET_LIGHT)
	{
		if (core::stringc(key_name) == core::stringc("light") )
			return "300"; 
		else
		if (core::stringc(key_name) == core::stringc("radius") )
			return "64"; 
		else
		if (core::stringc(key_name) == core::stringc("origin") )
			return "0 0 0"; 
		else
		if (core::stringc(key_name) == core::stringc("color") || 
			core::stringc(key_name) == core::stringc("_color")
			)
			return "1 1 1"; 
	}

    return "";
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
