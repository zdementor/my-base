//|-------------------------------------------------------------------------
//| File:        filter.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef _filter_h_
#define _filter_h_

#include "mathfunc.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

template <class T, u32 SamplesNumber = 32, u32 HistProcent = 5, s32 InitValue = 0>
class filter
{
public:

	filter() : m_CurrentIndex(0), m_Count(0), m_Dirty(false)
	{
		for (u32 i = 0; i < SamplesNumber; i++)
			m_Values[i] = InitValue;
		m_FilteredValue = InitValue;
	}

	template <class TT>
	void insertValue(TT value)
	{
		m_Values[m_CurrentIndex] = value;
		m_CurrentIndex = (++m_Count) % SamplesNumber;
		m_Dirty = true;
	}

	T getFilteredValue()
	{
		if (m_Dirty)
			_recalc();
		return m_FilteredValue;
	}

private:

	void _recalc()
	{
		u32 i;
		T avgValue = InitValue;
		for (i = 0; i < SamplesNumber && i < m_Count; i++)
			avgValue += m_Values[i];
		avgValue /= (f32)i;
		if (core::math::Abs(m_FilteredValue - avgValue) >
				core::math::Abs(m_FilteredValue * (f32)HistProcent / 100.0f))
			m_FilteredValue = avgValue;
		m_Dirty = false;
	}

	T	m_Values[SamplesNumber];
	T	m_FilteredValue;
	bool m_Dirty;
	u32	m_CurrentIndex, m_Count;
};

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------

#endif // _filter_h_