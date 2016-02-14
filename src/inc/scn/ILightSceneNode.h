//|-------------------------------------------------------------------------
//| File:        ILightSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ILightSceneNodeHPP
#define ILightSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <vid/SLight.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Scene node which is a dynamic light. 
//! You can switch the light on and off by 
//! making it visible or not, and let it be animated by ordinary scene node animators.
class __MY_SCN_LIB_API__ ILightSceneNode : public ISceneNode
{
public:

    //! constructor
    ILightSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id)
        : ISceneNode(type, parent, id) {}

    //! \return Returns the light data.
    virtual const vid::SLight& getLightParameters() const = 0;

    //! Set the light data.
    virtual void setLightParameters(const vid::SLight &lparams) = 0;

	virtual const core::vector3df& getLightedVolumeCenterOffset() = 0;
	virtual void setLightedVolumeCenterOffset(const core::vector3df& offset) = 0;

	virtual const core::vector3df& getLightedVolumeSizeChange() = 0;
	virtual void setLightedVolumeSizeChange(const core::vector3df& change) = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif

