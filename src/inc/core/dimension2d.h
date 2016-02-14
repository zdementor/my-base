//|-------------------------------------------------------------------------
//| File:        dimension2d.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef dimension2dHPP
#define dimension2dHPP

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

//! Specifies a 2 dimensional size.
template <class T>
class dimension2d
{
public:

	dimension2d()
		: Width(0), Height(0) {};

    dimension2d(T width, T height)
		: Width(width), Height(height) {};

    dimension2d(const dimension2d<T>& other)
        : Width(other.Width), Height(other.Height) {};

	inline bool operator == (const dimension2d<T> &other) const
    {
		return Width == other.Width && Height == other.Height;
    }

    inline bool operator != (const dimension2d<T> &other) const
    {
		return Width != other.Width || Height != other.Height;
    }

	template <class TT>
    inline const dimension2d<T>& operator = (const dimension2d<TT> &other) 
    {
		Width = (T)other.Width;
        Height = (T)other.Height;
        return *this;
    }

	template <class TT>
    inline void set(TT width, TT height) 
    {
		Width = (T)width;
        Height = (T)height;
    }

	template <class TT>
    inline void set(const dimension2d<TT>& other) 
    {
		(*this) = other;
    }

    T Width, Height;
};

//----------------------------------------------------------------------------

//! Typedef for a f32 dimension2d
typedef dimension2d<f32> dimension2df;

//! Typedef for an integer dimension2d
typedef dimension2d<s32> dimension2di;

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif

