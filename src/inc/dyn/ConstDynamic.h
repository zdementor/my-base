//|-------------------------------------------------------------------------
//| File:        ConstDynamic.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_dynHPP
#define my_const_dynHPP

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
//---------------------------------------------------------------------------

//! dynamic types enumeration
enum E_DYNAMIC_OBJECT_TYPE
{	
	EDOT_NONE = 0,
    EDOT_MONOLITH,
	EDOT_DYNAMIC,
	EDOT_PSEUDO_DYNAMIC,
	EDOT_SENSELESS,

	E_DYNAMIC_OBJECT_TYPE_COUNT,	

	E_DYNAMIC_OBJECT_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! dynamic types as string
static const c8* DynamicObjectTypeStr[E_DYNAMIC_OBJECT_TYPE_COUNT] = 
{
	"EDOT_NONE", 
	"EDOT_MONOLITH", 
	"EDOT_DYNAMIC",
	"EDOT_PSEUDO_DYNAMIC",
	"EDOT_SENSELESS"
};

//----------------------------------------------------------------------------

//! dynamic types names as string
static const c8* DynamicObjectTypeShortStr[E_DYNAMIC_OBJECT_TYPE_COUNT] = 
{
	"none", 
	"Monolith", 
	"Dynamic",
	"Pseudo Dynamic",
	"Senseless"
};

//----------------------------------------------------------------------------

MY_INLINE const c8* getDynamicObjectTypeName(E_DYNAMIC_OBJECT_TYPE type)
{ return DynamicObjectTypeStr[type]; }

MY_INLINE const c8* getDynamicObjectTypeReadableName(E_DYNAMIC_OBJECT_TYPE type)
{ return DynamicObjectTypeShortStr[type]; }

//---------------------------------------------------------------------------

//! dynamic types enumeration
enum E_DYN_COLLID_FILTER
{	
	EDCF_NONE_COLLID            = 0x00000000,
    EDCF_MONOLITH_COLLID        = 0x00000001,
	EDCF_DYNAMIC_COLLID         = 0x00000002,
	EDCF_PSEUDO_DYNAMIC_COLLID  = 0x00000004,
	EDCF_SENSELESS_COLLID       = 0x00000008,

	E_DYN_COLLID_FILTER_COUNT = E_DYNAMIC_OBJECT_TYPE_COUNT,

	E_DYN_COLLID_FILTER_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! 
const s32 DynamicObjectCollidFilter[E_DYN_COLLID_FILTER_COUNT][2] = 
{
    {	EDCF_NONE_COLLID,     
			EDCF_NONE_COLLID,
	},
    {	EDCF_MONOLITH_COLLID, 
			EDCF_DYNAMIC_COLLID
	},
	{	EDCF_DYNAMIC_COLLID,  
			EDCF_MONOLITH_COLLID | 	EDCF_DYNAMIC_COLLID | 
			EDCF_PSEUDO_DYNAMIC_COLLID | EDCF_SENSELESS_COLLID
	},
	{	EDCF_PSEUDO_DYNAMIC_COLLID, 
			EDCF_DYNAMIC_COLLID
	},
	{	EDCF_SENSELESS_COLLID,      
			EDCF_DYNAMIC_COLLID
	}
};

//----------------------------------------------------------------------------

//! geom form enumeration
enum E_DYN_OBJ_GEOM_FORM
{
	EDOGF_BOX = 0,        // cube
    EDOGF_SPHERE,     // sphere
    EDOGF_XCCYLINDER, // capped cylinder with main axis X
    EDOGF_YCCYLINDER, // capped cylinder with main axis Y
    EDOGF_ZCCYLINDER, // capped cylinder with main axis Z
	EDOGF_MESH,
	EDOGF_HEIGHTMAP,

	E_DYN_OBJ_GEOM_FORM_COUNT,

	E_DYN_OBJ_GEOM_FORM_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! geom forms as string
static const c8* GeomFormStr[E_DYN_OBJ_GEOM_FORM_COUNT] = 
{
	"EDOGF_BOX", 
	"EDOGF_SPHERE", 
	"EDOGF_XCCYLINDER", 
	"EDOGF_YCCYLINDER", 
	"EDOGF_ZCCYLINDER", 
	"EDOGF_MESH", 
	"EDOGF_HEIGHTMAP"
};

//----------------------------------------------------------------------------

//! geom forms names as string
static const c8* GeomFormShortStr[E_DYN_OBJ_GEOM_FORM_COUNT] = 
{
	"Box", 
	"Sphere", 
	"XCCylinder", 
	"YCCylinder", 
	"ZCCylinder", 
	"Mesh", 
	"Heightmap"
};

//----------------------------------------------------------------------------

MY_INLINE const c8* getGeomFormName(E_DYN_OBJ_GEOM_FORM type)
{ return GeomFormStr[type]; }

MY_INLINE const c8* getGeomFormReadableName(E_DYN_OBJ_GEOM_FORM type)
{ return GeomFormShortStr[type]; }

//----------------------------------------------------------------------------

//! enumeratin for dynamic object states
enum E_DYN_OBJ_STATE
{
	EDOS_UNKNOWN = 0,
    EDOS_WALKING,
    EDOS_START_FALLING,
    EDOS_FALLING,
    EDOS_STOP_FALLING,

	//! for counting enumeration
	E_DYN_OBJ_STATE_COUNT,

	//! not used
	E_DYN_OBJ_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_DYN_TRANSF_CONSTRAINT
{
	EDTC_NO_TRANSF  = 0x00000000,
	EDTC_ROTX       = 0x00000001,
	EDTC_ROTY       = 0x00000002,
	EDTC_ROTZ       = 0x00000004,
	EDTC_ROTXYZ     = EDTC_ROTX | EDTC_ROTY | EDTC_ROTZ,
	EDTC_MOVX       = 0x00000008,
	EDTC_MOVY       = 0x00000010,
	EDTC_MOVZ       = 0x00000020,
	EDTC_MOVXYZ     = EDTC_MOVX | EDTC_MOVY | EDTC_MOVZ,
	EDTC_ALL_TRANSF = EDTC_ROTXYZ | EDTC_MOVXYZ,

	//! not used
	E_DYN_TRANSF_CONSTRAINT_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! 
static const c8* DynamicObjectCorePropertiesTagValue	= "DynamicObjectCoreProperties";

MY_INLINE const c8* getDynamicObjectCorePropertiesTagValue()
{ return DynamicObjectCorePropertiesTagValue; }

//----------------------------------------------------------------------------
} // end namespace dyn
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_dynHPP