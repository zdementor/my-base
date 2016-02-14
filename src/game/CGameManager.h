//|-------------------------------------------------------------------------
//| File:        CGameManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef CGameManagerHPP
#define CGameManagerHPP
//----------------------------------------------------------------------------

#include <game/IGameManager.h>
#include <dev/IDevice.h>
#include <scn/ITerrainSceneNode.h>
#include <scn/ISceneNode.h>
#include <scn/ILightSceneNode.h>
#include <scn/IBillboardSceneNode.h>
#include <scn/IParticleSystemSceneNode.h>
#include <scn/ISkyBoxSceneNode.h>
#include <scn/ILensFlareSceneNode.h>
#include <scn/ICameraSceneNode.h>
#include <scn/ITestSceneNode.h>
#include <game/IGameNodeAI.h>
#include <game/SGameSceneProps.h>

#include "CGameSceneLoader.h"
#include "CGameSceneSaver.h"

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

enum E_GAME_PROFILE_TYPES
{
	EGPT_WHOLE_GAME = 0,
	EGPT_PRE_GAME,
	EGPT_DO_GAME,
	EGPT_DYNAMIC,
	EGPT_POST_GAME,
	EGPT_SCRIPT,
	EGPT_SOUND,
	E_GAME_PROFILE_TYPES_COUNT,
	E_GAME_PROFILE_TYPES_FORCE_32_BIT = 0xffffffff
};

//----------------------------------------------------------------------------

static const c8* GameProfileTypeNames[E_GAME_PROFILE_TYPES_COUNT] =
{
	"Whole Game",
	"Pre Game",
	"Do Game",
	"Dynamic",
	"Post Game",
	"Scripting",
	"Sound"
};

//----------------------------------------------------------------------------

//! Game Manager. Used as manager of all game scene nodes and games stuff
class CGameManager : public IGameManager
{
	friend class CGameNode;
	friend class CGameUnitsFormation;
	friend class CGameAIModule;
	friend class CGameTasksManager;
	friend class CGameEventsDispatcher;

public:

	CGameManager();
	virtual ~CGameManager();

	virtual IGameNode* getGameNode(u32 idx);
	virtual u32 getGameNodesCount();

	virtual u32 getActiveGameNodesCount();

	virtual IGameNode* getGameNode(	E_GAME_NODE_TYPE gntype, u32 idx=0 );
	virtual u32 getGameNodesCount(E_GAME_NODE_TYPE gntype);

	virtual E_GAME_NODE_TYPE	getGameNodeType (IGameNode* game_node);
	virtual u32					getGameNodeIndex(IGameNode* game_node);
	virtual u32					getGameNodeTypeAndIndex(
		IGameNode* game_node, E_GAME_NODE_TYPE *gntype=NULL, u32 *idx = NULL);

	virtual void applyGameNodeCommonParams(
		scn::ISceneNode *node, SGameNodeCommonParams &node_params);

	virtual const core::list<IGameNode*>& getGameNodesByType(E_GAME_NODE_TYPE gntype);
    virtual const core::list<IGameNode*>& getGameNodes();
    virtual const core::list<IGameNodeAI*>& getGameNodesAI();

    virtual IGameUnitsFormation* addGameUnitsFormation(core::dimension2di dim, s32 formation_id);
    virtual IGameUnitsFormation* getGameUnitsFormationFromID(s32 formation_id);

    virtual IGameNode* addGameNode(SGameNodeCommonParams &node_params, scn::ISceneNode* scene_node);
    virtual IGameNode* addGameNodeForSceneNode(scn::ISceneNode* n, SGameNodeParams &params);
    virtual bool removeGameNodeForSceneNode(scn::ISceneNode* n);

	virtual IGameNode* createGameNode(E_GAME_NODE_TYPE game_node_type, scn::E_SCENE_NODE_TYPE scene_node_type, bool log_info = false);
    virtual IGameNode* createGameNode(SGameNodeCommonParams &node_params, bool log_info = false);
    virtual IGameNode* cloneGameNode(IGameNode *game_node, bool log_info = false);
    virtual IGameNode* reloadGameNode(IGameNode* gn);

	virtual IGameNode* loadGameNode(const c8* fname);
	virtual IGameNode* loadGameNode(game::E_GAME_NODE_TYPE gntype, const c8* fname);
	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, const c8* fname);
	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file);     
                                                                                            
	virtual void clearGameScene(bool clear_resourses=true);
	virtual void loadGameSceneLater(const c8* filename);
	virtual bool loadGameScene(const c8* filename);
	virtual bool saveGameScene(const c8* filename);

	virtual const c8* getGameSceneFileName();

	virtual IGameNodeMainPlayer* setCurrentMainPlayer(s32 idx);
	virtual s32 getMainPlayersCount();
	virtual IGameNodeMainPlayer* getMainPlayerGameNode();
	virtual scn::ISceneNode* getMainPlayerSceneNode();
	virtual dyn::IDynamicObject* getMainPlayerDynamicObject();
	virtual scn::ICameraSceneNode* getMainPlayerCamera();

    virtual IGameNode* getGameNodeFromScreenCoordinates(const core::position2di &scr_pos);

	virtual void stopGame();
	virtual void startGame();

	virtual void doGame();	
	virtual void preRenderFrame();
	virtual void postRenderFrame();

	//! show/hide wait window
	virtual void showMessageBox(
		bool show, const c8 *caption = "", const c8 *message = "");   

    //! returns current dynamic calculation per second value
    virtual s32 getDynamicPS();

    //! returns current game calculation per second value
    virtual s32 getGamePS();

	//! returns current script parsing per second value
    virtual s32 getScriptPS();

	//! return true if game is started, otherwise false
	virtual bool isGameStarted();

	//! setting file name of game script
	virtual void setGameScriptFileName(core::stringc &scr_fname);

	//! Return file name of game script
	virtual core::stringc getGameScriptFileName();

	//! adding game sound track
	virtual void addSoundTrack(core::stringc &snd_fname);

	//! removing game sound tracks
	virtual void removeAllSoundTracks();
		
	//! return game sound track object by its index
	virtual mm::ISound* getSoundTrack(s32 idx);

	//! return overal count of game sound tracks
	virtual s32 getSoundTracksCount();

	//! starts playing sound track
	virtual bool playSoundTrack(s32 idx);

	//! paused playing sound track
	virtual bool pauseSoundTrack(s32 idx);

	//! stopps all playing sound tracks
	virtual void stopAllSoundTracks();

	//! setting sound tracks volume (0.0f - 1.0f) 
	virtual void setSoundTracksVolume(f32 volume);

	//! getting sound tracks volume (0.0f - 1.0f)
	virtual f32 getSoundTracksVolume();

	//! setting sound effects volume (0.0f - 1.0f)
	virtual void setSoundEffectsVolume(f32 volume);

	//! getting sound effects volume (0.0f - 1.0f)
	virtual f32 getSoundEffectsVolume();

	//! creating game inventory for node
	virtual IGameInventory* createGameInventory(game::IGameNode *owner);

	//! getting game node params
	virtual SGameNodeParams getGameNodeParams(IGameNode *gnode);

	//! setting game node params
	virtual void applyGameNodeParams(IGameNode *gnode, SGameNodeParams &params);

	//! registering game node class
	virtual bool registerGameNodeClass(
		c8 *class_name,
		E_GAME_NODE_TYPE game_node_type,
		IGameNode* (*alloc_func )(scn::ISceneNode* n, SGameNodeParams &params),
		IGameNode* (*create_func)(SGameNodeCommonParams &node_params, bool log_info),
		IGameNode* (*clone_func )(IGameNode *game_node, bool log_info),
		IGameNode* (*load_func  )(SGameNodeCommonParams &node_params, io::IReadFile* file)
		);	

  	virtual IPathFindTask* createPathFindTask();

	virtual const SGameSceneProps& getGameSceneProps(); 

	virtual void setGameSceneProps(const SGameSceneProps& props); 

	virtual core::array<SItemDropTableLine>& getItemDropTable(E_GAME_NODE_TYPE type);

	//////////////////////////////////
	//!      game callbacks
	//////////////////////////////////

	virtual void setCallbackOnShowMessage(void (*callback)(bool, const c8*, const c8*))
	{ m_CallbackOnShowMessage = callback; }

	virtual void setCallbackBeforeLoadScene(void (*callback)())
	{ m_CallbackBeforeLoadScene = callback; }

	virtual void setCallbackAfterLoadScene(void (*callback)())
	{ m_CallbackAfterLoadScene = callback; }

	virtual void setCallbackBeforeSaveScene(void (*callback)())
	{ m_CallbackBeforeSaveScene = callback; }

	virtual void setCallbackAfterSaveScene(void (*callback)())
	{ m_CallbackAfterSaveScene = callback; }

	virtual void setCallbackOnDeleteNode(void (*callback)(IGameNode*))
	{ m_CallbackOnDeleteNode = callback; }

	virtual void setCallbackOnCreateNode(void (*callback)(IGameNode* gnode))
	{ m_CallbackOnCreateNode = callback; }

	virtual void setCallbackOnDieNode(void (*callback)(IGameNode* gnode))
	{ m_CallbackOnDieNode = callback; }

	virtual void setCallbackOnCollectItem(void (*callback)(IGameNode*, IGameNodeItem*))
	{ m_CallbackOnCollectItem = callback; }

	virtual void setCallbackOnThrowItem(void (*callback)(IGameNode*, IGameNodeItem*))
	{ m_CallbackOnThrowItem = callback; }

	//----------------------------------------------------------------------------

	void CGameManager::setCallbackOnUseItem( void (*callback)(IGameNode*, IGameNodeItem*) )
	{ m_CallbackOnUseItem = callback; }

	//////////////////////////////////
	//!      game functions
	//////////////////////////////////

	//! return true if trigger activated
	virtual bool IsTriggerActivated(s32 id);

	//! return true if door activated
	virtual bool IsDoorActivated(s32 id);

	//! one game node hited by another
	virtual int GameNodeHit(IGameNode *victim, IGameNode *agressor, s32 hit_power);

	//! game node death
	virtual int GameNodeDie(IGameNode *victim);

	//! game node simple move
	virtual void GameNodeMove(IGameNode *moving_node, core::vector3df dest_pos);

	//! game node movewith complex trajectory
	virtual void GameNodeMoveTrajectory(IGameNode *moving_node, core::array<core::vector3df> &way_points);	

private:

	//////////////////////////////////
	//!      game events
	//////////////////////////////////

	virtual void onDeleteNode(IGameNode* gnode);
	virtual void onCreateNode(IGameNode* gnode);
	virtual void onDieNode(IGameNode* gnode);
	virtual void onCollectItem(IGameNode* gnode, IGameNodeItem* item);
	virtual void onThrowItem(IGameNode* gnode, IGameNodeItem* item);
	virtual void onUseItem(IGameNode* gnode, IGameNodeItem* item);

	//////////////////////////////////

	IGameNode* _forcedLoadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file);

	void _createDropItemsForGameNode(IGameNode* game_node)
	{
		core::array<SItemDropTableLine>& drop_table = 
			IGameManager::getSingleton().getItemDropTable(game_node->getGameNodeType());
		s32 idx = drop_table.binary_search(SItemDropTableLine(game_node->getSceneNode()->getFileName()));
		if (idx!=-1)
		{     				
			SItemDropTableLine drop_table_line = drop_table[idx];  

			f32 drop_prob = core::math::RangeRandom(0,1000)/1000.0f;
			if (drop_table_line.DropProb>=drop_prob)
			{ 			
				f32 prob = core::math::RangeRandom(0,1000)/1000.0f;

				s32 drop_item_count = drop_table_line.Items.size();
				for (s32 i=0; i<drop_item_count; i++)
				{
					SItemDropTableCell& drop_item = drop_table_line.Items[i];  
					if (drop_item.Prob>=prob)
					{
						IGameNodeItem* item = (IGameNodeItem*)IGameManager::getSingleton().loadGameNode(
							EGNT_ITEM, drop_item.ItemFileName.c_str()
							);
						if (item)
						{
							game_node->getInventory().addItem(item);						
							item->getSceneNode()->setParent( game_node->getSceneNode() );
							item->getSceneNode()->setPosition(drop_item.LocalPosition);						
							item->setAutoCreatedFlag(true);
							item->getSceneNode()->setVisible(false);
						}
						break;	
					}					
				}
			}
		} 
	}

	void _doGame ();

	CGameSceneLoader GameSceneLoader;
	CGameSceneSaver  GameSceneSaver;

	IGameNodeMainPlayer* CurrentMainPlayer;

	bool m_GameStopped;

	core::math::event_counter DynamicPSCounter, GamePSCounter, ScriptPSCounter;
	
	core::list<mm::ISound*> GameSoundTracks;

	core::stringc GameScriptFileName;	
	core::stringc GameSceneFileName;	

	f64 thisFrameTime, previousFrameTime;
	f64 thisDeltaTime, previousDeltaTime;	

	f32 RemainDeltaSeconds;	

	//! array of lists of all gamenodes grouped by type
	static core::list<IGameNode*>GameNodesByType[E_GAME_NODE_TYPE_COUNT];   
	//! a list of all gamenodes contained in the game
    static core::list<IGameNode*> GameNodes;  
	//! a list of all ai gamenodes contained in the game
    static core::list<IGameNodeAI*> GameNodesAI;

	s32 m_ActiveGameNodesCount;

	f32 SoundEffectsVolume, SoundTracksVolume;	

	struct SGameClassData
	{
		SGameClassData() 
			:
		ClassName("Noname Class"), 
		Alloc(0), Create(0), Clone(0), Load(0)
		{}

		core::stringc ClassName;
		IGameNode* (*Alloc )(scn::ISceneNode* n, SGameNodeParams &params);
		IGameNode* (*Create)(SGameNodeCommonParams &node_params, bool log_info);
		IGameNode* (*Clone )(IGameNode *game_node, bool log_info);
		IGameNode* (*Load  )(SGameNodeCommonParams &node_params, io::IReadFile* file);
	};

	core::array<SGameClassData> GameClassesData[E_GAME_NODE_TYPE_COUNT];

	SGameSceneProps GameSceneProps;

	bool m_NewGameSceneLoaded, m_DoGameCompleted;

	void (*m_CallbackOnShowMessage)(bool, const c8*, const c8*);
	void (*m_CallbackBeforeLoadScene)();
	void (*m_CallbackAfterLoadScene)();
	void (*m_CallbackBeforeSaveScene)();
	void (*m_CallbackAfterSaveScene)();
	void (*m_CallbackOnCreateNode)(IGameNode* node);
	void (*m_CallbackOnDeleteNode)(IGameNode* node);
	void (*m_CallbackOnDieNode)(IGameNode* node);
	void (*m_CallbackOnCollectItem)(IGameNode*, IGameNodeItem*);
	void (*m_CallbackOnThrowItem)(IGameNode*, IGameNodeItem*);
	void (*m_CallbackOnUseItem)(IGameNode*, IGameNodeItem*);

	core::array< SItemDropTableLine > m_ItemDropTables[E_GAME_NODE_TYPE_COUNT];

	u32 m_DoGameCount;

	u32 m_ProfileIds[E_GAME_PROFILE_TYPES_COUNT];

	dev::IProfiler &m_Profiler;

	f32 m_ShiftTimeSec, m_StopTimeSec;
};

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef CGameManagerHPP