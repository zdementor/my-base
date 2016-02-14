//|-------------------------------------------------------------------------
//| File:        IAnimatedMeshSceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IAnimatedMeshSceneNodeHPP
#define IAnimatedMeshSceneNodeHPP
//---------------------------------------------------------------------------

#include <scn/ISceneNode.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

//! Scene node, with animated mesh attached to
class __MY_SCN_LIB_API__ IAnimatedMeshSceneNode : public ISceneNode
{
public:

	IAnimatedMeshSceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id)
		: ISceneNode(type, parent, id) {}

	virtual ~IAnimatedMeshSceneNode()
	{
		clearMaterials();
	}

	virtual void clearMaterials()
	{ m_Materials.clear(); }

	virtual vid::SMaterial& addMaterial(vid::SMaterial& mat = vid::SMaterial())
	{
		m_Materials.push_back(mat);
		return m_Materials[m_Materials.size() - 1];
	}

	virtual const c8* getCurrentAnimationFileName()
	{ return getAnimationFileName(getCurrentAnimation()); }
	virtual u32 getCurrentAnimationBeginFrame()
	{ return getAnimationBeginFrame(getCurrentAnimation()); }
	virtual u32 getCurrentAnimationEndFrame()
	{ return getAnimationEndFrame(getCurrentAnimation()); }
	virtual f32 getCurrentAnimationSpeed()
	{ return getAnimationSpeed(getCurrentAnimation()); }
	virtual bool getCurrentAnimationLoopMode()
	{ return getAnimationLoopMode(getCurrentAnimation()); }

	virtual void setCurrentAnimationBeginFrame(u32 frame)
	{ setAnimationBeginFrame(getCurrentAnimation(), frame); }
	virtual void setCurrentAnimationEndFrame(u32 frame)
	{ setAnimationEndFrame(getCurrentAnimation(), frame); }
	virtual void setCurrentAnimationSpeed(f32 speed)
	{ setAnimationSpeed(getCurrentAnimation(), speed); }
	virtual void setCurrentAnimationLoopMode(bool looped)
	{ setAnimationLoopMode(getCurrentAnimation(), looped); }

	virtual const c8* getAnimationFileName(u32 idx) = 0;
	virtual u32 getAnimationBeginFrame(u32 idx) = 0;
	virtual u32 getAnimationEndFrame(u32 idx) = 0;
	virtual f32 getAnimationSpeed(u32 idx) = 0;
	virtual bool getAnimationLoopMode(u32 idx) = 0;

	virtual void setAnimationBeginFrame(u32 idx, u32 frame) = 0;
	virtual void setAnimationEndFrame(u32 idx, u32 frame) = 0;
	virtual void setAnimationSpeed(u32 idx, f32 speed) = 0;
	virtual void setAnimationLoopMode(u32 idx, bool looped) = 0;

	//! Sets the speed with witch the animation is played.
	virtual void setAnimationOveralSpeed(f32 speed) = 0;

    //! Sets the speed with witch the animation is played.
    virtual f32 getAnimationOveralSpeed() = 0;

	//! Return true if smooth animation set to true
	virtual bool isAnimationOveralSmooth() = 0;

	//! Setting on/off smooth animation flag
	virtual void setAnimationOveralSmooth(bool flag) = 0;

    //! Geting overal animations count 
    virtual u32 getAnimationOveralFramesCount() = 0;

	//! returns animated mesh of this scene node
	virtual IAnimatedMesh* getAnimatedMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;

	//! setting animated mesh of this scene node
	//! this works with scene nodes with animated meshes 
	virtual void setAnimatedMesh(IAnimatedMesh* amesh, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;

	//! return current frame of animated mesh
	virtual IMesh* getMesh(E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;

	//! Return numbered frame of animated mesh.
	//! Note, that frame can be a float value, in such case returned mesh interpolated
	//! between nearest frames
	virtual IMesh* getMesh(f32 frame, E_LOD_LEVEL lodlevel=ELL_LOD_NEAREST) = 0;

	//! Attaching another scene node to named skeleton bone, return true on success
	virtual bool attachToBone(ISceneNode* node, const c8 *bone_name) = 0;
};

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef IAnimatedMeshSceneNodeHPP
