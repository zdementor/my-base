//|-------------------------------------------------------------------------
//| File:        CQ3Entity.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <my_inc.h>
#include "CBSPEntity.h"

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

class CQ3EntityParser;

//----------------------------------------------------------------------------

class CQ3Entity : public CBSPEntity
{
	friend class CQ3EntityParser;

private:	

	void parse();	

public:

	CQ3Entity();
	CQ3Entity(c8 *str);

	virtual const c8* getKeyValue(c8 *key_name);
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------