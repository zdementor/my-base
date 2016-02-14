//|-------------------------------------------------------------------------
//| File:        IAnimatedMesh.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IAnimatedMeshHPP
#define IAnimatedMeshHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <scn/IMesh.h>
#include <scn/IAnimatedMeshSceneNode.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------	

class IAnimatedMesh;

//----------------------------------------------------------------------------	

//! Skeleton bones
struct SSkeletonBone
{
	SSkeletonBone ()
	{
		ParentBoneId = -1;
	}

	core::vector3df  Translation;
	core::quaternion Rotation;
	core::stringc    Name;
	core::line3df    Line;
	core::aabbox3df  BBox;

	s32 ParentBoneId;
};

//----------------------------------------------------------------------------	

//! State of animation
struct SAnimationState
{
	s32 Animation;
	f32 AnimationTime, AnimationTimeSec, AnimationDurationSec;
	bool Looped;	

	core::array<SSkeletonBone> Bones;	

	SAnimationState() 
		: Animation(-1), AnimationTime(0.0f), 
		AnimationTimeSec(0.0f), AnimationDurationSec(0.0f), Looped(false) {}
};

//----------------------------------------------------------------------------

struct SAdjacencyData
{
	SAdjacencyData(): Adjacency(NULL), IndexCount(0) {}

	~SAdjacencyData() { SAFE_DELETE_ARRAY(Adjacency); IndexCount=0; }

	u16* Adjacency;
	s32 IndexCount;
};

//----------------------------------------------------------------------------

//! Callback interface for catching events of ended animations.
class __MY_SCN_LIB_API__ IMeshAnimationCallback : public IUnknown
{
public:

    //! Will be called when the animation playback has started.
	virtual void OnAnimationBegin(u32 animation_idx) = 0;

    //! Will be called when the animation playback has ended.
	virtual void OnAnimationEnd(u32 animation_idx) = 0;
};

//----------------------------------------------------------------------------

class ISpecialAnimation
{
public:
	virtual ~ISpecialAnimation() {}
};

//----------------------------------------------------------------------------

//! Interface for an animated mesh.
class __MY_SCN_LIB_API__ IAnimatedMesh : public IUnknown
{
public:

	//! constructor
	IAnimatedMesh() 
		: m_MeshAnimationCallback(0), m_OveralSpeed(1.0f), 
	m_CurrentAnimationSpeed(150.0f), m_CurrentAnimationIdx(-1),
	m_OveralSmooth(true), m_InterpolateAnimation(0) {}	

	struct SAnimation
	{
		SAnimation(u32 start, u32 end, f32 speed, bool looped)
			: FileName(""), Speed(speed), Looping(looped),
			BeginFrame(start), EndFrame(end), SpecialAnimation(NULL) {}
		SAnimation(const c8 *filename, f32 speed, bool looped)
			: FileName(filename), Speed(speed), Looping(looped),
			BeginFrame(0), EndFrame(0), SpecialAnimation(NULL) {}
		~SAnimation() { SAFE_DELETE(SpecialAnimation); }
		core::stringc		FileName;
		f32					Speed;
		bool				Looping;
		u32					BeginFrame, EndFrame;
		ISpecialAnimation	*SpecialAnimation;
	};

    //! destructor
    virtual ~IAnimatedMesh()
	{
		u32 acnt = m_Animations.size();
		for (u32 i = 0; i < acnt; i++)
			delete m_Animations[i];
		m_Animations.clear();
	}
   
    //! Returns the IMesh interface according to the current animation
    virtual IMesh* getMesh() { return NULL; }

	//! Returns the IMesh interface according to the specified key frame
	virtual IMesh* getMesh(f32 frame) { return NULL; }

    //! Returns an axis aligned bounding box of the mesh.
    //! \return A bounding box of this mesh is returned.
    virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return m_BoundingBox;
	}

	//! calc bounding box
    virtual void recalculateBoundingBox() = 0;

    //! Returns the type of the animated mesh.
    //! In most cases it is not neccessary to use this method.
    //! This is useful for making a save downcast, for example
    //! if getMeshType() returns EMT_MD2, its save to cast the
    //! IAnimatedMesh to IAnimatedMeshMD2.
    //! \returns Type of the mesh.
    virtual E_MESH_TYPE getAnimatedMeshType() const
    { return EMT_UNKNOWN; }

	//! Gets the frame count of the animated mesh.
    //! \return Returns the amount of frames. If the amount is 1, it is a static, non animated mesh.
    virtual u32 getOveralFramesCount() = 0;

    //! Geting animations count 
    virtual u32 getAnimationFramesCount(u32 idx)
	{
		if (idx >= m_Animations.size())
			return 0;
		s32 frames = m_Animations[idx]->EndFrame - m_Animations[idx]->BeginFrame + 1;
		return frames > 0 ? frames : 0;
	}

	//! Geting animations count 
    virtual u32 getAnimationsCount()
	{ return m_Animations.size(); }

	//! rescale normals
	virtual void rescaleAnimatedMeshNormals(const core::vector3df &newScale) = 0;

	//! setting animation
	virtual bool setCurrentAnimation(u32 idx, f32 delay=0.0f) { return false; }

	//! getting animation index
	virtual u32 getCurrentAnimationIndex() 
	{ return (u32)m_CurrentAnimationIdx; }

	//! must be called bebore registerig new animations
	virtual void startRegisteringAnimations() {}

	//! Registering new animation as frames cycle
	//! (must be called between start/end RegisteringAnimations)
	virtual bool registerAnimation(u32 start_frame, u32 end_frame, f32 speed, bool looped)
	{
		m_Animations.push_back(new SAnimation(start_frame, end_frame, speed, looped));
		return true;
	}

	//! Registering new animation which stored in file
	//! (must be called between start/end RegisteringAnimations)
	virtual bool registerAnimation(const c8 *filename, f32 speed, bool looped)
	{
		m_Animations.push_back(new SAnimation(filename, speed, looped));
		return true;
	}

	//! Unregistering/removing animation from this mesh
	//! (must be called between start/end RegisteringAnimations)
	virtual bool unregisterAnimation(u32 idx)
	{
		if (idx >= m_Animations.size())
			return false;
		delete m_Animations[idx];
		m_Animations.erase(idx);
		if ((s32)idx <= m_CurrentAnimationIdx)
		{
			m_CurrentAnimationIdx--;
			if (m_CurrentAnimationIdx < 0)
			{
				m_CurrentAnimationIdx = -1;
				if (m_Animations.size())
					setCurrentAnimation(0);
			}
			else
				setCurrentAnimation(m_CurrentAnimationIdx);
		}
		return true;
	}

	//! Swap animations
	//! (must be called between start/end RegisteringAnimations)
	virtual bool swapAnimations(u32 idx1, u32 idx2)
	{
		if (idx1 >= m_Animations.size() || idx2 >= m_Animations.size())
			return false;
		SAnimation *tmp = m_Animations[idx1];
		m_Animations[idx1] = m_Animations[idx2];
		m_Animations[idx2] = tmp;
		return true;
	}

	//! Calling after registerig new animations
	virtual void endRegisteringAnimations() {}

	//! return special animation by its index
	virtual const c8* getAnimationFileName(u32 idx)
	{
		if (idx >= m_Animations.size()
				|| !m_Animations[idx]->FileName.size())
			return NULL;
		return m_Animations[idx]->FileName.c_str();
	}
	virtual u32 getAnimationBeginFrame(u32 idx)
	{
		if (idx >= m_Animations.size())
			return 0;
		return m_Animations[idx]->BeginFrame;
	}
	virtual u32 getAnimationEndFrame(u32 idx)
	{
		if (idx >= m_Animations.size())
			return 0;
		return m_Animations[idx]->EndFrame;
	}
	virtual f32 getAnimationSpeed(u32 idx)
	{
		if (idx >= m_Animations.size())
			return 1.0f;
		return m_Animations[idx]->Speed;
	}
	virtual bool getAnimationLoopMode(u32 idx)
	{
		if (idx >= m_Animations.size())
			return false;
		return m_Animations[idx]->Looping;
	}

	//! set special animation parameters
	virtual void setAnimationBeginFrame(u32 idx, u32 frame)
	{
		if (idx >= m_Animations.size())
			return;
		m_Animations[idx]->BeginFrame = frame;
	}
	virtual void setAnimationEndFrame(u32 idx, u32 frame)
	{
		if (idx >= m_Animations.size())
			return;
		m_Animations[idx]->EndFrame = frame;
	}
	virtual void setAnimationSpeed(u32 idx, f32 speed)
	{
		if (idx >= m_Animations.size())
			return;
		m_Animations[idx]->Speed = speed;
		if (m_CurrentAnimationIdx == idx)
			m_CurrentAnimationSpeed = speed;
	}
	virtual void setAnimationLoopMode(u32 idx, bool looped)
	{
		if (idx >= m_Animations.size())
			return;
		m_Animations[idx]->Looping = looped;
	}

    //! Sets the speed with wich all animations are played.
    virtual void setAnimationOveralSpeed(f32 speed)
	{ m_OveralSpeed = speed; }

    //! gets the speed with wich all animations are played.
    virtual f32 getAnimationOveralSpeed()
	{ return m_OveralSpeed; }

	//! Return true if smooth animation set to true
	virtual bool isAnimationOveralSmooth()
	{ return m_OveralSmooth; }

	//! Setting on/off smooth animation flag
	virtual void setAnimationOveralSmooth(bool flag)
	{ m_OveralSmooth = flag; }

	//! setting animation end callback
	virtual void setMeshAnimationCallback(IMeshAnimationCallback *callback)
	{
		SAFE_DROP(m_MeshAnimationCallback);
		m_MeshAnimationCallback = callback;
		SAFE_GRAB(m_MeshAnimationCallback);
	}

	//! runnimg animation end callback
	virtual void OnAnimationBegin(u32 animation_idx)
	{
		if (m_MeshAnimationCallback)
			m_MeshAnimationCallback->OnAnimationBegin(animation_idx);
	}

	//! runnimg animation end callback
	virtual void OnAnimationEnd(u32 animation_idx)
	{
		if (m_MeshAnimationCallback)
			m_MeshAnimationCallback->OnAnimationEnd(animation_idx);
	}

	//! Returns the animation time in seconds.
	virtual f32 getAnimationTimeSec() { return 0.0f; }

	//! Returns the animation duration in seconds.
	virtual f32 getAnimationDurationSec() { return 0.0f; } 

	//! Getting animation fps
	virtual s32 getAnimationFPS() { return 0; }

	//! Setting animation fps
	virtual void setAnimationFPS(s32 fps) {}

	//! calculating adjacency info for this mesh
	virtual bool calculateAdjacency()
	{
		return scn::calculateAdjacency(this);
	}

	//! returning afjacency data of this mesh
	SAdjacencyData& getAdjacencyData()
	{
		return m_AdjacencyData;
	}

	//! returning current animation state
	virtual bool getAnimationState(SAnimationState& ani_state)
	{
		return false;
	}

	//! updating mesh animation
	virtual void updateMesh(s32 delta_ms) {}

	//! setting current animation state
	virtual bool setAnimationState(const SAnimationState &animstate)
	{
		return false;
	}

	//! Getting acces to bones
	virtual const core::array<SSkeletonBone>& getBones()
	{
		return *((core::array<SSkeletonBone>*)NULL);
	}

	//!
	virtual void clearGeometry()
	{
		s32 fr_cnt = getOveralFramesCount();

		for (s32 fr=0; fr<fr_cnt; fr++)
		{
			IMesh *mesh = getMesh(fr);
			if (mesh)
				mesh->clearMeshGeometry();					
		}
	}

	virtual bool isInterpolateAnimation()
	{
		return m_InterpolateAnimation;
	}

	virtual bool mapBone(s32 bone_idx)
	{
		return false;
	}

	virtual core::matrix4 getMapedBoneTransformation(s32 bone_idx)
	{
		return core::matrix4();
	}

	virtual void uploadToRenderBuffers(IAnimatedMeshSceneNode *node)
	{
		IMesh *mesh = getMesh();
		if (mesh)
			mesh->uploadToRenderBuffers(node);
	}

	virtual bool calculateTangents() { return false; }

	//! must be called before modifying any animations
	static void startRegisteringAnimations(scn::IAnimatedMeshSceneNode *node)
	{
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				amesh->startRegisteringAnimations();
		}
	}

	//! Registering new animation as frames cycle
	//! (must be called between start/end RegisteringAnimations)
	static bool registerAnimation(scn::IAnimatedMeshSceneNode *node,
		u32 start_frame, u32 end_frame, f32 speed, bool looped)
	{
		bool res = true;
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				res = res && amesh->registerAnimation(start_frame, end_frame, speed, looped);
		}
		return res;
	}

	//! Registering new animation which stored in file
	//! (must be called between start/end RegisteringAnimations)
	static bool registerAnimation(scn::IAnimatedMeshSceneNode *node,
		const c8 *filename, f32 speed, bool looped)
	{
		bool res = true;
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				res = res && amesh->registerAnimation(filename, speed, looped);
		}
		return res;
	}

	//! Unregistering/removing animation from this mesh
	//! (must be called between start/end RegisteringAnimations)
	static bool unregisterAnimation(scn::IAnimatedMeshSceneNode *node,
		u32 idx)
	{
		bool res = true;
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				res = res && amesh->unregisterAnimation(idx);
		}
		return res;
	}

	//! Swap animations
	//! (must be called between start/end RegisteringAnimations)
	static bool swapAnimations(scn::IAnimatedMeshSceneNode *node,
		u32 idx1, u32 idx2)
	{
		bool res = true;
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				res = res && amesh->swapAnimations(idx1, idx2);
		}
		return res;
	}

	//! must be called after modifying any animations
	static void endRegisteringAnimations(scn::IAnimatedMeshSceneNode *node)
	{
		for (u32 i = 0; i < scn::E_LOD_LEVEL_COUNT; i++)
		{
			scn::IAnimatedMesh *amesh = node->getAnimatedMesh((scn::E_LOD_LEVEL)i);
			if (amesh)
				amesh->endRegisteringAnimations();
		}
	}

protected:

	IMeshAnimationCallback *m_MeshAnimationCallback;

	f32 m_OveralSpeed, m_CurrentAnimationSpeed;
	s32 m_CurrentAnimationIdx;

	bool m_OveralSmooth;    
	bool m_InterpolateAnimation;

    core::aabbox3d<f32> m_BoundingBox;

	SAdjacencyData m_AdjacencyData;

	core::array <SAnimation*> m_Animations;
};

//----------------------------------------------------------------------------
} // end namespace scene
} // end namespace my
//----------------------------------------------------------------------------
#endif // #ifndef IAnimatedMeshHPP

