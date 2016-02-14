//|-------------------------------------------------------------------------
//| File:        CSceneNodeCreators.hpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <scn/ISceneNode.h>
#include <scn/ISceneNodeCreator.h>
#include <scn/SAnimatedMesh.h>
#include <scn/SMesh.h>

//----------------------------------------------------------------------------
namespace my {
namespace scn {
//----------------------------------------------------------------------------

#define PRE_CREATE_SCENE_NODE_MACRO(sntype)                                             \
do {                                                                                    \
	if (log_info)                                                                       \
		LOGGER.logInfo("Creating %s Scene Node . . .", SceneNodeTypeShortStr[sntype]);  \
	if (snparams.Parameters.Type != sntype)                                             \
		LOGGER.logWarn("Wrong Scene Node Type (wanted '%s', got '%s') !",               \
			SceneNodeTypeShortStr[sntype],                                              \
			SceneNodeTypeShortStr[snparams.Parameters.Type]);                           \
} while (0)

//----------------------------------------------------------------------------

#define POST_CREATE_SCENE_NODE_MACRO(sntype)                                                  \
do {                                                                                          \
	if (!node)                                                                                \
		LOGGER.logErr("Couldn't create %s Scene Node node!", SceneNodeTypeShortStr[sntype] ); \
	else if (log_info)                                                                        \
		LOGGER.logInfo("%s Scene Node successfully created", SceneNodeTypeShortStr[sntype]);  \
} while (0)

//----------------------------------------------------------------------------

class CAnimatedMeshSceneNodeCreator : public ISceneNodeCreator
{
public:

	CAnimatedMeshSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CAnimatedMeshSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::IAnimatedMeshSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_ANIMATED_MESH_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			scn::IAnimatedMesh *amesh = SCENE_MANAGER.getMesh("#box_mesh_50_100_50");
			if (amesh)
			{
				node = SCENE_MANAGER.addAnimatedMeshSceneNode();
				node->setAnimatedMesh(amesh);
			}
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CBillboardSceneNodeCreator : public ISceneNodeCreator
{
public:

	CBillboardSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CBillboardSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{	
		scn::IBillboardSceneNode* node = 0;	

		E_SCENE_NODE_TYPE sntype = ESNT_BILLBOARD_SCENE_NODE;

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		node = SCENE_MANAGER.addBillboardSceneNode();
		node->setSize(core::dimension2d<f32>(200,200));

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CLensFlareSceneNodeCreator : public ISceneNodeCreator
{
public:

	CLensFlareSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CLensFlareSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ILensFlareSceneNode* node = 0;	

		E_SCENE_NODE_TYPE sntype = ESNT_LENS_FLARE_SCENE_NODE;

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		node = SCENE_MANAGER.addLensFlareSceneNode(); 

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CLightSceneNodeCreator : public ISceneNodeCreator
{
public:

	CLightSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CLightSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ILightSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_LIGHT_SCENE_NODE;

		PRE_CREATE_SCENE_NODE_MACRO(sntype);
		
	    vid::SLight lightData;
		lightData.Radius=300.0f;
		lightData.Enabled=true;
		lightData.IntensityWaveEnabled = true;

		node = SCENE_MANAGER.addLightSceneNode();

		if (node) 		
	    {
			// set light data
			node->setLightParameters(lightData);
	    }

	    POST_CREATE_SCENE_NODE_MACRO(sntype);

		return node;
	}
};

//----------------------------------------------------------------------------

class CKTreeSceneNodeCreator : public ISceneNodeCreator
{
public:

	CKTreeSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CKTreeSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::IKTreeSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_KTREE_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			scn::IAnimatedMesh *amesh = SCENE_MANAGER.getMesh("#box_mesh_1000_50_1000");
			if (amesh)
			{
				node = SCENE_MANAGER.addKTreeSceneNode();
				node->setAnimatedMesh(amesh);
			}
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CParticleSystemSceneNodeCreator : public ISceneNodeCreator
{
public:

	CParticleSystemSceneNodeCreator()
	{
	#if MY_DEBUG_MODE 
		setClassName("CParticleSystemSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::IParticleSystemSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_PARTICLE_SYSTEM_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			// adding default particle system
			node = SCENE_MANAGER.addParticleSystemSceneNode();

			IParticleEmitter* emitter = node->addEmitter(EPET_BOX);						
			
			IParticleAffector* affector = emitter->addAffector(EPAT_FADEOUT);	
			affector->setAffectTimeMs(1000);
			((CParticleFadeOutAffector*)affector)->setTargetColor(img::SColor(0,0,0,0));
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CSkyBoxSceneNodeCreator : public ISceneNodeCreator
{
public:

	CSkyBoxSceneNodeCreator()
	{
	#if MY_DEBUG_MODE 
		setClassName("CSkyBoxSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ISkyBoxSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_SKY_BOX_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			// adding default sky box
			node = SCENE_MANAGER.addSkyBoxSceneNode(0,0,0,0,0,0);
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}    
};

//----------------------------------------------------------------------------

class CTestSceneNodeCreator : public ISceneNodeCreator
{
public:

	CTestSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTestSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ITestSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_TEST_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			node = SCENE_MANAGER.addTestSceneNode();
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CTextSceneNodeCreator : public ISceneNodeCreator
{
public:

	CTextSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTextSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ITextSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_TEXT_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			node = SCENE_MANAGER.addTextSceneNode(NULL, "Text");
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CDummySceneNodeCreator : public ISceneNodeCreator
{
public:

	CDummySceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CDummySceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::IDummyTransformationSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_DUMMY_TANSFORMATION_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			node = SCENE_MANAGER.addDummyTransformationSceneNode();
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------

class CTerrainLibMiniSceneNodeCreator : public ISceneNodeCreator
{
public:

	CTerrainLibMiniSceneNodeCreator() 
	{
	#if MY_DEBUG_MODE 
		setClassName("CTerrainLibMiniSceneNodeCreator");
	#endif
	}

	virtual scn::ISceneNode* createSceneNode(SSceneNodeCommonParams snparams, bool log_info)
	{
		scn::ITerrainSceneNode* node = 0;		

		E_SCENE_NODE_TYPE sntype = ESNT_TERRAIN_SCENE_NODE;		

		PRE_CREATE_SCENE_NODE_MACRO(sntype);

		if (snparams.Parameters.Type == sntype)
		{
			node = SCENE_MANAGER.addLibMiniTerrainSceneNode();
		}

		POST_CREATE_SCENE_NODE_MACRO(sntype);
		
		return node;
	}
};

//----------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//----------------------------------------------------------------------------
