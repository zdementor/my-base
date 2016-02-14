//|-------------------------------------------------------------------------
//| File:        S3DVertex.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef S3DVertexHPP
#define S3DVertexHPP
//---------------------------------------------------------------------------

#include <my_inc.h>
#include <img/SColor.h>

//---------------------------------------------------------------------------
namespace my {
namespace vid {
//---------------------------------------------------------------------------

//! basic vertex for all kinds of vertices
struct S3DBaseVertex
{
	//! default constructor
    S3DBaseVertex() {} 

    //! constructor
    S3DBaseVertex(f32 x, f32 y, f32 z) : Pos(x,y,z) {}

    //! constructor
    S3DBaseVertex(const core::vector3df& pos) :	Pos(pos) {}

    //! Position
    core::vector3df Pos; 
};

//---------------------------------------------------------------------------

//! Simple vertex
struct S3DVertexSimple : public S3DBaseVertex
{
    //! default constructor
    S3DVertexSimple() {} 

    //! constructor
    S3DVertexSimple(f32 x, f32 y, f32 z) : S3DBaseVertex(x,y,z) {}

    //! constructor
    S3DVertexSimple(const core::vector3df& pos) : S3DBaseVertex(pos) {}  

	S3DVertexSimple getInterpolated(
		const S3DVertexSimple& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertexSimple interpolated;

		if (with_pos)
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

	const core::vector3df& getNormal() const
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Simple vertex with color data (for per vertex color effects)
struct S3DVertexSimpleColoured : public S3DBaseVertex
{
    //! default constructor
    S3DVertexSimpleColoured() {} 

    //! constructor
    S3DVertexSimpleColoured(f32 x, f32 y, f32 z, img::SColor c) : 
		S3DBaseVertex(x,y,z), Color(c) {}

    //! constructor
    S3DVertexSimpleColoured(const core::vector3df& pos, img::SColor& color) : 
		S3DBaseVertex(pos), Color(color) {}

	S3DVertexSimpleColoured getInterpolated(
		const S3DVertexSimpleColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertexSimpleColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		return interpolated;
	}

	core::vector3df& getNormal()
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

	const core::vector3df& getNormal() const
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

	//! Color
    img::SColor Color;   

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Standard vertex
struct S3DVertex1TCoords : public S3DBaseVertex
{
    //! default constructor
    S3DVertex1TCoords() {} 

    //! constructor
    S3DVertex1TCoords(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, f32 tu, f32 tv
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), TCoords(tu,tv) {}

    //! constructor
    S3DVertex1TCoords(
		const core::vector3df& pos, const core::vector3df& normal,
        const core::vector2df& tcoords
		) : 
	S3DBaseVertex(pos), Normal(normal), TCoords(tcoords) {}  

	S3DVertex1TCoords getInterpolated(
		const S3DVertex1TCoords& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex1TCoords interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

    //! Texture coordinates
    core::vector2df TCoords;   

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Standard vertex with color data (for per vertex color effects)
struct S3DVertex1TCoordsColoured : public S3DBaseVertex
{
    //! default constructor
    S3DVertex1TCoordsColoured() {} 

    //! constructor
    S3DVertex1TCoordsColoured(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, img::SColor c, f32 tu, f32 tv
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv) {}

    //! constructor
    S3DVertex1TCoordsColoured(
		const core::vector3df& pos, const core::vector3df& normal,
        img::SColor& color, const core::vector2df& tcoords
		) : 
	S3DBaseVertex(pos), Normal(normal), Color(color), TCoords(tcoords) {}

	S3DVertex1TCoordsColoured getInterpolated(
		const S3DVertex1TCoordsColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex1TCoordsColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

	//! Color
    img::SColor Color;   

    //! Texture coordinates
    core::vector2df TCoords;   

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with two texture coordinates
struct S3DVertex2TCoords : public S3DBaseVertex
{
    //! default constructor
    S3DVertex2TCoords() {}

    //! constructor
    S3DVertex2TCoords(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, f32 tu, f32 tv, f32 tu2, f32 tv2
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), TCoords(tu,tv), TCoords2(tu2,tv2) {}
	
	S3DVertex2TCoords getInterpolated(
		const S3DVertex2TCoords& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex2TCoords interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;
		interpolated.TCoords2 = TCoords2 + ( other.TCoords2 - TCoords2) * d;

		return interpolated;
	}
	
	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

    //! First set of texture coordinates
    core::vector2df TCoords;

    //! Second set of texture coordinates
    core::vector2df TCoords2;  

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with two texture coordinates with color data (for per vertex color effects)
struct S3DVertex2TCoordsColoured : public S3DBaseVertex
{
    //! default constructor
    S3DVertex2TCoordsColoured() {};

    //! constructor
    S3DVertex2TCoordsColoured(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, img::SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv), TCoords2(tu2,tv2) {}

	S3DVertex2TCoordsColoured getInterpolated(
		const S3DVertex2TCoordsColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex2TCoordsColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;
		interpolated.TCoords2 = TCoords2 + ( other.TCoords2 - TCoords2) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

	//! Color
    img::SColor Color;

    //! First set of texture coordinates
    core::vector2df TCoords;

    //! Second set of texture coordinates
    core::vector2df TCoords2;  

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with a tangent and binormal vector.
//  Usually used for tangent space normal mapping.
struct S3DVertex1TCoordsTBN : public S3DBaseVertex
{
	//! default constructor
	S3DVertex1TCoordsTBN() {}

	//! constructor
	S3DVertex1TCoordsTBN(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz) {}

	//! constructor
	S3DVertex1TCoordsTBN(
		const core::vector3df& pos, const core::vector3df& normal,
		const core::vector2df& tcoords
		) : 
	S3DBaseVertex(pos), Normal(normal), TCoords(tcoords) {}

	S3DVertex1TCoordsTBN getInterpolated(
		const S3DVertex1TCoordsTBN& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex1TCoordsTBN interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;

		interpolated.Tangent  = Tangent  + ( other.Tangent -  Tangent ) * d;
		interpolated.Binormal = Binormal + ( other.Binormal - Binormal) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

	//! Texture coordinates
	core::vector2df TCoords;

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;	

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with a tangent and binormal vector (with color data).
//  Usually used for tangent space normal mapping.
struct S3DVertex1TCoordsTBNColoured : public S3DBaseVertex
{
	//! default constructor
	S3DVertex1TCoordsTBNColoured() {}

	//! constructor
	S3DVertex1TCoordsTBNColoured(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz) {}

	//! constructor
	S3DVertex1TCoordsTBNColoured(
		const core::vector3df& pos, const core::vector3df& normal,
		const core::vector2df& tcoords
		) : 
	S3DBaseVertex(pos), Normal(normal), TCoords(tcoords) {}

	S3DVertex1TCoordsTBNColoured getInterpolated(
		const S3DVertex1TCoordsTBNColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex1TCoordsTBNColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;

		interpolated.Tangent  = Tangent  + ( other.Tangent -  Tangent ) * d;
		interpolated.Binormal = Binormal + ( other.Binormal - Binormal) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

	//! Color
    img::SColor Color;

	//! Texture coordinates
	core::vector2df TCoords;

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;	

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	static E_VERTEX_TYPE	Type;
};

//---------------------------------------------------------------------------

//! Vertex with two texture coordinates, with a tangent and binormal vector
struct S3DVertex2TCoordsTBN : public S3DBaseVertex
{
    //! default constructor
    S3DVertex2TCoordsTBN() {}

    //! constructor
    S3DVertex2TCoordsTBN(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, f32 tu, f32 tv, f32 tu2, f32 tv2
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), TCoords(tu,tv), TCoords2(tu2,tv2) {}
	
	S3DVertex2TCoordsTBN getInterpolated(
		const S3DVertex2TCoordsTBN& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex2TCoordsTBN interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;
		interpolated.TCoords2 = TCoords2 + ( other.TCoords2 - TCoords2) * d;

		interpolated.Tangent  = Tangent  + ( other.Tangent -  Tangent ) * d;
		interpolated.Binormal = Binormal + ( other.Binormal - Binormal) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

    //! First set of texture coordinates
    core::vector2df TCoords;

    //! Second set of texture coordinates
    core::vector2df TCoords2;  

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;	

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with two texture coordinates, with a tangent and binormal vector (with color data)
struct S3DVertex2TCoordsTBNColoured : public S3DBaseVertex
{
    //! default constructor
    S3DVertex2TCoordsTBNColoured() {};

    //! constructor
    S3DVertex2TCoordsTBNColoured(
		f32 x, f32 y, f32 z, f32 nx, f32 ny, f32 nz, img::SColor c, f32 tu, f32 tv, f32 tu2, f32 tv2
		) : 
	S3DBaseVertex(x,y,z), Normal(nx,ny,nz), Color(c), TCoords(tu,tv), TCoords2(tu2,tv2) {}

	S3DVertex2TCoordsTBNColoured getInterpolated(
		const S3DVertex2TCoordsTBNColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex2TCoordsTBNColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		interpolated.Normal   = Normal   + ( other.Normal   - Normal  ) * d;
		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;
		interpolated.TCoords2 = TCoords2 + ( other.TCoords2 - TCoords2) * d;

		interpolated.Tangent  = Tangent  + ( other.Tangent -  Tangent ) * d;
		interpolated.Binormal = Binormal + ( other.Binormal - Binormal) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ return Normal; }

	const core::vector3df& getNormal() const
	{ return Normal; }

	//! Normal vector
    core::vector3df Normal; 

	//! Color
    img::SColor Color;

    //! First set of texture coordinates
    core::vector2df TCoords;

    //! Second set of texture coordinates
    core::vector2df TCoords2;  

	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;	

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	static E_VERTEX_TYPE Type;
};

//---------------------------------------------------------------------------

//! Vertex with a W-component, single tex coord and with color data. 
struct S3DVertex1TCoordsRHWColoured : public S3DBaseVertex
{
    //! default constructor
    S3DVertex1TCoordsRHWColoured() {};

    //! constructor
    S3DVertex1TCoordsRHWColoured(
		f32 x, f32 y, f32 z, f32 rhw, img::SColor c, f32 tu, f32 tv
		) : 
	S3DBaseVertex(x,y,z), RHW(rhw), Color(c), TCoords(tu,tv) {}

	S3DVertex1TCoordsRHWColoured getInterpolated(
		const S3DVertex1TCoordsRHWColoured& other, f32 d, bool with_pos=true
		) const
	{
		S3DVertex1TCoordsRHWColoured interpolated;

		if (with_pos)
		{
			interpolated.Pos = Pos + ( other.Pos - Pos ) * d;
			interpolated.RHW = RHW + ( other.RHW - RHW ) * d;
		}

		interpolated.Color    = Color.getInterpolated(other.Color, d);

		interpolated.TCoords  = TCoords  + ( other.TCoords  - TCoords ) * d;

		return interpolated;
	}

	core::vector3df& getNormal()
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

	const core::vector3df& getNormal() const
	{ static core::vector3df n(0.f, 0.f, 0.f); return n; }

    f32 RHW;

	//! Color
    img::SColor Color;

    //! First set of texture coordinates
    core::vector2df TCoords;

	static E_VERTEX_TYPE Type;	
};

//---------------------------------------------------------------------------

static const u32 VertexSize[E_VERTEX_TYPE_COUNT] =
{
	sizeof(S3DVertexSimple),
	sizeof(S3DVertexSimpleColoured),
	sizeof(S3DVertex1TCoords),
	sizeof(S3DVertex1TCoordsColoured),
	sizeof(S3DVertex2TCoords),
	sizeof(S3DVertex2TCoordsColoured),
	sizeof(S3DVertex1TCoordsTBN),
	sizeof(S3DVertex1TCoordsTBNColoured),
	sizeof(S3DVertex2TCoordsTBN),
	sizeof(S3DVertex2TCoordsTBNColoured),
	sizeof(S3DVertex1TCoordsRHWColoured),
};

//---------------------------------------------------------------------------

inline u32 getVertexSize(E_VERTEX_TYPE type)
{ return VertexSize[type]; }

//---------------------------------------------------------------------------

static const u32 VertexComponents[E_VERTEX_TYPE_COUNT] =
{
	EVC_POS,													// EVT_SIMPLE
	EVC_POS | EVC_COL,											// EVT_SIMPLE_COLOURED
	EVC_POS | EVC_TC0 | EVC_NRM,								// EVT_1TCOORDS
	EVC_POS | EVC_TC0 | EVC_NRM | EVC_COL,						// EVT_1TCOORDS_COLOURED
	EVC_POS | EVC_TC0 | EVC_TC1 | EVC_NRM,						// EVT_2TCOORDS
	EVC_POS | EVC_TC0 | EVC_TC1 | EVC_NRM | EVC_COL,			// EVT_2TCOORDS_COLOURED
	EVC_POS | EVC_TC0 | EVC_NRM | EVC_TBN,						// EVT_1TCOORDS_TBN
	EVC_POS | EVC_TC0 | EVC_NRM | EVC_TBN | EVC_COL,			// EVT_1TCOORDS_TBN_COLOURED
	EVC_POS | EVC_TC0 | EVC_TC1 | EVC_TBN | EVC_NRM,			// EVT_2TCOORDS_TBN
	EVC_POS | EVC_TC0 | EVC_TC1 | EVC_TBN | EVC_NRM | EVC_COL,	// EVT_2TCOORDS_TBN_COLOURED
	EVC_POS | EVC_TC0 | EVC_RHW | EVC_COL,						// EVT_1TCOORDS_RHW_COLOURED
};

//---------------------------------------------------------------------------

inline u32 getVertexComponents(E_VERTEX_TYPE type)
{ return VertexComponents[type]; }

//---------------------------------------------------------------------------

inline E_VERTEX_TYPE getVertexTypeFromComponents(u32 components)
{
	for (u32 i = 0; i < E_VERTEX_TYPE_COUNT; i++)
		if (VertexComponents[i] == components)
			return (E_VERTEX_TYPE)i;
	return EVT_SIMPLE;
}

//---------------------------------------------------------------------------

static const u32 IndexSize[E_INDEX_TYPE_COUNT] =
{
	sizeof(u16),
	sizeof(u32)
};

//---------------------------------------------------------------------------

inline u32 getIndexSize(E_INDEX_TYPE type)
{ return IndexSize[type]; }

//---------------------------------------------------------------------------
} // end namespace vid
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef S3DVertexHPP

