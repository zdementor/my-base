//|-------------------------------------------------------------------------
//| File:        ConstGame.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef my_const_gameHPP
#define my_const_gameHPP

//---------------------------------------------------------------------------
namespace my {
namespace game {
//---------------------------------------------------------------------------

//! enumaretion for all kinds of game nodes types
enum E_GAME_NODE_TYPE
{
    //! Main Player node
    EGNT_MAIN_PLAYER = 0,

    //! Person node
    EGNT_PERSON,

    //! Decoration node
    EGNT_DECORATION,

    //! Level Map
    EGNT_LEVEL_MAP,

	//! Terrain
	EGNT_TERRAIN,

    //! Sky Dome
    EGNT_SKY_DOME,

    //! Light 
    EGNT_LIGHT, 

    //! Trigger 
    EGNT_TRIGGER,

	//! Door
    EGNT_DOOR,

	//! Platform
    EGNT_PLATFORM,

    //! Button
    EGNT_BUTTON,

	//! Item
	EGNT_ITEM,

    //! Weapon
    EGNT_WEAPON,

    //! Units formation
    EGNT_UNITS_FORMATION,

    //! Unknown game node type
    EGNT_UNKNOWN,

    //! Not used, only for counting enumeration
    E_GAME_NODE_TYPE_COUNT,

	E_GAME_NODE_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameNodeTypeStr[E_GAME_NODE_TYPE_COUNT] = 
{  
    "EGNT_MAIN_PLAYER",
    "EGNT_PERSON",
    "EGNT_DECORATION",
	"EGNT_LEVEL_MAP",
	"EGNT_TERRAIN",
    "EGNT_SKY_DOME",
    "EGNT_LIGHT",    
    "EGNT_TRIGGER",
	"EGNT_DOOR",
	"EGNT_PLATFORM",
	"EGNT_BUTTON",
	"EGNT_ITEM",
	"EGNT_WEAPON",    
	"EGNT_UNITS_FORMATION",
    "EGNT_UNKNOWN"
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameNodeTypeShortStr[E_GAME_NODE_TYPE_COUNT] = 
{ 
    "Main Player", 
    "Person", 
    "Decoration", 
	"Level Map",
	"Terrain",
    "Sky Dome", 
    "Light", 
    "Trigger", 
	"Door",
	"Platform",
	"Button",
	"Item",
	"Weapon",
    "Units Formation",          
    "Unknown Node"
};

//----------------------------------------------------------------------------

static MY_INLINE const c8* getGameNodeTypeName(E_GAME_NODE_TYPE type)
{ return GameNodeTypeStr[type]; }

static MY_INLINE const c8* getGameNodeTypeReadableName(E_GAME_NODE_TYPE type)
{ return GameNodeTypeShortStr[type]; }

//----------------------------------------------------------------------------

//! enumeration for all kind of game triigers
enum E_GAME_TRIGGER_TYPE
{
    EGTT_TRAP =0,
    EGTT_PUSH,

    E_GAME_TRIGGER_TYPE_COUNT,

	E_GAME_TRIGGER_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameTriggerTypeStr[E_GAME_TRIGGER_TYPE_COUNT] = 
{  
    "EGTT_TRAP", 
	"EGTT_PUSH"
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameTriggerTypeShortStr[E_GAME_TRIGGER_TYPE_COUNT] = 
{  
    "Trap Trigger", 
	"Push Trigger"
};

//----------------------------------------------------------------------------

enum E_GAME_AI_EVENT_TYPE
{
	EGAIET_ENEMY_VISUAL_CONTACT = 0,
	EGAIET_ENEMY_SOUND_CONTACT,
	EGAIET_ENEMY_PHYSIC_CONTACT,

	EGAIET_FRIEND_VISUAL_CONTACT,
	EGAIET_FRIEND_SOUND_CONTACT,
	EGAIET_FRIEND_PHYSIC_CONTACT,

	E_GAME_AI_EVENT_TYPE_COUNT,

	E_GAME_AI_EVENT_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_GAME_EVENT_TYPE
{
	EGET_SOUND=0,
	EGET_PHYSIC,

	E_GAME_EVENT_TYPE_COUNT,

	E_GAME_EVENT_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! enumeration for all kinds of game actions
enum E_GAME_TASK_TYPE
{
	EGTT_LOAD_SCENE=0,
	EGTT_ACTIVATE,
	EGTT_ATTACK,
	EGTT_HITED,	
	EGTT_DIE,
	EGTT_FIND_PATH_AND_MOVE,
	EGTT_ORIENT,

	E_GAME_TASK_TYPE_COUNT,

	E_GAME_TASK_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! Defines player states
enum EPLAYER_STATE
{   
	EPS_RUNNING=0,
    EPS_STANDING,
    EPS_JUMPING,

	EPLAYER_STATE_COUNT,

	EPLAYER_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_OBJECT_ACTIVATE_MODE
{
	EOAM_AUTOMATIC = 0,
	EOAM_BY_BUTTON,
	EOAM_BY_KEY,

	E_OBJECT_ACTIVATE_MODE_COUNT,

	E_OBJECT_ACTIVATE_MODE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_AI_STATE
{
	EAIS_IDLE = 0,	
	EAIS_SEEKING,	
	EAIS_ATTACKING,
	EAIS_FINISHING_ATTACK,
	EAIS_FINISHED_ATTACK,
	EAIS_WAIT_A_TURN,
	EAIS_DYING,
	EAIS_DEAD,

	E_AI_STATE_COUNT,

	E_AI_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_BUTTON_STATE
{
	EBS_UNPUSHED = 0,
	EBS_PUSHING,
	EBS_WAITING,
	EBS_UNPUSHING,

	E_BUTTON_STATE_COUNT,

	E_BUTTON_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_DOOR_STATE
{
	EDS_CLOSED = 0,
	EDS_OPENING,
	EDS_WAITING,
	EDS_CLOSING,

	E_DOOR_STATE_COUNT,

	E_DOOR_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

enum E_GAME_ITEM_TYPE
{
	EGIT_MEDPACK = 0,
	EGIT_ARMOR,
	EGIT_KEY,
	EGIT_WEAPON,
	EGIT_AMMO,

	E_GAME_ITEM_TYPE_COUNT,

	E_GAME_ITEM_TYPE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameItemTypeStr[E_GAME_ITEM_TYPE_COUNT] = 
{  
	"EGIT_MEDPACK",
	"EGIT_ARMOR",
	"EGIT_KEY",
	"EGIT_WEAPON",
	"EGIT_AMMO"
};

//----------------------------------------------------------------------------

//! game nodes names as string
static const c8* GameItemTypeShortStr[E_GAME_ITEM_TYPE_COUNT] = 
{ 
	"Medic Pack",
	"Armor",
	"Key",
	"Weapon",
	"Ammo"
};

//----------------------------------------------------------------------------

enum E_PLATFORM_STATE
{
	EPS_WAITING_AT_START = 0,
	EPS_MOVING_FORWARD,
	EPS_MOVING_BACKWARD,
	EPS_WAITING_AT_END,

	E_PLATFORM_STATE_COUNT,

	E_PLATFORM_STATE_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! The three formation states.
enum E_FORMATION_STATES
{
	EFS_BROKEN=0,
	EFS_FORMING,
	EFS_FORMED,

	E_FORMATION_STATES_COUNT,

	E_FORMATION_STATES_FORCE_32BIT = 0xffffffff
};

//----------------------------------------------------------------------------

//! enumeration for actions. Used for example in the FPS Camera.
enum E_GAME_ACTION
{
    EGA_MOVE_FORWARD = 0,        // go forward
    EGA_MOVE_BACKWARD,           // go backward
    EGA_MOVE_LEFT_STRAFE,        // strafe left
    EGA_MOVE_RIGHT_STRAFE,       // strafe right
 	EGA_VIEW_ZOOM_IN,            // zoom in
	EGA_VIEW_ZOOM_OUT,           // zoom out
	EGA_ATTACK_PRIMARY,          // 
	EGA_SELECT_PREV_WEAPON,      // prev weapon
	EGA_SELECT_NEXT_WEAPON,      // next weapon
	EGA_SELECT_WEAPON_0,         // weapon 0
	EGA_SELECT_WEAPON_1,         // weapon 1
	EGA_SELECT_WEAPON_2,         // weapon 2
	EGA_ENTER_INVENTORY,         // inventory

    E_GAME_ACTION_COUNT,

    //! This value is not used. It only forces this enumeration to compile in 32 bit. 
    E_GAME_ACTION_FORCE_32BIT = 0x7fffffff    
};

//---------------------------------------------------------------------------

// game actions type names  as string
static const c8* GameActionNameStr[E_GAME_ACTION_COUNT] = 
{  
    "Forward",
    "Backward",
    "Left (Strafe)",
    "Right (Strafe)",      
	"Zoom In",
	"Zoom Out",
	"Attack Prim.",
	"Prev Weapon",
	"Next Weapon",
	"Weapon 0",
	"Weapon 1",
	"Weapon 2",
	"Inventory"
};

//!---------------------------------------------------------------------------

enum E_WAR_SIDE
{
	EWS_FRIEND  = 0x00000001,
	EWS_ENEMY   = 0x00000003,
	EWS_NEUTRAL = 0,

	//! This value is not used. It only forces this enumeration to compile in 32 bit. 
	E_WAR_SIDE_FORCE_32BIT = 0x7fffffff   
};

//----------------------------------------------------------------------------

//! 
static const c8* GameNodeCorePropertiesTagValue		= "GameNodeCoreProperties";

MY_INLINE const c8* getGameNodeCorePropertiesTagValue()
{ return GameNodeCorePropertiesTagValue; }

//----------------------------------------------------------------------------
} // end namespace game
} // end namespace my
//----------------------------------------------------------------------------

#endif // #ifndef my_const_gameHPP