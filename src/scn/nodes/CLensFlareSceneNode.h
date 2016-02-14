//|-------------------------------------------------------------------------
//| File:        CLensFlareSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CLensFlareSceneNodeHPP
#define CLensFlareSceneNodeHPP
//--------------------------------------------------------------------------------

#include <scn/ILensFlareSceneNode.h>
#include <vid/S3DVertex.h>

//--------------------------------------------------------------------------------
namespace my{
namespace scn{
//--------------------------------------------------------------------------------

class CLensFlareSceneNode : public ILensFlareSceneNode
{

public:

    CLensFlareSceneNode(ISceneNode* parent, s32 id=-1);
    virtual ~CLensFlareSceneNode();  
 
	const core::aabbox3d<f32>& CLensFlareSceneNode::getBoundingBox() const
	{ return m_Box; }

	virtual void OnBeforeCulling();
	virtual void OnCheckCulling();
    virtual void OnPreRender(u32 timeMs); 

	virtual u32 getLensCount()
	{ return m_Lens.size(); }

	virtual bool setLens(u32 idx, const SLens &lens);
	virtual bool setLens(u32 idx, vid::ITexture *tex, f32 space);

	virtual void addLens(const SLens &lens)
	{ m_Lens.push_back(lens); }

	virtual void addLens(vid::ITexture *tex, f32 space)
	{ m_Lens.push_back(SLens(tex, space)); }

	virtual const SLens& getLens(u32 idx) const;

	virtual void removeLens(u32 idx)
	{ m_Lens.erase(idx); }

	virtual void clearLens()
	{ m_Lens.clear(); }

	virtual void setCorona(vid::ITexture *t);

	virtual vid::ITexture* getCorona() const
	{ return m_Corona; }

	virtual void setColor(const img::SColor &color)
	{ m_Material.getPass(0).setDiffuseColor(color); }

	virtual const img::SColor& getColor()
	{ return m_Material.getPass(0).getDiffuseColor(); }

	virtual void setEtalonScreenSize(const core::dimension2di &scrSize);
	virtual void setEtalonScreenSize(u32 scrWidth, u32 scrHeight);

	virtual const core::dimension2d<s32>& getEtalonScreenSize() const
	{ return m_EtalonScreenSize; }

	virtual const core::position2d<s32>& getScreenPosition() const
	{ return m_ScreenPosition; }

	virtual bool set(ISceneNode* other);

	virtual const c8* getSceneCorePropertiesXMLString();

private:   

	enum E_LENS_VISIBILITY_STATUS
	{
		ELVS_INVISIBLE = 0,
		ELVS_APPEARING,
		ELVS_VISIBLE,
		ELVS_DISAPPEARING,

		E_LENS_VISIBILITY_STATUS,

		E_LENS_VISIBILITY_STATUS_COUNT_FORCE_32_BIT = 0xffffffff
	} 
	m_VisibilityStatus;

	f32 m_Visibility;
	
	vid::ITexture* m_Corona;
	core::array<SLens> m_Lens;

	core::dimension2di m_EtalonScreenSize;

    vid::SMaterial &m_Material;

    core::aabbox3df m_Box;
  
    void _registerLensForRendering(vid::ITexture *tex,
		const core::position2df &drawPos, const core::dimension2df &scale);

	core::position2d<s32> m_ScreenPosition;
	f32 m_ScreenWidth, m_ScreenHeight;
	core::dimension2d<f32> m_Scale2D;

	core::position2d<f32> m_StartPos; 
	core::vector3df m_PosNode;

	static core::queue<CLensFlareSceneNode*> m_LensCullQueue;
	static bool m_VisibilityCalc;

	bool m_CurrentCullStatus;

	img::SColor m_CurrentLensColor;

	s32 m_PrevTimeMs;
};

//--------------------------------------------------------------------------------
} // namespace scn
} // namespace my
//--------------------------------------------------------------------------------

#endif


