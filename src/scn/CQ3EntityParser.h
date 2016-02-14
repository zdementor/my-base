//|-------------------------------------------------------------------------
//| File:        CQ3EntityParser.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CQ3Entity;

//----------------------------------------------------------------------------

class CQ3EntityParser
{
private:

	static core::array< CQ3Entity > EntitiesEntries;

public:

	static void parseEntities(c8 *EntitiesString);

	static void clearEntities();

	static core::array< CQ3Entity >& getEntities();
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------