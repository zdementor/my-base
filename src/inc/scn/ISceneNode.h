//|-------------------------------------------------------------------------
//| File:        ISceneNode.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef ISceneNodeHPP
#define ISceneNodeHPP
//---------------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <scn/ISceneNodeAnimator.h>
#include <vid/IVideoDriver.h>
#include <vid/SMaterial.h>
#include <vid/SLight.h>
#include <vid/IRenderBuffer.h>

//----------------------------------------------------------------------------

//! some helper macro

#define SAFE_CAST_TO_BILLBOARD_SCENE_NODE(scene_node) (                        \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_BILLBOARD_SCENE_NODE         \
		)                                                                      \
	) ? static_cast<scn::IBillboardSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_ANIMATED_MESH_SCENE_NODE(scene_node) (                    \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_ANIMATED_MESH_SCENE_NODE ||  \
		scene_node->getSceneNodeType()==scn::ESNT_KTREE_SCENE_NODE             \
		)                                                                      \
	) ? static_cast<scn::IAnimatedMeshSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_PARTICLE_SYSTEM_SCENE_NODE(scene_node) (                  \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_PARTICLE_SYSTEM_SCENE_NODE   \
		)                                                                      \
	) ? static_cast<scn::IParticleSystemSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_LENS_FLARE_SCENE_NODE(scene_node) (                       \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_LENS_FLARE_SCENE_NODE        \
		)                                                                      \
	) ? static_cast<scn::ILensFlareSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_TEXT_SCENE_NODE(scene_node) (                             \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_TEXT_SCENE_NODE              \
		)                                                                      \
	) ? static_cast<scn::ITextSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_KTREE_SCENE_NODE(scene_node) (                            \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_KTREE_SCENE_NODE             \
		)                                                                      \
	) ? static_cast<scn::IKTreeSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_SKY_BOX_SCENE_NODE(scene_node) (                          \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_SKY_BOX_SCENE_NODE           \
		)                                                                      \
	) ? static_cast<scn::ISkyBoxSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_TERRAIN_SCENE_NODE(scene_node) (                          \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_TERRAIN_SCENE_NODE           \
		)                                                                      \
	) ? static_cast<scn::ITerrainSceneNode*>(scene_node) : NULL

#define SAFE_CAST_TO_LIGHT_SCENE_NODE(scene_node) (                            \
    scene_node && (                                                            \
		scene_node->getSceneNodeType()==scn::ESNT_LIGHT_SCENE_NODE             \
		)                                                                      \
	) ? static_cast<scn::ILightSceneNode*>(scene_node) : NULL

//----------------------------------------------------------------------------
namespace my {
//----------------------------------------------------------------------------

namespace dyn {
class CDynamicObject;     
class CDynamicManager;
} // end namespace dyn

//----------------------------------------------------------------------------

namespace game {
class CGameNode;
} // end namespace game

//---------------------------------------------------------------------------------
namespace scn {
//---------------------------------------------------------------------------------

struct SActionQueueEntry
{
	SActionQueueEntry() : Node(0), ActionTimeMs(0) {}
	SActionQueueEntry(ISceneNode* node, s32 action_time_ms) 
		: Node(node), ActionTimeMs(action_time_ms) {}

	ISceneNode* Node;
	s32 ActionTimeMs;
};

//---------------------------------------------------------------------------------

struct SSceneNodeParameters
{
	SSceneNodeParameters()
	{
		defaultIinit();
	}

	SSceneNodeParameters(const SSceneNodeParameters& p)
    {
		defaultIinit();
        set(p);            
    }

    SSceneNodeParameters& operator = (const SSceneNodeParameters& p)
    {
		set(p);
		return *this;
    } 

	void set(const SSceneNodeParameters& p) 
	{
		Name		= p.Name;			
		ID			= p.ID;
		ParentID	= p.ParentID;
		Type		= p.Type;
	}

	bool operator != (const SSceneNodeParameters& p)
	{
		return Name!= p.Name || 
			ID != p.ID || ParentID != p.ParentID ||
			Type != p.Type;
	}

	bool operator == (const SSceneNodeParameters& p)
	{ return !((*this) != p); }

	void defaultIinit()
	{
		Name		= NONAME_SCENENODE_NAME;
		ID			= -1;
		ParentID	= -1;
		Type		= ESNT_UNKNOWN_SCENE_NODE;
	}

	core::stringc Name;
	s32 ID;
	s32 ParentID;
	E_SCENE_NODE_TYPE Type;
};

//---------------------------------------------------------------------------------

struct SSceneNodeMaterial
{
	vid::SMaterial Material;

	SSceneNodeMaterial() {}

	void set(const SSceneNodeMaterial &p)
	{
		Material	= p.Material;
	}

	SSceneNodeMaterial& operator = (const SSceneNodeMaterial &p)
    {
		set(p);
		return *this;
    }

	bool operator != (const SSceneNodeMaterial& m)
	{ return Material != m.Material; }

	bool operator == (const SSceneNodeMaterial& m)
	{ return !((*this)!=m); }
};

//---------------------------------------------------------------------------------

struct SSceneNodeTransformation
{
	core::vector3df Position;
	core::vector3df Rotation;
	core::vector3df Scale;

	SSceneNodeTransformation()
	{
		Position = core::vector3df(0.0f, 0.0f, 0.0f);
		Rotation = core::vector3df(0.0f, 0.0f, 0.0f);
		Scale    = core::vector3df(1.0f, 1.0f, 1.0f);
	}

	void set(const SSceneNodeTransformation &p)
	{
		Position	= p.Position;
		Rotation	= p.Rotation;
		Scale		= p.Scale;	
	}

	SSceneNodeTransformation& operator = (const SSceneNodeTransformation &p)
    {
		set(p);
		return *this;
    }

	bool operator != (const SSceneNodeTransformation& t)
	{
		return 
			Position != t.Position || Rotation != t.Rotation ||
			Scale != t.Scale;
	}

	bool operator == (const SSceneNodeTransformation& t)
	{ return !((*this)!=t); }
};

//---------------------------------------------------------------------------------

struct SSceneNodeCommonParams
{
	SSceneNodeCommonParams() {}
	SSceneNodeCommonParams(const SSceneNodeCommonParams& p) { *this = p; }

	SSceneNodeParameters		Parameters;		
	SSceneNodeMaterial			Material;
	SSceneNodeTransformation	Transformation;

	void set(const SSceneNodeCommonParams &p)
	{
		Parameters		= p.Parameters;
		Material		= p.Material;
		Transformation	= p.Transformation;	
	}

	SSceneNodeCommonParams& operator = (const SSceneNodeCommonParams &p)
    {
		set(p);
		return *this;
    } 

	bool operator != (const SSceneNodeCommonParams& p)
	{
		return 
			Parameters != p.Parameters || Material != p.Material ||	
			Transformation != p.Transformation;	
	}

	bool operator == (const SSceneNodeCommonParams& p)
	{ return !((*this)!=p); }
};

//----------------------------------------------------------------------------	

struct SImposterParameters
{
private:
	core::stringc FileName;
public:
	SImposterParameters() : 
		ViewOffset(0), ShiftCustom(0,0,0), ShiftBBCenter(0,0,0), Centered(false) {}

	core::vector3df ShiftCustom, ShiftBBCenter;
	f32 ViewOffset;
	bool Centered;

	const c8* getFileName() const
	{ return FileName.size() > 0 ? FileName.c_str() : NULL; }

	void setFileName(const c8 *file_name)
	{ FileName = file_name; }

	void set(const SImposterParameters &p) 
	{
		ShiftCustom   = p.ShiftCustom;
		ShiftBBCenter = p.ShiftBBCenter;
		ViewOffset    = p.ViewOffset;
		Centered      = p.Centered;
		FileName      = p.FileName;
	}

	SImposterParameters& operator = (const SImposterParameters &p)
    {
		set(p);
		return *this;
    }

	bool equal(const SImposterParameters &p) const
	{ return (*this) == p; }

	bool operator == (const SImposterParameters &p) const
    {
		return FileName      == p.FileName
			&& ShiftCustom   == p.ShiftCustom
			&& ShiftBBCenter == p.ShiftBBCenter
			&& ViewOffset    == p.ViewOffset
			&& Centered      == p.Centered;
    }

	bool operator != (const SImposterParameters &p) const
	{ return !(this->operator == (p)); }
};

//----------------------------------------------------------------------------	

struct SImposterCreationParameters
{
	SImposterCreationParameters()
		: ViewsCount(8),
		CameraViewScale(core::vector2df(1.0f,1.0f)), 
		SizeScale(core::vector2df(1.0f,1.0f)),
		TextureResolution(64), AnimationFrameStep(2.0f), OptimalViewAngle(25.0f),
		AmbientColor(img::SColor(255,150,150,150)), 
		DiffuseColor(img::SColor(255,255,255,255)), 
		SpecularColor(img::SColor(255,255,255,255)),
		Compress(true)
		{}

	s32 ViewsCount;
	core::vector2df CameraViewScale;
	core::vector2df SizeScale;
	f32 TextureResolution;
	f32 AnimationFrameStep;	
	f32 OptimalViewAngle;
	img::SColor AmbientColor, DiffuseColor, SpecularColor;
	bool Compress;

	void set(const SImposterCreationParameters &p) 
	{
		ViewsCount			= p.ViewsCount;
		CameraViewScale		= p.CameraViewScale;	
		SizeScale			= p.SizeScale;	
		TextureResolution	= p.TextureResolution;	
		AnimationFrameStep	= p.AnimationFrameStep;
		OptimalViewAngle	= p.OptimalViewAngle;	
		AmbientColor		= p.AmbientColor;
		DiffuseColor		= p.DiffuseColor;	
		SpecularColor		= p.SpecularColor;	
		Compress			= p.Compress;
	}

	SImposterCreationParameters& operator = (const SImposterCreationParameters &p)
    {
		set(p);
		return *this;
    }

	bool equal(const SImposterCreationParameters &p) const
	{ return (*this) == p; }

	bool operator == (const SImposterCreationParameters &p) const
    {
		return ViewsCount         == p.ViewsCount
			&& CameraViewScale    == p.CameraViewScale
			&& SizeScale          == p.SizeScale
			&& TextureResolution  == p.TextureResolution
			&& AnimationFrameStep == p.AnimationFrameStep
			&& OptimalViewAngle   == p.OptimalViewAngle
			&& AmbientColor       == p.AmbientColor
			&& DiffuseColor       == p.DiffuseColor
			&& SpecularColor      == p.SpecularColor
			&& Compress           == p.Compress;
    }

	bool operator != (const SImposterCreationParameters &p) const
	{ return !(this->operator == (p)); }
};

//---------------------------------------------------------------------------------

//! Callback interface for catching events of ended animations.
class __MY_SCN_LIB_API__ IAnimationCallback : public IUnknown
{
public:

    //! Will be called when the animation playback has started.
    //! \param node: Node of which the animation has started.
	virtual void OnAnimationBegin(ISceneNode* node, u32 animation_idx) = 0;

    //! Will be called when the animation playback has ended.
    //! \param node: Node of which the animation has ended.
	virtual void OnAnimationEnd(ISceneNode* node, u32 animation_idx) = 0;
};

//---------------------------------------------------------------------------------

class __MY_SCN_LIB_API__ ISceneNode : public IUnknown
{
	friend class game::IGameNode;
	friend class dyn::CDynamicObject;     
    friend class dyn::CDynamicManager;
	friend class CSceneManager;
	friend class game::CGameManager;

protected:

public:    

    //! Constructor
    ISceneNode(E_SCENE_NODE_TYPE type, ISceneNode* parent, s32 id=-1)
		: IsVisible(true), Parent(NULL), 
		m_DebugDataVisible(false),
		RelativeTranslation(core::vector3df(0.0f, 0.0f, 0.0f)), 
		RelativeRotation(core::vector3df(0.0f, 0.0f, 0.0f)), 
		RelativeScale(core::vector3df(1.0f, 1.0f, 1.0f)), 
		IsMainPlayer(false), m_Shadow(0), DynObject(0), GameNode(0), m_Camera(0),
		Name(NONAME_SCENENODE_NAME), FileName(NONAME_FILE_NAME),	
		m_NodeType(type), ID(-1), m_Caption(0),
		m_Imposter(0), m_ImposterParameters(0), m_ImposterCreationParameters(0), 
		m_Rendered(false), TranfStored(false), m_Occluder(false), m_PotencialOccluder(false),
		m_CullingFilter(E_CULLING_TESTS(ECT_VIEW_FRUSTRUM_CULL|ECT_OCCLUDERS_FRUSTRUM_CULL)),
		m_Culled(true), m_CullingChecked(false),
		m_TransformationChanged(true), m_ParentTransformationChanged(true), 
		m_AbsolutePositionChanged(true), m_ShadowType(ESHT_SHADOW_FAKE),
		m_MaterialsLocked(false), 
		m_SceneNodesListIterator(0), m_OccludersListIterator(0),
		m_VisibleSceneNodesListIterator(0), m_VisibleSceneNodesListByTypeIterator(0),
		m_SceneNodesListByTypeIterator(0), m_SelectedNodesListIterator(0),
		m_DeletionQueueIterator(0),         
		m_VisibilityChangeToFalseQueueIterator(0), m_VisibilityChangeToTrueQueueIterator(0),
		m_IsClonableChild(false)
    {
		setID(ID);
		if (parent)
            parent->addChild(this);
		scn::onCreateSceneNode(this);
	}

    //! Destructor
    virtual ~ISceneNode()    
    {
		scn::onDeleteSceneNode(this);

		setAnimationCallback(NULL);

		detachCamera();
		detachShadow();
		detachCaption();
		detachDynamicObject();
		detachGameNode();

		// delete all children
        core::list<ISceneNode*>::iterator it = Children.begin();
        for (; it != Children.end(); ++it)		
            (*it)->drop();

        // delete all animators
        core::list<ISceneNodeAnimator*>::iterator ait = Animators.begin();
        for (; ait != Animators.end(); ++ait)	
			(*ait)->drop();				

		SAFE_DROP(m_Imposter);
		SAFE_DELETE(m_ImposterParameters);
		SAFE_DELETE(m_ImposterCreationParameters);

		clearRenderBuffers();
		m_Materials.clear();

		core::removeUniqueID(core::EUID_SCENE_NODE_ID, ID);
    }

	virtual void clearRenderBuffers()
	{
		// delete attached render buffers
		for (u32 i=0; i < m_RenderBuffers.size(); i++)
			m_RenderBuffers[i]->drop();
		m_RenderBuffers.set_used(0);
	}

	virtual void addRenderBuffer(vid::IRenderBuffer * rb)
	{ m_RenderBuffers.push_back(rb); }

	virtual u32 getRenderBuffersCount()
	{ return m_RenderBuffers.size(); }

	virtual vid::IRenderBuffer* getRenderBuffer(u32 i)
	{ return m_RenderBuffers[i]; }

	virtual	core::array<vid::IRenderBuffer*>& getRenderBuffers()
	{ return m_RenderBuffers; }

    virtual vid::SMaterial& getMaterial(s32 i = 0)
    { return m_Materials[i]; }

    virtual s32 getMaterialsCount()
    { return m_Materials.size(); }	

	//! set material
    virtual void setMaterial(vid::SMaterial& mat)
    {
		if (m_MaterialsLocked)
			return;
		s32 mat_count = getMaterialsCount();
		if (mat_count == 1)
		{
			vid::SMaterial &Mat = getMaterial(0);
			Mat = mat;
		}
    }  

    virtual void getVisibleChildsIntoSingleList(
		core::list<ISceneNode*> &child_list,
		core::array< core::list<ISceneNode*> > &child_list_by_type) 
    {
		core::list<ISceneNode*>::iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			ISceneNode *sn= (*it);

			if (!sn->isCulled())
			{
				E_SCENE_NODE_TYPE sntype = sn->getSceneNodeType();

				child_list.push_back(sn);
				child_list_by_type[sntype].push_back(sn);

				sn->m_VisibleSceneNodesListIterator       = child_list.get_last();
				sn->m_VisibleSceneNodesListByTypeIterator = child_list_by_type[sntype].get_last();

				sn->getVisibleChildsIntoSingleList(child_list, child_list_by_type);
			}
		}
    }

	virtual bool isCulled()
	{ return m_Culled; }

	//! is called on check visibility of this node
	virtual void OnCheckCulling()
	{
		if (!m_Culled)
		{
			core::list<ISceneNode*>::iterator it = Children.begin();
			for (; it != Children.end(); ++it)	
				(*it)->OnCheckCulling();				
		}
		m_CullingChecked = true;
	}

	//! 
	virtual void OnBeforeCulling()
	{
		m_CullingChecked = false;
		m_Rendered  = false; 
		m_Culled    = true;
		core::list<ISceneNode*>::iterator it = Children.begin();
		for (; it != Children.end(); ++it)	
			(*it)->OnBeforeCulling();	
	}

    //! This method is called just before the rendering process of the whole scene.
    //! Nodes may register themselves in the render pipeline during this call,
    //! precalculate the geometry which should be renderered, and prevent their
    //! children from being able to register them selfes if they are clipped by simply
    //! not calling their OnPreRender-Method.
    virtual void OnPreRender(u32 timeMs)  
	{ m_Rendered = true; }

    //! OnPostRender() is called just after rendering the whole scene.
    //! Nodes may calculate or store animations here, and may do other useful things,
    //! dependent on what they are.
    //! \param timeMs: Current time in milli seconds.
    virtual void OnPostRender(u32 timeMs)
    {
		// animate this node with all animators
		u32 acnt = Animators.size();
        core::list<ISceneNodeAnimator*>::iterator ait = Animators.begin();
		s32 idx = 0;
        for (; ait != Animators.end(); ++ait)
			(*ait)->animateNode(this, timeMs);
        // update absolute position
        updateAbsoluteTransformation();
    }

    //! Returns the name of the scene node.
    //! \return Returns name as wide character string.
    virtual const c8* getName() const
    { return Name.c_str(); }

    //! Sets the name of the scene node
    //! \param name: New name of the scene node.
    virtual void setName(const c8* name)
    { Name = name; }

    //! Returns the filename of the node.
    //! \return Returns filename as wide character string.
    virtual const c8* getFileName() const
    { return FileName.c_str(); }

    //! Sets the filename of the node.
    //! \param name: New filename of the scene node.
    virtual void setFileName(const c8* name)
    { FileName = name; }

    //! Returns the axis aligned, not transformed bounding box of this node.
    //! This means that if this node is a animated 3d character, moving in a room,
    //! the bounding box will always be around the origin. To get the box in
    //! real world coordinates, just transform it with the matrix you receive with
    //! getAbsoluteTransformation() or simply use getTransformedBoundingBox(),
    //! which does the same.
    virtual const core::aabbox3df& getBoundingBox() const = 0;

    //! Returns the axis aligned, transformed and animated absolute bounding box
    //! of this node.
    virtual const core::aabbox3df& getTransformedBoundingBox() const
    { return m_TransformedBoundingBox; }

    //! Returns true if the node is visible. This is only an option, set by the user and has
    //! nothing to do with geometry culling
    virtual bool isVisible()
    { return IsVisible; }

    //! Sets if the node should be visible or not. All children of this node won't be visible too.
    virtual void setVisible(bool isVisible)
    { IsVisible = isVisible; }

    //! Returns the id of the scene node. This id can be used to identify the node.
    virtual s32 getID()
    { return ID; }

    //! sets the id of the scene node. This id can be used to identify the node.
	//! ( also performing checking unique of new node identificator )
    virtual bool setID(s32 id)
    {		
		if (core::addUniqueID(core::EUID_SCENE_NODE_ID, id))
		{
			core::removeUniqueID(core::EUID_SCENE_NODE_ID, ID);
			ID = id;
			return true;
		}
		return false;
    }

    //! Adds a child to this scene node. If the scene node already
    //! has got a parent, it is removed from there as child.
    virtual void addChild(ISceneNode* child, bool clonable_child=false)
    {
		if (child)
        {
            child->grab();
			child->remove();
            Children.push_back(child);
            child->Parent = this;  
			child->m_IsClonableChild=clonable_child;
			m_ParentTransformationChanged = true;
        }
    }

    //! Removes a child from this scene node.
    //! \return Returns true if the child could be removed, and false if not.
    virtual bool removeChild(ISceneNode* child)
    {
		core::list<ISceneNode*>::iterator it = Children.begin();
        for (; it != Children.end(); ++it)
		{
            if ((*it) == child && !child->isLocked())
            {       
				(*it)->Parent = 0;
				(*it)->drop();
                Children.erase(it);
				return true;
            }
		}
        return false;
    }

    //! Removes all children of this scene node
    virtual void removeAll()
    {
		core::list<ISceneNode*>::iterator it = Children.begin();
		u32 nLockes =0;
		while (Children.size() > nLockes)
		{
			it = Children.begin() + nLockes;
			if (!(*it)->isLocked())
			{
				(*it)->Parent = 0;
				(*it)->drop();
				Children.erase(it);				
			}
			else
				nLockes++;
		}
    }

    //! Removes this scene node from the scene, deleting it.
    virtual bool remove()
    {
		if (Parent)
			return Parent->removeChild(this);
		return false;
    }

	//! Adds an animator which should animate this node.
    virtual void addAnimator(ISceneNodeAnimator* animator)
    {
		if (animator)
        {
            Animators.push_back(animator);
            animator->grab();
        }
    }

    //! Removes an animator from this scene node.
    virtual void removeAnimator(ISceneNodeAnimator* animator)
    {
		core::list<ISceneNodeAnimator*>::iterator it = Animators.begin();
        for (; it != Animators.end(); ++it)
		{
            if ((*it) == animator)
            {
                (*it)->drop();
                Animators.erase(it);				
                return;
            }
		}
    }

    //! Removes all animators from this scene node.
    virtual void removeAnimators()
    {
		core::list<ISceneNodeAnimator*>::iterator it = Animators.begin();
        for (; it != Animators.end(); ++it)
            (*it)->drop();
        Animators.clear();  
    }

    //! Gets the scale of the scene node. 
    //! \return Returns the scale of the scene node.
    virtual const core::vector3df& getScale() const
    { return RelativeScale; }

    //! Gets the rotation of the node. Note that this is
    //! the relative rotation of the node.
    //! \return Current relative rotation of the scene node.
    virtual const core::vector3df& getRotation() const
    { return RelativeRotation; }

    //! Gets the position of the node. Note that the position is
    //! relative to the parent.
    //! \return Returns the current position of the node relative to the parent.
    virtual const core::vector3df& getPosition() const
    { return RelativeTranslation; }

    //! Sets the scale of the scene node. 
    //! \param scale: New scale of the node
    virtual void setScale(const core::vector3df& scale)
    {
		RelativeScale = scale;
		rescaleNormals(scale);
		if (DynObject)
			dyn::setDynamicObjectScale(DynObject, scale);
		m_TransformationChanged = true;
	}

    //! Sets the rotation of the node. This only modifies
    //! the relative rotation of the node.
    //! \param roation: New rotation of the node in degrees.
    virtual void setRotation(const core::vector3df& rotation, bool controlled_by_ai = false)
    {
		if (DynObject)
			dyn::setDynamicObjectRotation(DynObject, rotation, controlled_by_ai);
		else
			RelativeRotation = rotation;
		m_TransformationChanged = true;
    }

    //! Sets the position of the node. Note that the position is
    //! relative to the parent.
    //! \param newpos: New relative postition of the scene node.
    virtual void setPosition(const core::vector3df& newpos, bool controlled_by_ai = false)
    {
		if (DynObject)
			dyn::setDynamicObjectPosition(DynObject, newpos, controlled_by_ai);
		else
			RelativeTranslation = newpos; 
		m_TransformationChanged = true;
		m_AbsolutePositionChanged = true;
    }

    //! Gets the abolute position of the node. The position is absolute.
    //! \return Returns the current absolute position of the scene node.
    virtual const core::vector3df& getAbsolutePosition()
    {
		if (m_AbsolutePositionChanged)
		{
			updateAbsoluteTransformation();
			AbsolutePosition.set(0.0f, 0.0f, 0.0f);
			AbsoluteTransformation.transformVect(AbsolutePosition);
			m_AbsolutePositionChanged = false;
		}
        return AbsolutePosition;
    }

    //! returns the absolute transformation of the node. 
	//! Is recalculated every OnPostRender()-call.
    inline virtual const core::matrix4& getAbsoluteTransformation() 
    {
		if (m_TransformationChanged || m_ParentTransformationChanged)
			updateAbsoluteTransformation();
		return AbsoluteTransformation;
    }

	virtual const core::matrix4& getRelativeTransformation() 
	{
		if (m_TransformationChanged)
		{
			// rotation
			RelativeTransformation.setRotationDegrees(RelativeRotation);

			// translation
			RelativeTransformation.setTranslation(RelativeTranslation);

			// scaling
			if (RelativeScale != core::vector3df(1,1,1))
			{
				static core::matrix4 smat;
				smat.setScale(RelativeScale);
				RelativeTransformation *= smat;
			}
		}
		return RelativeTransformation;
	}

    //! updates the absolute position based on the relative and the parents position
    virtual void updateAbsoluteTransformation()
    {		
		if (m_TransformationChanged)
		{
			if (Parent)
			{
				AbsoluteTransformation = 
					Parent->getAbsoluteTransformation() * getRelativeTransformation();
				m_ParentTransformationChanged = false;
			}
			else
				AbsoluteTransformation = getRelativeTransformation();

			m_TransformedBoundingBox = getBoundingBox();
			AbsoluteTransformation.transformBox(m_TransformedBoundingBox);

			m_TransformationChanged = false;

			core::list<ISceneNode*>::iterator it = Children.begin();
			for (; it != Children.end(); ++it)
			{
				(*it)->setParentTransformationChanged();
				(*it)->updateAbsoluteTransformation();
			}

			m_AbsolutePositionChanged = true;
			m_CullingChecked = false;

			if (GameNode)
				game::updateGameNode(GameNode);
		}
		else 
		if (Parent && m_ParentTransformationChanged)
		{
			AbsoluteTransformation = 
				Parent->getAbsoluteTransformation() * getRelativeTransformation();

			m_TransformedBoundingBox = getBoundingBox();
			AbsoluteTransformation.transformBox(m_TransformedBoundingBox);

			m_ParentTransformationChanged = false;	 

			core::list<ISceneNode*>::iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				(*it)->updateAbsoluteTransformation();

			m_AbsolutePositionChanged = true;
			m_CullingChecked = false;

			if (GameNode)
				game::updateGameNode(GameNode);
		}
		else
			m_ParentTransformationChanged = false;

    }

	virtual void setParentTransformationChanged()
	{
		m_ParentTransformationChanged = true;

		core::list<ISceneNode*>::iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->setParentTransformationChanged();
	}

    //! 
    virtual void storeTransformation()
    {
		if (!TranfStored)
		{
			StoredRelativeRotation    = RelativeRotation;
			StoredRelativeTranslation = RelativeTranslation;
			StoredRelativeScale       = RelativeScale;
			StoredAbsoluteTransformation = AbsoluteTransformation;
			StoredAbsolutePosition    = AbsolutePosition;
			TranfStored = true;
		}
    }

    //! 
    virtual void restoreTransformation()
    {
		if (TranfStored)
		{
			RelativeRotation    = StoredRelativeRotation;
			RelativeTranslation = StoredRelativeTranslation;
			RelativeScale       = StoredRelativeScale;
			AbsoluteTransformation = StoredAbsoluteTransformation;
			AbsolutePosition    = StoredAbsolutePosition;

			TranfStored = false;

			m_TransformationChanged = m_ParentTransformationChanged = m_AbsolutePositionChanged = true;
		}
    }

    //! Sets if debug data like bounding boxes should be drawed.
    //! Please note that not all scene nodes support this feature.
    virtual void setDebugDataVisible(bool visible)
    {
		m_DebugDataVisible = visible;
		if (m_Imposter)
			m_Imposter->setDebugDataVisible(visible);		
    }

    //! Returns if debug data like bounding boxes are drawed.
    virtual bool isDebugDataVisible()
    { return m_DebugDataVisible; }

    //! Returns a const reference to the list of all children.
    virtual const core::list<ISceneNode*>& getChildren() const
    { return Children;}

    //! Changes the parent of the scene node.
    virtual void setParent(ISceneNode* newParent, bool clonable_child=false)
    {
		grab();
		remove();
        Parent = newParent;
        if (Parent)
		{
            Parent->addChild(this, clonable_child);

			core::vector3df abs_pos = getAbsolutePosition();
			core::matrix4 parent_transf = Parent->getAbsoluteTransformation();
			parent_transf.makeInversed();			
			parent_transf.transformVect(abs_pos);

			setRotation(core::vector3df(0,0,0)); 
			setPosition(abs_pos);				
			
			updateAbsoluteTransformation();
		}
        drop();
    }

    //! Returning parent of this scene node.
    virtual ISceneNode* getParent()
    { return Parent; }

	virtual bool setParentID(s32 parent_id)
	{
		if (Parent && Parent->getID() == parent_id)
			return true;
		scn::ISceneNode *new_parent =
			scn::getSceneNodeFromId(parent_id);		
		if (!new_parent)
			new_parent = scn::getRootSceneNode();
		setParent(new_parent);
		if (!Parent || Parent->getID() != parent_id)
			return false;
		return true;
	}

	virtual s32 getParentID()
	{ return Parent ? Parent->getID() : -1; }

	//! return true if this scene node attached to main player
    virtual bool isMainPlayer()
    { return IsMainPlayer; }

    virtual void setIsMainPlayer(bool mp)
    { IsMainPlayer = mp;}

    //! draw debug data
    virtual void drawDebugData(const core::aabbox3d <f32> &Box) {}    

    //! Adds shadow to this scene node    
	virtual IShadow* attachShadow(E_SHADOW_TYPE type)
	{
		if (m_NodeType != ESNT_ANIMATED_MESH_SCENE_NODE)
			return NULL;
		if (!m_Shadow)
		{
			m_Shadow = scn::createShadowForSceneNode(this, type);
			m_ShadowType = type;
		}   
   		return m_Shadow;
	}

    //! Remove shadow this scene node    
	virtual bool detachShadow()
	{
		bool res = false;
		if (m_Shadow)
			res = scn::destroyShadowForSceneNode(this);
		m_Shadow =0;
		return true;
	}

	//! returns shadow scene node
	virtual IShadow* getShadow()
	{ return m_Shadow; }

    //! Adds caption to this scene node    
	virtual ITextSceneNode* attachCaption(
		vid::IFont *font = NULL, const c8* text = "Caption",
		img::SColor color = img::SColor(100,255,255,255))
	{
		if (!m_Caption)
		{
			m_Caption = scn::addTextSceneNode(this, -1, font, text, color);
			((ISceneNode*)m_Caption)->grab();
		}
   		return m_Caption;
	}

    //! Remove caption this scene node    
	virtual bool detachCaption()
	{
		if (m_Caption &&
				((ISceneNode*)m_Caption)->getParent() == this)
			((ISceneNode*)m_Caption)->remove();
		if (m_Caption)
		{
			((ISceneNode*)m_Caption)->drop();
			m_Caption = NULL;
		}
		return true;
	}

    //! Show/hide caption of this scene node    
	virtual bool showCaption(bool show)
	{
		if (m_Caption)
			((ISceneNode*)m_Caption)->setVisible(show);
			return true;
		return false;
	}

	//! returns caption scene node
	virtual ITextSceneNode* getCaption()
	{ return m_Caption; }

    //! Adds dynamic object to this scene node
	virtual dyn::IDynamicObject* attachDynamicObject(
		dyn::SDynamicObjectParams &ode_obj_params)
	{
		if (!DynObject)
			DynObject = dyn::createDynamicObjectForSceneNode( this, ode_obj_params );
		return DynObject;
	}

    //! Remove Dynamic object from this scene node
	virtual bool detachDynamicObject()
	{
		if (DynObject)
			dyn::destroyDynamicObjectForSceneNode(this);
		DynObject =0;
		return true;
	}

	//! returns dynamic object
	virtual dyn::IDynamicObject* getDynamicObject()
	{ return DynObject; }	

    //! Adds game node to this scene node
	virtual game::IGameNode* attachGameNode(game::SGameNodeParams &game_node_params)
	{
		if (!GameNode)
			GameNode = game::createGameNodeForSceneNode( this, game_node_params );
		return GameNode;
	}

    //! Remove game node from this scene node
	virtual bool detachGameNode()
	{
		if (GameNode)
			game::destroyGameNodeForSceneNode(this);
		GameNode =0;
		return true;
	}

	//! returns game node attached to this scene node
	virtual game::IGameNode* getGameNode()
	{ return GameNode; }	

	//! Adds imposter to this scene node node
	virtual ISceneNode* attachImposter(
		const SImposterCreationParameters &creation_params,
		const SImposterParameters &params,
		bool recreate_if_exists = false)
	{ return NULL; }

	//! Adds imposter to this scene node node
	virtual bool attachImposter(ISceneNode* imposter, 
		const SImposterCreationParameters &creat_params,
		const SImposterParameters &params)
	{
		if (m_Imposter || !imposter || 
			imposter->getSceneNodeType()!=scn::ESNT_BILLBOARD_SCENE_NODE)
			return false;

		bool need_to_add_shadow = false;
		if (m_Shadow)
		{
			detachShadow();
			need_to_add_shadow = true;
		}

		m_Imposter = imposter;

		SAFE_GRAB(m_Imposter);		

		if (m_Imposter)
		{
			if (!m_ImposterParameters)
				m_ImposterParameters = new SImposterParameters();

			*m_ImposterParameters = params;

			if (!m_ImposterCreationParameters)
				m_ImposterCreationParameters = new SImposterCreationParameters();

			*m_ImposterCreationParameters = creat_params;

			addChild(m_Imposter);

			m_Imposter->setVisible(false);				
		}
		
		if (need_to_add_shadow)
			attachShadow(m_ShadowType);

		return true;
	}

	//! Remove imposter from this scene node node
	virtual bool detachImposter()
	{
		if (m_Imposter)
		{
			bool need_to_add_shadow = false;
			if (m_Shadow)
			{
				detachShadow();
				need_to_add_shadow = true;
			}

			if (m_Imposter->getParent() == this)
				m_Imposter->remove();
			m_Imposter->drop();
			m_Imposter = 0;	

			delete m_ImposterParameters;
			m_ImposterParameters=0;

			delete m_ImposterCreationParameters; 
			m_ImposterCreationParameters=0;
			
			if (need_to_add_shadow)
				attachShadow(m_ShadowType);

			return true;
		}
		return false;
	}

	//! getting imposter to this scene node node
	virtual ISceneNode* getImposter()
	{ return m_Imposter; }

	//! attaching camera to this scene node
	virtual void attachCamera(ICameraSceneNode* camera)
	{ scn::attachCamera(this, camera); }

	//! detaching camera from this scene node
	virtual void detachCamera()
	{ scn::detachCamera(this); }

	//! return camera attached to this scene node
	virtual ICameraSceneNode* getCamera()
	{ return m_Camera; }

	//! return type of this scene node
	virtual E_SCENE_NODE_TYPE getSceneNodeType()
	{ return m_NodeType; }

	//! returns file name of the physical mesh of this scene node
	virtual const c8* getPhysicalMeshFullFileName()
	{ return NONAME_FILE_NAME; }

	//! returns physical mesh
	virtual IAnimatedMesh* getPhysicalMesh()
	{ return NULL; }

	//! setting physical mesh 
	virtual void setPhysicalMesh(IAnimatedMesh* mesh) {}

	//! rescale nodes normals
	virtual bool rescaleNormals(const core::vector3df& new_scale)
	{ return false; }

	//! return true if this node was renered in this frame
	virtual bool isRendered()
	{ return m_Rendered; }

	//! getting imposter common parameters
	const SImposterParameters* getImposterParameters()
	{ return m_ImposterParameters; }

	//! setting imposter common parameters
	virtual bool setImposterParameters(const SImposterParameters &params)
	{
		if (!m_ImposterParameters)
			return false;
		*m_ImposterParameters = params;
		return true;
	}

	//!
	const SImposterCreationParameters* getImposterCreationParameters()
	{ return m_ImposterCreationParameters; }

	//! getting scene node params
	virtual SSceneNodeCommonParams getParameters()
	{ return scn::getSceneNodeParams(this); }

	//! setting scene node params
	virtual void setParameters(SSceneNodeCommonParams &params)
	{ scn::applySceneNodeParams(this, params); }

	//! setting scene node params
	virtual void setParameters(SSceneNodeParameters &parameters)
	{ scn::applySceneNodeParams(this, parameters); }

	//! setting scene node params
	virtual void setParameters(SSceneNodeTransformation &transformation)
	{ scn::applySceneNodeParams(this, transformation); }

	//! setting scene node params
	virtual void setParameters(SSceneNodeMaterial &material)
	{ scn::applySceneNodeParams(this, material); }

	//! setting occluder indication for this scene node,
	//! this mean that scene node can occlude other nodes in scene 
	virtual bool setOccluder(bool occluder)
	{
		if (m_Occluder != occluder && m_NodeType == ESNT_ANIMATED_MESH_SCENE_NODE)
		{
			m_Occluder = occluder;
			scn::onSetOccluder(this, occluder);
			m_PotencialOccluder |= occluder;
			return true;
		}
		return false;	
	}

	//! return true if this scene node occluding other nodes in scene
	virtual bool isOccluder()
	{ return m_Occluder; }

	//! return true if this scene node may occluding other nodes in scene
	virtual bool isPotencialOccluder()
	{ return m_PotencialOccluder; }

	//! getting culling filter for this scene node
	virtual E_CULLING_TESTS getCullingFilter()
	{ return m_CullingFilter; }

	//! setting culling filter for this scene node
	virtual void setCullingFilter(E_CULLING_TESTS cull_filter)
	{ m_CullingFilter = cull_filter; }

	//! setting scene node properties from other scene node
	virtual bool set(ISceneNode* other)
	{ return false; }

	//! return string with scene node's core properties in xml format
	virtual const c8* getSceneCorePropertiesXMLString() = 0;

	//!
	virtual void lockMaterials()
	{ m_MaterialsLocked = true; }

	//!
	virtual void unlockMaterials()
	{ m_MaterialsLocked = false; }

	//!	
	const core::list<ISceneNodeAnimator*>& getAnimators()
	{ return Animators; }

	//!
	bool isClonableChild()
	{ return m_IsClonableChild; }

	//! Geting animations count 
	virtual u32 getAnimationsCount()
	{ return 0; }

	//! Starts a special animation. 
	virtual bool setCurrentAnimation(u32 anim, f32 delay=0.0f)
	{ return false; }

    //! Geting current animation. 
	virtual u32 getCurrentAnimation() { return 0; }

	//! Sets the progress of the current animation (value between [0,1])
	virtual bool setCurrentAnimationProgress(f32 progress)
	{ return false; }

	//! Gets the progress of the current animation (value between [0,1])
	virtual f32 getCurrentAnimationProgress()
	{ return 0; }

    //! Sets a callback interface which will be called if an animation
    //! playback has ended. Set this to 0 to disable the callback again.
    //! Please note that this will only be called when in non looped mode,
	virtual void setAnimationCallback(IAnimationCallback *callback = 0) {}

protected:

    //! file name of the scene node.
    core::stringc FileName;

    //! name of the scene node.
    core::stringc Name;

	//! relative transformation of the node.
	core::matrix4 RelativeTransformation;

    //! absolute transformation of the node.
    core::matrix4 AbsoluteTransformation, StoredAbsoluteTransformation; 

    //! relative translation of the scene node.
    core::vector3df RelativeTranslation, StoredRelativeTranslation;

    //! relative rotation of the scene node.
    core::vector3df RelativeRotation, StoredRelativeRotation;

    //! relative scale of the scene node.
    core::vector3df RelativeScale, StoredRelativeScale;

	core::vector3df AbsolutePosition, StoredAbsolutePosition;

	bool TranfStored;

    //! Pointer to the parent
    ISceneNode* Parent;

    //! List of all children of this node
    core::list<ISceneNode*> Children;

    //! List of all animators of this node
    core::list<ISceneNodeAnimator*> Animators;

    //! id of the node. 
    s32 ID;

    //! flag if debug data should be drawed, like Bounding Boxes.
    bool m_DebugDataVisible;      

    //! is the node visible?
    bool IsVisible;

    //! is this a main player
    bool IsMainPlayer;

	//! m_Shadow of this Node
	IShadow* m_Shadow;
	E_SHADOW_TYPE m_ShadowType;

	//! Caption of this node
	ITextSceneNode* m_Caption;

    //! dynamic object, attached to this scene node
	dyn::IDynamicObject* DynObject;

	//! game node attached to this scene node
	game::IGameNode* GameNode;

	//! camera attached to this scene node
	ICameraSceneNode* m_Camera;

	ISceneNode                  *m_Imposter;
	SImposterParameters         *m_ImposterParameters;
	SImposterCreationParameters *m_ImposterCreationParameters;

	bool m_Rendered;

	bool m_Occluder, m_PotencialOccluder;

	E_CULLING_TESTS m_CullingFilter;

	bool m_Culled, m_CullingChecked;

	bool m_TransformationChanged, m_ParentTransformationChanged, m_AbsolutePositionChanged;

	bool m_MaterialsLocked;

	core::list<ISceneNode*>::iterator       m_SceneNodesListIterator,        m_OccludersListIterator;
	core::list<ISceneNode*>::iterator       m_VisibleSceneNodesListIterator, m_VisibleSceneNodesListByTypeIterator;
	core::list<ISceneNode*>::iterator       m_SceneNodesListByTypeIterator,  m_SelectedNodesListIterator;
	core::list<SActionQueueEntry>::iterator m_DeletionQueueIterator;         
	core::list<SActionQueueEntry>::iterator m_VisibilityChangeToFalseQueueIterator, m_VisibilityChangeToTrueQueueIterator;

	core::aabbox3df m_TransformedBoundingBox;

	bool m_IsClonableChild;

protected:

	core::array<vid::SMaterial> m_Materials;

private:

	core::array<vid::IRenderBuffer*> m_RenderBuffers;

	//! Type of the scene node
	mutable E_SCENE_NODE_TYPE m_NodeType;	
};

//---------------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//---------------------------------------------------------------------------------

#endif // #ifndef ISceneNodeHPP

