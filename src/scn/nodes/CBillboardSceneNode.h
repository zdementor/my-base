//|-------------------------------------------------------------------------
//| File:        CBillboardSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CBillboardSceneNodeHPP
#define CBillboardSceneNodeHPP
//--------------------------------------------------------------------------

#include <scn/IBillboardSceneNode.h>
#include <vid/S3DVertex.h>
#include "../CSceneNodeAnimatorTexture.h"

//--------------------------------------------------------------------------
namespace my {
namespace scn {
//--------------------------------------------------------------------------

//! Scene node which is a billboard. A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera. 
class CBillboardSceneNode : public IBillboardSceneNode
{
public:

    CBillboardSceneNode(ISceneNode* parent, s32 id);
    virtual ~CBillboardSceneNode();

	// ISceneNode implementation

	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs);

    virtual const core::aabbox3d<f32>& getBoundingBox() const
	{ return m_BBox; }

    virtual vid::SMaterial& getMaterial(s32 i)
	{ return m_Material; }
    
    virtual s32 getMaterialsCount()
	{ return 1; }

	virtual bool set(ISceneNode* other);

	virtual const c8* getSceneCorePropertiesXMLString();

	virtual u32 getAnimationsCount()
	{ return m_Animators.size(); }

    virtual bool setCurrentAnimation(u32 anim, f32 delay=0.3f);

    virtual u32 getCurrentAnimation()
	{ return m_CurrentAnimation; }

	virtual bool setCurrentAnimationProgress(f32 progress);

	virtual f32 getCurrentAnimationProgress();

    virtual void setAnimationCallback(IAnimationEndCallBack* callback=0) {}

	// IBillboardSceneNode implementation

    virtual void setSize(const core::dimension2d<f32>& size);

	virtual const core::dimension2d<f32>& getSize()
	{ return m_Params.Size; }

	virtual void setColor(const img::SColor& color);

	virtual const SBillboardParams & getBillboardParameters()
	{ return m_Params; }

	virtual void setBillboardParameters(const SBillboardParams &bill_params);

	virtual void setAnimationDirectionSetFrames( 
		u32 animation_idx, u32 direction_idx,
		const core::array<SAnimationFrame> &frames,
		const SAnimationFrameParameters &params);

	virtual const core::array<SAnimationFrame>& getAnimationDirectionSetFrames(
		u32 animation_idx, u32 direction_idx);

	virtual const SAnimationFrame& getAnimationDirectionSetFrame(
		u32 animation_idx, u32 direction_idx, u32 frame_idx);

	virtual const SAnimationFrameParameters& getAnimationFramesParameters(u32 animation_idx);

	virtual u32 getAnimationDirectionSetsCount(u32 animation_idx);

	virtual u32 getAnimationDirectionSetFramesCount(u32 animation_idx, u32 direction_idx);

	virtual void setAnimationFramesDirectionOrderCCW(bool value)
	{ m_AnimationTexturesDirectionOrderCCW = value; }

	virtual void setRectColor(
		img::SColor &c1, img::SColor &c2, img::SColor &c3, img::SColor &c4);

	virtual vid::ITexture* getCurrentAnimationTextureForCameraPosition(
		const core::vector3df &cam_pos );

	virtual void getCornersCoordinatesForView(
		core::vector3df  view_pos, core::vector3df view_tgt,
		core::vector3df& right_down_corner,
		core::vector3df& right_up_corner,
		core::vector3df& left_up_corner,
		core::vector3df& left_down_corner);

    virtual void setMaterial(vid::SMaterial& mat);

	virtual const core::vector3df & getNormal()
	{ return m_Normal; }

	virtual void setTCoordsRect ( const core::rectf &tc )
	{
		m_Vertices[0].TCoords.set(tc.Right, tc.Bottom);
		m_Vertices[1].TCoords.set(tc.Right, tc.Top   );
		m_Vertices[2].TCoords.set(tc.Left,  tc.Top   );
		m_Vertices[3].TCoords.set(tc.Left,  tc.Bottom);
	}

	virtual const core::vector2df& getTCoords0 () { return m_Vertices[0].TCoords; }
	virtual const core::vector2df& getTCoords1 () { return m_Vertices[1].TCoords; }
	virtual const core::vector2df& getTCoords2 () { return m_Vertices[2].TCoords; }
	virtual const core::vector2df& getTCoords3 () { return m_Vertices[3].TCoords; }

private:

	struct SBillTexName
	{
		SBillTexName() : tex(0) {}
		SBillTexName(const c8 * f, vid::ITexture * t)
			: fname(f), tex(t) {}
		bool operator < (const SBillTexName & other) const
		{ return fname < other.fname; }
		bool operator == (const vid::ITexture * other) const
		{ return tex == other; }
		core::stringc fname;
		vid::ITexture * tex;
	};

	//! animates a scene node
    void _animate(core::vector3df cam_pos);

    core::aabbox3d<f32> m_BBox;
    vid::SMaterial m_Material;

	vid::S3DVertex1TCoordsColoured m_Vertices[4];
	static u16 m_Indices[6];

	SBillboardParams m_Params;

	bool m_AnimationTexturesDirectionOrderCCW;
	
	core::array<CSceneNodeAnimatorTexture*> m_Animators;
	
	u32  m_StartTime;

	u32 m_CurrentAnimation;

	SAnimationFrameParameters m_EmptyTextureAnimatorParameters;
	core::array<SAnimationFrame> m_EmptyTextures;
	SAnimationFrame m_EmptyFrame;

	core::vector3df m_Normal;

	s32 m_ReinitVertsNum;

	core::vector2df m_FrameOffset, m_FrameScale;
	bool m_GL;
};

//--------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//--------------------------------------------------------------------------

#endif

