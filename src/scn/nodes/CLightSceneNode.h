//|-------------------------------------------------------------------------
//| File:        CLightSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CLightSceneNodeHPP
#define CLightSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ILightSceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Scene node which is a dynamic light. You can switch the light on and off by 
//! making it visible or not, and let it be animated by ordinary scene node animators.
class CLightSceneNode : public ILightSceneNode
{
	friend class CSceneManager;

public:

    CLightSceneNode(
        ISceneNode* parent, s32 id, vid::SLight lightData);
    virtual ~CLightSceneNode();

	virtual void OnCheckCulling();

    virtual void OnPreRender(u32 timeMs);

    virtual void updateAbsoluteTransformation();

    virtual const vid::SLight& getLightParameters() const;
    virtual void setLightParameters(const vid::SLight &lparams);

	virtual const core::vector3df& getLightedVolumeCenterOffset();
	virtual void setLightedVolumeCenterOffset(const core::vector3df& offset);

	virtual const core::vector3df& getLightedVolumeSizeChange();
	virtual void setLightedVolumeSizeChange(const core::vector3df& change);

    virtual const core::aabbox3d<f32>& getBoundingBox() const;

	virtual bool set(ISceneNode* other);

	virtual const c8* getSceneCorePropertiesXMLString();

private:

	//! updating light data
	void _updateLightData();

	vid::SRenderPass m_RenderPass;

	core::vector3df m_LightedVolumeCenterOffset, m_LightedVolumeSizeChange;

    vid::SLight LightData, CurrentLightData;
    core::aabbox3d<f32> BBox;

	static IMesh* LightSphereMesh;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef CLightSceneNodeHPP

