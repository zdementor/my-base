//|-------------------------------------------------------------------------
//| File:        SColor.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef SColorHPP
#define SColorHPP
//----------------------------------------------------------------------------

#include <my_inc.h>

//----------------------------------------------------------------------------
namespace my {
namespace img {
//----------------------------------------------------------------------------

//! Creates a 16 bit A1R5G5B5 color
inline s16 RGBA16(s32 r, s32 g, s32 b, s32 a)
{
	return (
		( a & 0x80 ) << 8 |
		( r & 0xF8 ) << 7 |
		( g & 0xF8 ) << 2 |
		( b & 0xF8 ) >> 3
		);
}

//----------------------------------------------------------------------------

//! Creates a 16 bit A1R5G5B5 color
inline s16 RGB16(s32 r, s32 g, s32 b)
{
	return RGBA16(r,g,b,0xFF);
}

//----------------------------------------------------------------------------

//! Converts a 32 bit (X8R8G8B8) color to a 16 A1R5G5B5 color
inline s16 X8R8G8B8toA1R5G5B5(s32 color)
{
	return (
		( color & 0x00F80000 ) >> 9 |
		( color & 0x0000F800 ) >> 6 |
		( color & 0x000000F8 ) >> 3
		);
}

//----------------------------------------------------------------------------

//! Converts a 32 bit (A8R8G8B8) color to a 16 A1R5G5B5 color
inline u16 A8R8G8B8toA1R5G5B5(u32 color)
{
	return (
		( color & 0x80000000 ) >> 16|
		( color & 0x00F80000 ) >> 9 |
		( color & 0x0000F800 ) >> 6 |
		( color & 0x000000F8 ) >> 3
		);
}

//----------------------------------------------------------------------------

//! Converts a 32 bit (A8R8G8B8) color to a 16 A1R5G5B5 color
inline u16 A8R8G8B8toR5G6B5(u32 color)
{
	return (
		( color & 0x00F80000 ) >> 8 |
		( color & 0x0000FC00 ) >> 5 |
		( color & 0x000000F8 ) >> 3
		);
}

//----------------------------------------------------------------------------

//! Returns A8R8G8B8 Color from A1R5G5B5 color
inline u32 A1R5G5B5toA8R8G8B8(u16 color)
{
	return (
		( color & 0x8000 ) << 16|
		( color & 0x7C00 ) << 9 |
		( color & 0x03E0 ) << 6 |
		( color & 0x001F ) << 3
		);
}

//----------------------------------------------------------------------------

//! Returns A8R8G8B8 Color from R5G6B5 color
inline s32 R5G6B5toA8R8G8B8(s16 color)
{
	return (
		0xFF000000 |
		((color & 0xF800) << 8)|
		((color & 0x07E0) << 5)|
		((color & 0x001F) << 3)
		);
}

//----------------------------------------------------------------------------

//! Returns A1R5G5B5 Color from R5G6B5 color
inline s16 R5G6B5toA1R5G5B5(s16 color)
{ return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F)); }

//----------------------------------------------------------------------------

//! Returns R5G6B5 Color from A1R5G5B5 color
inline s16 A1R5G5B5toR5G6B5(s16 color)
{ return (((color & 0x7FE0) << 1) | (color & 0x1F)); }

//----------------------------------------------------------------------------

//! Returns A8R8G8B8 Color from A8B8G8R8 color
inline s32 A8R8G8B8toA8B8G8R8(s32 color)
{ return (color & 0xff00ff00) | ((color & 0xff) << 16) | ((color >> 16) & 0xff); }

//----------------------------------------------------------------------------

//! Returns A8B8G8R8 Color from A8R8G8B8 color
inline s32 A8B8G8R8toA8R8G8B8(s32 color)
{ return (color & 0xff00ff00) | ((color & 0xff) << 16) | ((color >> 16) & 0xff); }

//----------------------------------------------------------------------------

//! Converts a 32 bit (A8B8G8R8) color to a 16 A1R5G5B5 color
inline u16 A8B8G8R8toA1R5G5B5(u32 color)
{
	return (
		( color & 0x80000000 ) >> 16|
		( color & 0x00F80000 ) >> 3 |
		( color & 0x0000F800 ) >> 6 |
		( color & 0x000000F8 ) >> 9
		);
}

//----------------------------------------------------------------------------

//! Converts a 32 bit (A8B8G8R8) color to a 16 A1R5G5B5 color
inline u16 A8B8G8R8toR5G6B5(u32 color)
{
	return (
		( color & 0x00F80000 ) >> 3 |
		( color & 0x0000FC00 ) >> 5 |
		( color & 0x000000F8 ) >> 8
		);
}

//----------------------------------------------------------------------------

//! Returns the red component from the color
inline s32 getAlpha(s16 color)
{   
	return ((color >> 11)&0x1);    
}

//----------------------------------------------------------------------------

//! Returns the red component from the color
inline s32 getRed(s16 color)
{   
	return ((color >> 10)&0x1F);
}

//----------------------------------------------------------------------------

//! Returns the green component from the color
inline s32 getGreen(s16 color)
{   
	return ((color >> 5)&0x1F);
}

//----------------------------------------------------------------------------

//! Returns the blue component from the color
inline s32 getBlue(s16 color)
{   
	return ((color)&0x1F);
}

//----------------------------------------------------------------------------

//! Returns the luminance from a 16 bit A1R5G5B5 color 
inline s32 getLuminance(s16 color)
{
	return ((getRed(color)<<3) + (getGreen(color)<<3) + (getBlue(color)<<3)) / 3;
}

//----------------------------------------------------------------------------

struct SColorf;

static inline void ToSColor(const SColorf &in, SColor &out);

//----------------------------------------------------------------------------

//! Class representing a 32 bit ARGB color.
//! The color values for red, green, blue
//! and alpha are stored in a single s32. So all four values may be between 0 and 255.
//!This class is used by most parts of the Irrlicht Engine
//!to specify a color. An other way is using the class Colorf, which
//!stores the color values in 4 floats.
struct SColor
{
    //! Constructors
	SColor() : color(0) {}
    SColor(const SColor &other)
		: color(other.color) {}
    SColor(const SColorf &other)
	{ set(other); }
    SColor(u32 clr) : color(clr) {}
    SColor(u8 a, u8 r, u8 g, u8 b)
		: alpha(a), red(r), green(g), blue(b) {}

	//! Returns the alpha component of the color. The alpha component
	//! defines how transparent a color should be.
	//! 0 means not transparent, 255 means fully transparent.
	u8 getAlpha() const
	{ return alpha; }

	//! Returns the red component of the color. 
	//! \return Returns a value between 0 and 255, specifying how red the color is.
	//! 0 means dark, 255 means full red.
	u8 getRed() const
	{ return red; }

	//! Returns the green component of the color. 
	//! \return Returns a value between 0 and 255, specifying how green the color is.
	//! 0 means dark, 255 means full green.
	u8 getGreen() const
	{ return green; }

	//! Returns the blue component of the color.
	//! \return Returns a value between 0 and 255, specifying how blue the color is.
	//! 0 means dark, 255 means full blue.
	u8 getBlue() const
	{ return blue; }

    //! Sets the alpha comonent of the Color. The alpha component
    //! defines how transparent a color should be.
    //! \param a: Has to be a value between 0 and 255. 
    //! 0 means not transparent, 255 means fully transparent.
    void setAlpha(u8 a)
	{ alpha = a; }

    //! Sets the red comonent of the Color. 
    //! \param r: Has to be a value between 0 and 255. 
    //! 0 means dark red (=black), 255 means full red.
    void setRed(u8 r) 
	{ red = r; }

    //! Sets the green comonent of the Color. 
    //! \param g: Has to be a value between 0 and 255. 
    //! 0 means dark green (=black), 255 means full green.
    void setGreen(u8 g)
	{ green = g; }

    //! Sets the blue comonent of the Color. 
    //! \param b: Has to be a value between 0 and 255. 
    //! 0 means dark blue (=black), 255 means full blue.
    void setBlue(u8 b)
	{ blue = b; }

    //! Calculates a 16 bit A1R5G5B5 value of this color.
    //! \return Returns the 16 bit A1R5G5B5 value of this color.
    s16 toA1R5G5B5() const 
	{ return A8R8G8B8toA1R5G5B5(color); }

	//! Calculates a 16 bit R5G6B5 value of this color.
    //! \return Returns the 16 bit R5G6B5 value of this color.
    s16 toR5G6B5() const 
	{ return A8R8G8B8toR5G6B5(color); }	

    //! Converts color to open gl color format.
    //! \return Returns the 32 bit openGL color value.
    s32 toOpenGLColor() const
    { return A8R8G8B8toA8B8G8R8(color); }

    //! Sets all four components of the color at once.
    //! Constructs the color from 4 values representing the alpha, red, green and 
    //! blue components of the color. Must be values between 0 and 255.
    //! \param a: Alpha component of the color. 
    //! The alpha component defines how transparent a color should be.
    //! Has to be a value between 0 and 255. 
    //! 0 means not transparent, 255 means fully transparent.
    //! \param r: Sets the red comonent of the Color. 
    //! Has to be a value between 0 and 255. 
    //! 0 means dark red (=black), 255 means full red.
    //! \param g: Sets the green comonent of the Color. 
    //! Has to be a value between 0 and 255. 
    //! 0 means dark green (=black), 255 means full green.
    //! \param b: Sets the blue comonent of the Color. 
    //! Has to be a value between 0 and 255.
    //! 0 means dark blue (=black), 255 means full blue.
    void set(u8 a, u8 r, u8 g, u8 b)
	{ alpha = a, red = r, green = g, blue = b; }

	void set(const SColor &other)
	{ color = other.color; }

	void set(const SColorf &other)
	{ ToSColor(other, *this); }

	//! Assignment operator
	void operator = (const SColor& other)
	{ color = other.color; }

	void operator = (const u32 &c)
	{ color = c; }

    //! Compares the color to another color.
    //! \return Returns true if the color less than the other
    bool operator < (const SColor& other) const 
	{
		if (color != other.color)
			return color < other.color;
		return false;
	}

	bool operator < (const u32 &c) const 
	{
		if (color != c)
			return color < c;
		return false;
	}

    //! Compares the color to another color.
    //! \return Returns true if the colors are the same, and false if not.
    bool operator == (const SColor& other) const 
	{ return other.color == color; }

	bool operator == (const u32 &c) const 
	{ return color == c; }

    //! Compares the color to another color.
    //! \return Returns true if the colors are different, and false if they are the same.
    bool operator != (const SColor& other) const 
	{ return other.color != color; }

	bool operator != (const u32 &c) const 
	{ return color != c; }

	img::SColor operator * (f32 k) const
	{
		return img::SColor(
			u8(alpha * k), u8(red * k), u8(green * k), u8(blue * k));
	}

	img::SColor& operator *= (f32 k)
	{
		red		= u8(k * red);
		green	= u8(k * green);
		blue	= u8(k * blue);
		alpha	= u8(k * alpha);
		return *this;
	}

    //! Interpolates the color with a f32 value to an other color 
    //! \param other: Other color
    //! \param d: value between 0.0f and 1.0f
    //! \return Returns interpolated color.
    SColor getInterpolated(SColor other, f32 d) const
    {
        f32 inv = 1.0f - d;
        return SColor(
			(u8)(other.alpha* inv + alpha	* d),
            (u8)(other.red	* inv + red		* d),
            (u8)(other.green* inv + green	* d),
            (u8)(other.blue	* inv + blue	* d));
    }

    //! Interpolates the color with a f32 value to an other color 
    //! \param other: Other color
    //! \param d: value between 0.0f and 1.0f
    //! \return Returns interpolated color.
    const SColor& interpolate(SColor other, f32 d)
    {
        f32 inv = 1.0f - d;
        alpha	= (u8)(other.alpha	* inv + alpha	* d);
        red		= (u8)(other.red	* inv + red		* d);
        green	= (u8)(other.green	* inv + green	* d);
        blue	= (u8)(other.blue	* inv + blue	* d);
		return *this;
    }

	inline f32 getIntensity ()
	{
		u32 val = red + green + blue;
		CHECK_RANGE(val, 0, 255);
		return (f32)val / 255.0f;
	}

    //! color in A8R8G8B8 Format
	union
	{
		u32 color;
		struct
		{
			u8 blue, green, red, alpha;
		};
		struct
		{
			u8 bgra[4];
		};
		struct
		{
			u16 depth16;
			u16 depth16tophalf;
		};
	};
};

//----------------------------------------------------------------------------

//! Class representing a color with four floats.
//! The color values for red, green, blue
//! and alpha are each stored in a 32 bit floating point variable.
//! So all four values may be between 0.0f and 1.0f.
//! This class is used rarely used by the Irrlicht Engine
//! to specify a color. An other, faster way is using the class Color, which
//! stores the color values in a single 32 bit integer.
struct SColorf
{
    //! Constructs a color. All values are initialised with 0.0f, resulting
    //! in a black color.
    SColorf () : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

    //! Constructs a color from three color values: red, green and blue.
    //! \param r: Red color component. Should be a value between 0.0f meaning
    //! very dark red (=black) and 1.0f, meaning full red.
    //! \param g: Green color component. Should be a value between 0.0f meaning
    //! very dark green (=black) and 1.0f, meaning full green.
    //! \param b: Blue color component. Should be a value between 0.0f meaning
    //! very dark blue (=black) and 1.0f, meaning full blue.
    SColorf(f32 r, f32 g, f32 b) : r(r), g(g), b(b), a(1.0f) {}

    //! Constructs a color from four color values: alpha, red, green and blue.
    //! \param a: Alpha color component of the color.
    //! The alpha component defines how transparent a color should be.
    //! Has to be a value between 0.0f and 1.0f,
    //! 0.0f means not transparent, 1.0f means fully transparent.
    //! \param r: Red color component. Should be a value between 0.0f meaning
    //! very dark red (=black) and 1.0f, meaning full red.
    //! \param g: Green color component. Should be a value between 0.0f meaning
    //! very dark green (=black) and 1.0f, meaning full green.
    //! \param b: Blue color component. Should be a value between 0.0f meaning
    //! very dark blue (=black) and 1.0f, meaning full blue.
    SColorf(f32 r, f32 g, f32 b, f32 a) : r(r), g(g), b(b), a(a) {}

    //! Constructs a color from 32 bit Color.
    //! \param c: 32 bit color value from which this Colorf class is
    //! constructed from.
    SColorf(const SColor &other)
	{ set(other); }

    SColorf(const SColorf &other)
	{ set(other); }

    //! Converts this color to a SColor without floats.
    SColor toSColor() const
    { return SColor((u8)(a*255.0f), (u8)(r*255.0f), (u8)(g*255.0f), (u8)(b*255.0f)); }

	//! Compares the color to another color.
    //! \return Returns true if the colors are different, and false if they are the same.
    bool operator != ( const SColorf& other ) const
	{ return r!=other.r || g!=other.g || b!=other.b || a!=other.a; }

    SColorf operator * ( const f32& k ) const 
	{ return SColorf(r*k, g*k, b*k, a*k); }

    SColorf& operator *= ( const f32& k )
	{ 
		r*=k; g*=k; b*=k; a*=k;
		return *this; 
	}

    //! red color component
    f32 r;

    //! green color component
    f32 g;

    //! blue component
    f32 b;

    //! alpha color component
    f32 a;

    //! Sets the three color comonents to new values at once.
    //! \param rr: Red color component. Should be a value between 0.0f meaning
    //! very dark red (=black) and 1.0f, meaning full red.
    //! \param gg: Green color component. Should be a value between 0.0f meaning
    //! very dark green (=black) and 1.0f, meaning full green.
    //! \param bb: Blue color component. Should be a value between 0.0f meaning
    //! very dark blue (=black) and 1.0f, meaning full blue.
    void set(f32 rr, f32 gg, f32 bb)
	{ r = rr; g =gg; b = bb; }
    void set(f32 rr, f32 gg, f32 bb, f32 aa)
	{ r = rr; g =gg; b = bb; a = aa; }

	void set(const SColor &other)
	{ 
		static const f32 inv = 1.0f / 255.0f; 
		r = other.getRed()   * inv; 
		g = other.getGreen() * inv; 
		b = other.getBlue()  * inv; 
		a = other.getAlpha() * inv; 
	}
    void set(const SColorf &other)
	{  (*this) = other; }
};

//----------------------------------------------------------------------------

static inline void ToSColor(const SColorf &in, SColor &out)
{
	out.set((u8)(in.a*255.0f), (u8)(in.r*255.0f),
		(u8)(in.g*255.0f), (u8)(in.b*255.0f));
}

//----------------------------------------------------------------------------
} // end namespace img
} // end namespace my
//----------------------------------------------------------------------------

#endif
