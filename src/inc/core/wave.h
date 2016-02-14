//|-------------------------------------------------------------------------
//| File:        wave.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef waveHPP
#define waveHPP

//---------------------------------------------------------------------------
namespace my {
namespace core { 
//---------------------------------------------------------------------------

class __MY_CORE_LIB_API__ wave
{
public:

	E_FUNC_TYPE func;

	f32 base;
	f32 amplitude;
	f32 phase;
	f32 frequency;

	wave() { defaultInit(); }	
	wave(E_FUNC_TYPE type) { defaultInit(EFT_SIN); }
	wave(E_FUNC_TYPE type, f32 bas, f32 ampl, f32 ph, f32 freq)
		: func(type), base(bas), amplitude(ampl), phase(ph), frequency(freq) {}	
	
	void defaultInit(E_FUNC_TYPE type = EFT_SIN)
	{
		func = type;
		base = 0.0f;
		amplitude = 1.0f;
		phase = 0.5f;
		frequency = 1.0f;
	}

	//! Compare operator <
    bool operator < ( const wave& w ) const
    {
		if (func != w.func)
			return func < w.func;
		if (base != w.base)
			return base < w.base;
		if (amplitude != w.amplitude)
			return amplitude < w.amplitude;
		if (phase != w.phase)
			return phase < w.phase;
		if (frequency != w.frequency)
			return frequency < w.frequency;

		return false;
	}	

	//! Compare operator ==
    bool operator == (const wave& w) const
    {
		return 
			func==w.func && 
			math::Equals(base,      w.base     ) && 
			math::Equals(amplitude, w.amplitude) && 
			math::Equals(phase,     w.phase    ) && 
			math::Equals(frequency, w.frequency);
	}	

    //! Compare operator !=
    bool operator != (const wave& w) const
    { return !((*this)==w); } 

	//! = operator
	wave& operator = (const wave &other)    
	{ return set(other); }

	wave& set(const wave &other) 
	{
		func = other.func;
		base = other.base;
		amplitude = other.amplitude;
		phase = other.phase;
		frequency = other.frequency;
		return *this; 
	}

	//! getting current wave value
	f32 get_value(u32 curr_time_ms) const;

	u32 get_hash() const
	{
		u32 hash = (func << 24) & 0x0f000000;
		hash ^= ((u32)(base		* 10000.f)) & 0xffffff;
		hash ^= ((u32)(amplitude* 10000.f)) & 0xffffff;
		hash ^= ((u32)(phase	* 10000.f)) & 0xffffff;
		hash ^= ((u32)(frequency* 10000.f)) & 0xffffff;
		return hash;
	}
}; 

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------

#endif
