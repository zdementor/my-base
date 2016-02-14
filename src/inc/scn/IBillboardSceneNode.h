//|-------------------------------------------------------------------------
//| File:        IBillboardSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IBillboardSceneNodeHPP
#define IBillboardSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

struct SBillboardParams
{
	SBillboardParams() 
		: Size(100.0f, 100.0f), FrameResolution(0,0),
		UpAxis(0,1,0), AxisAligmentType(EAAT_NONE), 
		AlignAxis(0,1,0), FlipWidth(false), FlipHeight(false) {}

	void set(const SBillboardParams &other)
	{
		Size			= other.Size;
		FrameResolution = other.FrameResolution;
		AxisAligmentType= other.AxisAligmentType;
		AlignAxis		= other.AlignAxis;
		UpAxis			= other.UpAxis;
		FlipWidth		= other.FlipWidth;
		FlipHeight		= other.FlipHeight;
	}

	core::dimension2df Size;
	core::dimension2di FrameResolution;

	E_AXIS_ALIGMENT_TYPE AxisAligmentType;

	core::vector3df AlignAxis;
	core::vector3df UpAxis;

	bool FlipWidth, FlipHeight;
};

//---------------------------------------------------------------------------

//! A billboard scene node.
//! A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera. It is usually used for explosions, fire,
//! lensflares, particles and things like that.
class __MY_SCN_LIB_API__ IBillboardSceneNode : public ISceneNode
{
public:

    //! constructor
    IBillboardSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id) 
		: ISceneNode(type, parent, id) {}

    //! Sets the size of the billboard.
    virtual void setSize(const core::dimension2df &size) = 0;

    //! Returns the size of the billboard.
    virtual const core::dimension2df& getSize() = 0;

	//! Sets the color of the billboard.	
	virtual void setColor(const img::SColor& color) = 0;

	//! return billboard parameters
	virtual const SBillboardParams& getBillboardParameters() = 0;

	//! setting billboard parameters
	virtual void setBillboardParameters(const SBillboardParams &bill_params) = 0;
	
	//! setting animation textures for each animation/direction
	virtual void setAnimationDirectionSetFrames( 
		u32 animation_idx, u32 direction_idx,
		const core::array<SAnimationFrame> &frames,
		const SAnimationFrameParameters &params) = 0;

	//! getting animation textures direction set for each type of animation
	virtual const core::array<SAnimationFrame>& getAnimationDirectionSetFrames(
		u32 animation_idx, u32 direction_idx) = 0;
	virtual const SAnimationFrame& getAnimationDirectionSetFrame(
		u32 animation_idx, u32 direction_idx, u32 frame_idx) = 0;

	//! getting animation textures direction set parameters
	virtual const SAnimationFrameParameters& getAnimationFramesParameters(u32 animation_idx) = 0;

	//! getting textures direction sets count for each type of animation
	virtual u32 getAnimationDirectionSetsCount(u32 animation_idx) = 0;

	//! getting frames count for animation/direction
	virtual u32 getAnimationDirectionSetFramesCount(u32 animation_idx, u32 direction_idx) = 0;

	//! setting direction order for choosing needed animated textures set,
	//! according to the current camera position
	virtual void setAnimationFramesDirectionOrderCCW(bool value) = 0;

	virtual void setRectColor(
		img::SColor &c1, img::SColor &c2, img::SColor &c3, img::SColor &c4) = 0;

	virtual vid::ITexture* getCurrentAnimationTextureForCameraPosition(
		const core::vector3df &cam_pos) = 0;

	virtual void getCornersCoordinatesForView(
		core::vector3df  view_pos, core::vector3df view_tgt,
		core::vector3df& right_down_corner,
		core::vector3df& right_up_corner,
		core::vector3df& left_up_corner,
		core::vector3df& left_down_corner) = 0;

	//!
	virtual const core::vector3df& getNormal() = 0;

	//!
	virtual void setTCoordsRect(const core::rectf &tc) = 0;

	//!
	virtual const core::vector2df& getTCoords0() = 0;
	virtual const core::vector2df& getTCoords1() = 0;
	virtual const core::vector2df& getTCoords2() = 0;
	virtual const core::vector2df& getTCoords3() = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

