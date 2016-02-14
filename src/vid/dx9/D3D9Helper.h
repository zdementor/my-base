//|-------------------------------------------------------------------------
//| File:        D3D9Helper.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef D3D9HelperHPP
#define D3D9HelperHPP

//----------------------------------------------------------------------------

inline DWORD convertToD3D9CompareFunction ( E_COMPARE_TEST cf ) 
{
	if      ( cf == ECT_NEVER		) { return D3DCMP_NEVER;        }
	else if ( cf == ECT_ALWAYS		) { return D3DCMP_ALWAYS;       }
	else if ( cf == ECT_LESS		) { return D3DCMP_LESS;         }
	else if ( cf == ECT_LEQUAL		) { return D3DCMP_LESSEQUAL;    }
	else if ( cf == ECT_EQUAL		) { return D3DCMP_EQUAL;        }
	else if ( cf == ECT_NOTEQUAL	) { return D3DCMP_NOTEQUAL;     }
	else if ( cf == ECT_GEQUAL		) { return D3DCMP_GREATEREQUAL; }
	else if ( cf == ECT_GREATER		) { return D3DCMP_GREATER;      }

	// to shut the compiler up
	return D3DCMP_ALWAYS;
}

//----------------------------------------------------------------------------

inline DWORD convertToD3D9SrcBlendFunction( E_SRC_BLEND_FUNCTION srcfunc ) 
{
	if      ( srcfunc == ESBF_ONE                 ) { return D3DBLEND_ONE;          }
	else if ( srcfunc == ESBF_ZERO                ) { return D3DBLEND_ZERO;         }
	else if ( srcfunc == ESBF_DST_COLOR           ) { return D3DBLEND_DESTCOLOR;    }
	else if ( srcfunc == ESBF_ONE_MINUS_DST_COLOR ) { return D3DBLEND_INVDESTCOLOR; }
	else if ( srcfunc == ESBF_SRC_ALPHA           ) { return D3DBLEND_SRCALPHA;     }
	else if ( srcfunc == ESBF_ONE_MINUS_SRC_ALPHA ) { return D3DBLEND_INVSRCALPHA;  }
	else if ( srcfunc == ESBF_DST_ALPHA           ) { return D3DBLEND_DESTALPHA;    }
	else if ( srcfunc == ESBF_ONE_MINUS_DST_ALPHA ) { return D3DBLEND_INVDESTALPHA; }
	else if ( srcfunc == ESBF_SRC_ALPHA_SATURATE  ) { return D3DBLEND_SRCALPHASAT;  }

	// to keep compiler happy
	return D3DBLEND_ONE;
}

//----------------------------------------------------------------------------

inline DWORD convertToD3D9DstBlendFunction( E_DST_BLEND_FUNCTION dstfunc ) 
{		
	if      ( dstfunc == EDBF_ONE                 ) { return D3DBLEND_ONE;          }
	else if ( dstfunc == EDBF_ZERO                ) { return D3DBLEND_ZERO;         }
	else if ( dstfunc == EDBF_SRC_ALPHA           ) { return D3DBLEND_SRCALPHA;     }
	else if ( dstfunc == EDBF_ONE_MINUS_SRC_ALPHA ) { return D3DBLEND_INVSRCALPHA;  }
	else if ( dstfunc == EDBF_DST_ALPHA           ) { return D3DBLEND_DESTALPHA;    }
	else if ( dstfunc == EDBF_ONE_MINUS_DST_ALPHA ) { return D3DBLEND_INVDESTALPHA; }
	else if ( dstfunc == EDBF_SRC_COLOR           ) { return D3DBLEND_SRCCOLOR;     }
	else if ( dstfunc == EDBF_ONE_MINUS_SRC_COLOR ) { return D3DBLEND_INVSRCCOLOR;  }

	// to keep compiler happy
	return D3DBLEND_ONE;
}

//----------------------------------------------------------------------------

inline DWORD convertToD3D9FogMode( E_FOG_TYPE fogtype ) 
{
	if      ( fogtype == EFT_LINEAR ) { return D3DFOG_LINEAR; }
	else if ( fogtype == EFT_EXP    ) { return D3DFOG_EXP;    }
	else if ( fogtype == EFT_EXP2   ) { return D3DFOG_EXP2;   }	

	// to keep compiler happy
	return D3DFOG_LINEAR;
}

//----------------------------------------------------------------------------

D3DVERTEXELEMENT9 D3D9VertexElements_SIMPLE[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_SIMPLE_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_1TCOORDS[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_1TCOORDS_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_2TCOORDS[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_2TCOORDS_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_1TCOORDS_TBN[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_1TCOORDS_TBN_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_2TCOORDS_TBN[] =

{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_2TCOORDS_TBN_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 56, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9 D3D9VertexElements_1TCOORDS_RHW_COLOURED[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 },
	{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

D3DVERTEXELEMENT9 *D3D9VertexElements[E_VERTEX_TYPE_COUNT] =
{
	D3D9VertexElements_SIMPLE,
	D3D9VertexElements_SIMPLE_COLOURED,
	D3D9VertexElements_1TCOORDS,
	D3D9VertexElements_1TCOORDS_COLOURED,
	D3D9VertexElements_2TCOORDS,
	D3D9VertexElements_2TCOORDS_COLOURED,
	D3D9VertexElements_1TCOORDS_TBN,
	D3D9VertexElements_1TCOORDS_TBN_COLOURED,
	D3D9VertexElements_2TCOORDS_TBN,
	D3D9VertexElements_2TCOORDS_TBN_COLOURED,
	D3D9VertexElements_1TCOORDS_RHW_COLOURED,
};

//----------------------------------------------------------------------------

inline DWORD convertToD3D9VertexType( E_VERTEX_TYPE vtype ) 
{
	switch ( vtype )
	{
	case EVT_SIMPLE:
		{
			return (D3DFVF_XYZ);
		}
	case EVT_SIMPLE_COLOURED:
		{
			return (D3DFVF_XYZ | D3DFVF_DIFFUSE);
		}
	case EVT_1TCOORDS:
		{
			return (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
		}
	case EVT_1TCOORDS_COLOURED:
		{
			return (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		}
	case EVT_2TCOORDS:
		{
			return (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2);
		}
	case EVT_2TCOORDS_COLOURED:
		{
			return (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);
		}
	case EVT_1TCOORDS_TBN:
		{
			return (
				D3DFVF_XYZ | D3DFVF_NORMAL | 
				D3DFVF_TEX3 |
				D3DFVF_TEXCOORDSIZE2(0) | // real texture coord 0
				D3DFVF_TEXCOORDSIZE3(1) | // misuse texture coord 1 for tangent
				D3DFVF_TEXCOORDSIZE3(2)   // misuse texture coord 2 for binormal
				);
		}
	case EVT_1TCOORDS_TBN_COLOURED:
		{
			return (
				D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | 
				D3DFVF_TEX3 |
				D3DFVF_TEXCOORDSIZE2(0) | // real texture coord 0
				D3DFVF_TEXCOORDSIZE3(1) | // misuse texture coord 1 for tangent
				D3DFVF_TEXCOORDSIZE3(2)   // misuse texture coord 2 for binormal
				);
		}
	case EVT_2TCOORDS_TBN:
		{
			return (
				D3DFVF_XYZ | D3DFVF_NORMAL | 
				D3DFVF_TEX4 |
				D3DFVF_TEXCOORDSIZE2(0) | // real texture coord 0
				D3DFVF_TEXCOORDSIZE2(1) | // real texture coord 1
				D3DFVF_TEXCOORDSIZE3(2) | // misuse texture coord 2 for tangent
				D3DFVF_TEXCOORDSIZE3(3)   // misuse texture coord 3 for binormal
				);
		}
	case EVT_2TCOORDS_TBN_COLOURED:
		{
			return (
				D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | 
				D3DFVF_TEX4 |
				D3DFVF_TEXCOORDSIZE2(0) | // real texture coord 0
				D3DFVF_TEXCOORDSIZE2(1) | // real texture coord 1
				D3DFVF_TEXCOORDSIZE3(2) | // misuse texture coord 2 for tangent
				D3DFVF_TEXCOORDSIZE3(3)   // misuse texture coord 3 for binormal
				);
		}
	case EVT_1TCOORDS_RHW_COLOURED:
		{
			return (
				D3DFVF_XYZRHW |
				D3DFVF_DIFFUSE |
				D3DFVF_TEX1
				);
		}
	}

	return (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1); 
}

//---------------------------------------------------------------------------

inline D3DSTENCILOP convertToD3D9StencilOp( E_STENCIL_OPERATION sop )
{
	if      ( sop == ESO_KEEP      ) { return D3DSTENCILOP_KEEP;    }
	else if ( sop == ESO_ZERO      ) { return D3DSTENCILOP_ZERO;    }
	else if ( sop == ESO_REPLACE   ) { return D3DSTENCILOP_REPLACE; }	
	else if ( sop == ESO_INCR_WRAP ) { return D3DSTENCILOP_INCRSAT; }
	else if ( sop == ESO_DECR_WRAP ) { return D3DSTENCILOP_DECRSAT; }
	else if ( sop == ESO_INVERT    ) { return D3DSTENCILOP_INVERT;  }
	else if ( sop == ESO_INCR      ) { return D3DSTENCILOP_INCR;    }
	else if ( sop == ESO_DECR      ) { return D3DSTENCILOP_DECR;    }

	// to keep compiler happy
	return D3DSTENCILOP_KEEP;
}

//---------------------------------------------------------------------------

inline D3DPRIMITIVETYPE convertToD3D9PrimitiveType( E_DRAW_PRIMITIVE_TYPE pt )
{
	if		( pt == EDPT_TRIANGLE_LIST	) return D3DPT_TRIANGLELIST;
	else if ( pt == EDPT_TRIANGLE_STRIP	) return D3DPT_TRIANGLESTRIP;
	else if ( pt == EDPT_TRIANGLE_FAN	) return D3DPT_TRIANGLEFAN;
	else if ( pt == EDPT_POINT_LIST		) return D3DPT_POINTLIST;
    else if ( pt == EDPT_LINE_LIST		) return D3DPT_LINELIST;
    else if ( pt == EDPT_LINE_STRIP		) return D3DPT_LINESTRIP;

	// to keep compiler happy
	return D3DPT_TRIANGLELIST;
}

//----------------------------------------------------------------------------

inline D3DCOLORVALUE colorToD3D(const img::SColor& col)
{
    const f32 f = 1.0f / 255.0f;
    static D3DCOLORVALUE v;
    v.r = col.getRed() * f;
    v.g = col.getGreen() * f;
    v.b = col.getBlue() * f;
    v.a = col.getAlpha() * f;
    return v;
}

//----------------------------------------------------------------------------

inline D3DBLENDOP convertToD3D9BlendEquation(E_BLEND_EQUATION eq)
{
	if		(eq == EBE_ADD				) return D3DBLENDOP_ADD;
	else if (eq == EBE_SUBTRACT			) return D3DBLENDOP_SUBTRACT;
	else if (eq == EBE_REVERSE_SUBTRACT	) return D3DBLENDOP_REVSUBTRACT;
	else if (eq == EBE_MIN				) return D3DBLENDOP_MIN;
	else if (eq == EBE_MAX				) return D3DBLENDOP_MAX;

	return D3DBLENDOP_ADD;
}

//----------------------------------------------------------------------------

#endif // D3D9HelperHPP