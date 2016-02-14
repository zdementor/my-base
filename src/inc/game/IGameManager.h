//|-------------------------------------------------------------------------
//| File:        IGameManager.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef IGameManagerHPP
#define IGameManagerHPP
//----------------------------------------------------------------------------

#include <my_inc.h>
#include <IUnknown.h>
#include <img/SColor.h>
#include <vid/SMaterial.h>
#include <ISingleton.h>
#include <dyn/IDynamicObject.h>
#include <game/GameTasks.h>
#include <game/SGameSceneProps.h>

//----------------------------------------------------------------------------
namespace my {
namespace game {
//----------------------------------------------------------------------------

struct SItemDropTableCell
{
	SItemDropTableCell() 
		: ItemFileName(""), Prob(0.0f), LocalPosition(core::vector3df(0,0,0)) {}
	SItemDropTableCell(const c8* name, f32 probe, const core::vector3df& pos) 
		: ItemFileName(name), Prob(probe), LocalPosition(pos) {}

	core::stringc ItemFileName;  		
	f32 Prob;
	core::vector3df LocalPosition;
};

//----------------------------------------------------------------------------

struct SItemDropTableLine
{
	SItemDropTableLine() : GameNodeFileName(""), DropProb(0.0f) {}
	SItemDropTableLine(const c8* name) : GameNodeFileName(name), DropProb(0.0f) {}

	core::stringc GameNodeFileName;
	f32 DropProb;
	core::array <SItemDropTableCell > Items;

	bool operator < (const SItemDropTableLine& other) const
	{
		return (GameNodeFileName < other.GameNodeFileName);
	}
};

//----------------------------------------------------------------------------

//! Game Manager. Used as manager of all game scene nodes and games stuff
class __MY_GAME_LIB_API__ IGameManager :
	public IUnknown, public ISingleton<IGameManager>
{
public:

	//! retrieve game node by index
	virtual IGameNode* getGameNode(u32 idx) = 0;

	//! retrieve game nodes count
	virtual u32 getGameNodesCount() = 0;

	//! retrieve active game nodes count
	virtual u32 getActiveGameNodesCount() = 0;

	//! retrieve game node by type and index
	virtual IGameNode* getGameNode(E_GAME_NODE_TYPE gntype, u32 idx = 0) = 0;

	//! retrieve game nodes count by type
	virtual u32 getGameNodesCount(E_GAME_NODE_TYPE gntype) = 0;

	//! retrieve game node type and index
	virtual E_GAME_NODE_TYPE getGameNodeType(IGameNode* game_node) = 0;
	virtual u32              getGameNodeIndex(IGameNode* game_node) = 0;
	virtual u32 getGameNodeTypeAndIndex(IGameNode* game_node,
		E_GAME_NODE_TYPE *gntype = NULL, u32 *idx=NULL) = 0;

	//! apply scenenodes common parameters to the selected scenenode
	virtual void applyGameNodeCommonParams(scn::ISceneNode *node,
		SGameNodeCommonParams &node_params) = 0;

	//! list of lists of all gamenodes grouped by type
	virtual const core::list<IGameNode*>& getGameNodesByType(E_GAME_NODE_TYPE gntype) = 0;

	//! a list of all gamenodes contained in the game
    virtual const core::list<IGameNode*>& getGameNodes() = 0;

	//! a list of all ai gamenodes contained in the game
    virtual const core::list<IGameNodeAI*>& getGameNodesAI() = 0;

	//! Adds Game Units formation 
    virtual IGameUnitsFormation* addGameUnitsFormation(core::dimension2di dim, s32 formation_id) = 0;

	//! Adds Game Units formation 
    virtual IGameUnitsFormation* getGameUnitsFormationFromID(s32 formation_id) = 0;

    //! Adds Game node to the scene graph.
    virtual IGameNode* addGameNode(SGameNodeCommonParams &node_params, scn::ISceneNode* scene_node) = 0;

    //! add new game node to scene node
    virtual IGameNode* addGameNodeForSceneNode(scn::ISceneNode* n, SGameNodeParams &params)=0;

    //! destroy game node object for scene node
	//! (return true on success, otherwise false)
    virtual bool removeGameNodeForSceneNode(scn::ISceneNode* n) = 0;

	//! Creating Game node (return new created game node)
	virtual IGameNode* createGameNode(E_GAME_NODE_TYPE game_node_type, scn::E_SCENE_NODE_TYPE scene_node_type, bool log_info = false) = 0;

	//! Creating Game node (return new created game node)
    virtual IGameNode* createGameNode(SGameNodeCommonParams &node_params, bool log_info = false) = 0;

    //! Cloning Game node (return new created game node)
    virtual IGameNode* cloneGameNode(IGameNode *game_node, bool log_info = false) = 0;

    //! reloads Game node
    virtual IGameNode* reloadGameNode(IGameNode* gn) = 0;

    //! load Game node from the file.
	virtual IGameNode* loadGameNode(const c8* fname) = 0;
	virtual IGameNode* loadGameNode(game::E_GAME_NODE_TYPE gntype, const c8* fname) = 0;
	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, const c8* fname) = 0;
	virtual IGameNode* loadGameNode(SGameNodeCommonParams &node_params, io::IReadFile* file) = 0;  

	//! loading game scene from file
	virtual void loadGameSceneLater(const c8* filename) = 0;

	//! load game scene from file
	virtual bool loadGameScene(const c8* filename) = 0;

	//! saving scene to xml-file
	virtual bool saveGameScene(const c8* filename = NULL) = 0;

	//! clear all game scene
	virtual void clearGameScene(bool clear_resourses=true) = 0;

	virtual const c8* getGameSceneFileName() = 0;

	//! setting current main player
	virtual IGameNodeMainPlayer* setCurrentMainPlayer(s32 idx) = 0;

	//! getting main players count
	virtual s32 getMainPlayersCount() = 0;

	//! getting current main player game node object
	virtual IGameNodeMainPlayer* getMainPlayerGameNode() = 0;

	//! getting current main player scene node object
	virtual scn::ISceneNode* getMainPlayerSceneNode() = 0;

	//! getting current main player dynamic object
	virtual dyn::IDynamicObject* getMainPlayerDynamicObject() = 0;

	//! getting current main player camera
	virtual scn::ICameraSceneNode* getMainPlayerCamera() = 0;

	//! choosing game node from screen coordinates
    virtual IGameNode* getGameNodeFromScreenCoordinates(const core::position2di &scr_pos) = 0;

	//! stopping the game
	virtual void stopGame() = 0;

	//! resuming the game
	virtual void startGame() = 0;

	//! do game step
	virtual void doGame() = 0;

	//! called each time before render frame
	virtual void preRenderFrame() = 0;

	//! called each time after render frame
	virtual void postRenderFrame() = 0;

	//! show/hide wait window
	virtual void showMessageBox(
		bool show, const c8 *caption="", const c8 *message="") = 0;

    //! returns current dynamic calculation per second value
    virtual s32 getDynamicPS() = 0;

    //! returns current game calculation per second value
    virtual s32 getGamePS() = 0;

	//! returns current script parsing per second value
    virtual s32 getScriptPS() = 0;

	//! return true if game is started, otherwise false
	virtual bool isGameStarted()=0;

	//! setting file name of game script
	virtual void setGameScriptFileName(core::stringc &scr_fname) = 0;

	//! Return file name of game script
	virtual core::stringc getGameScriptFileName() = 0;

	//! adding game sound track
	virtual void addSoundTrack(core::stringc &snd_fname) = 0;

	//! removing game sound tracks
	virtual void removeAllSoundTracks() = 0;
		
	//! return game sound track object by its index
	virtual mm::ISound* getSoundTrack(s32 idx) = 0;

	//! return overal count of game sound tracks
	virtual s32 getSoundTracksCount() = 0;

	//! starts playing sound track
	virtual bool playSoundTrack(s32 idx) = 0;

	//! paused playing sound track
	virtual bool pauseSoundTrack(s32 idx) = 0;

	//! stopps all playing sound tracks
	virtual void stopAllSoundTracks() = 0;

	//! setting sound tracks volume (0.0f - 1.0f) 
	virtual void setSoundTracksVolume(f32 volume) = 0;

	//! getting sound tracks volume (0.0f - 1.0f)
	virtual f32 getSoundTracksVolume() = 0;

	//! setting sound effects volume (0.0f - 1.0f)
	virtual void setSoundEffectsVolume(f32 volume) = 0;

	//! getting sound effects volume (0.0f - 1.0f)
	virtual f32 getSoundEffectsVolume() = 0;

	//! creating game inventory for node
	virtual IGameInventory* createGameInventory(game::IGameNode *owner) = 0;

	//! getting game node params
	virtual SGameNodeParams getGameNodeParams(IGameNode *gnode) = 0;

	//! setting game node params
	virtual void applyGameNodeParams(IGameNode *gnode, SGameNodeParams &params) = 0;

	//! registering game node class
	virtual bool registerGameNodeClass(
		c8 *class_name,
		E_GAME_NODE_TYPE game_node_type,
		IGameNode* (*alloc_func )(scn::ISceneNode* n, SGameNodeParams &params),
		IGameNode* (*create_func)(SGameNodeCommonParams &node_params, bool log_info),
		IGameNode* (*clone_func )(IGameNode *game_node, bool log_info),
		IGameNode* (*load_func  )(SGameNodeCommonParams &node_params, io::IReadFile* file)
		) = 0;

	virtual IPathFindTask* createPathFindTask() = 0;

	virtual const SGameSceneProps& getGameSceneProps() = 0; 

	virtual void setGameSceneProps(const SGameSceneProps& props) = 0; 

	virtual core::array<SItemDropTableLine>& getItemDropTable(E_GAME_NODE_TYPE type) = 0;

	//////////////////////////////////
	//!      game events
	//////////////////////////////////

	//! calling on deleting game node
	virtual void onDeleteNode(IGameNode* gnode) = 0;

	//! calling on creating game node
	virtual void onCreateNode(IGameNode* gnode) = 0;

	//! calling when some game node died
	virtual void onDieNode(IGameNode* gnode) = 0;

	//! calling on collecting item
	virtual void onCollectItem(IGameNode* gnode, IGameNodeItem* item) = 0;

	//! calling on throwing item
	virtual void onThrowItem(IGameNode* gnode, IGameNodeItem* item) = 0;

	//! calling on use item
	virtual void onUseItem(IGameNode* gnode, IGameNodeItem* item) = 0;

	//////////////////////////////////
	//!      game callbacks
	//////////////////////////////////

	//! setting callback, needed to show/hide wait window by external GUI
	virtual void setCallbackOnShowMessage(
		void (*show_wait_window_ptr)(bool, const c8*, const c8*) ) = 0;

	//! setting callback, needed to handle before game scene load event
	virtual void setCallbackBeforeLoadScene( void (*before_load_scene_ptr)() ) = 0;

	//! setting callback, needed to handle after game scene load event
	virtual void setCallbackAfterLoadScene( void (*after_load_scene_ptr)() ) = 0;  	

	//! setting callback, needed to handle on game node delete event
	virtual void setCallbackOnDeleteNode( void (*on_delete_node_ptr)(IGameNode*) ) = 0;

	//! setting callback, needed to handle on game node create event
	virtual void setCallbackOnCreateNode( void (*on_create_node_ptr)(IGameNode*) ) = 0;

	//! setting callback, needed to handle on game node die event
	virtual void setCallbackOnDieNode( void (*on_die_node_ptr)(IGameNode*) ) = 0;

	//! setting callback, needed to handle item collect event
	virtual void setCallbackOnCollectItem( void (*collect_item_ptr)(IGameNode*, IGameNodeItem*) ) = 0;

	//! setting callback, needed to handle item throw event
	virtual void setCallbackOnThrowItem( void (*throw_item_ptr)(IGameNode*, IGameNodeItem*) ) = 0;

	//! setting callback, needed to handle item use event
	virtual void setCallbackOnUseItem( void (*use_item_ptr)(IGameNode*, IGameNodeItem*) ) = 0;

	//////////////////////////////////
	//!      game functions
	//////////////////////////////////

	//! return true if trigger activated
	virtual bool IsTriggerActivated(s32 id) = 0;

	//! return true if door activated
	virtual bool IsDoorActivated(s32 id) = 0;

	//! one game node hited by another
	virtual int GameNodeHit(IGameNode *victim, IGameNode *agressor, s32 hit_power) = 0;

	//! game node death
	virtual int GameNodeDie(IGameNode *victim) = 0;

	//! game node simple move
	virtual void GameNodeMove(IGameNode *moving_node, core::vector3df dest_pos) = 0;

	//! game node movewith complex trajectory
	virtual void GameNodeMoveTrajectory(IGameNode *moving_node, core::array<core::vector3df> &way_points) = 0;	

	//////////////////////////////////

	//!	Return singleton object
	static IGameManager& getSingleton();

	//! Return pointer to singleton object
	static IGameManager* getSingletonPtr();	
};

//----------------------------------------------------------------------------

__MY_GAME_LIB_API__ IGameManager* createGameManager();

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef IGameManagerHPP