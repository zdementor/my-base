//|-------------------------------------------------------------------------
//| File:        CQ3EntityParser.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CQ3EntityParser.h"
#include "CQ3Entity.h"
#include <io/ILogger.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

core::array< CQ3Entity > CQ3EntityParser::EntitiesEntries;

//----------------------------------------------------------------------------

void CQ3EntityParser::parseEntities(c8 *EntitiesString)
{
	LOGGER.log("Parsing entities . . .");

	c8 *tmp = 0;

	bool first = true;

	if (EntitiesString[0]=='{')
	{
		tmp=EntitiesString+1;
	}
	else
		return;

    while(tmp)
    {
		if (first)
		{
			tmp = strtok(tmp, "}"); 
			first = false;
		}
		else
		{
			tmp = strtok(NULL, "}");            
		}

        if (tmp)
        {
            EntitiesEntries.push_back(tmp);         

            tmp = strtok(NULL, "{");
        }       
    }

    u32 e=0;

    for (e=0; e<EntitiesEntries.size();)
    {
        EntitiesEntries[e].parse();

        if (!EntitiesEntries[e].isOK())
            EntitiesEntries.erase(e);
        else
            e++;
    }	

	bool worldspawn_finded=false;

	for (e=0; e<EntitiesEntries.size();)
    {
        if (worldspawn_finded && EntitiesEntries[e].Type==EBSPET_WORLDSPAWN)
		// worldspawn entity must be just one
		{
            EntitiesEntries.erase(e);
		}
        else
		{
			if (EntitiesEntries[e].Type==EBSPET_WORLDSPAWN)
			{
				worldspawn_finded = true;
			}
            e++;
		}
    }	
}

//----------------------------------------------------------------------------

void CQ3EntityParser::clearEntities()
{
    EntitiesEntries.clear();
}

//----------------------------------------------------------------------------

core::array< CQ3Entity >&  CQ3EntityParser::getEntities()
{ 
	return EntitiesEntries; 
}

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
