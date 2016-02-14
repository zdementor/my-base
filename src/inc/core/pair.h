//|-------------------------------------------------------------------------
//| File:        pair.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef pairHPP
#define pairHPP

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

template <class TValue1, class TValue2>
class pair
{
public:
	pair() : value1(0), value2(0) {}
	pair(TValue1 v1, TValue2 v2) : value1(v1), value2(v2) {}

	TValue1 value1;
	TValue2 value2;
};

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef pairHPP
