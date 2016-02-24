//|-------------------------------------------------------------------------
//| File:        CMemStat.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CMemStatHPP
#define CMemStatHPP
//---------------------------------------------------------------------------

#include <os/IMemStat.h>

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

class CMemStatus : public IMemStatus
{
public:
	CMemStatus();
	virtual ~CMemStatus();
private:
};

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//-----------------------------------------------------------------------------

#endif