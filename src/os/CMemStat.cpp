//|-------------------------------------------------------------------------
//| File:        CMemStat.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CMemStat.h"

//---------------------------------------------------------------------------
namespace my {
namespace os {
//---------------------------------------------------------------------------

CMemStatus::CMemStatus()
{
#if MY_DEBUG_MODE  
	setClassName("CMemStatus");
#endif
}

//-----------------------------------------------------------------------------

CMemStatus::~CMemStatus()
{
}

//---------------------------------------------------------------------------

__MY_EXPORT__ IMemStatus* createMemStatus()
{
	return new CMemStatus();
}

//-----------------------------------------------------------------------------
} // end namespace os
} // end namespace my
//----------------------------------------------------------------------------- 