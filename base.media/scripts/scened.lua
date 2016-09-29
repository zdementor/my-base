
require "helper"

Scened =
{
	start,
	stop,
	update,
	updateMenuItemSaveScene,
	updateControls,
	isStarted,
	changeState,
	showPanel,
	hidePanel,
	close,
	onEvent,
	startStopGame,
	startGame,
	stopGame,
	suspendResumeTimer,
	suspendTimer,
	resumeTimer,
	reloadMaterials,
	setStartStopCallback,
	getWorkCamera,
}

SCENED_IMAGE_OPEN_FILE_FILTER =
{
	{Descr = "TGA images",           Value = "tga"    },
	{Descr = "JPEG images",          Value = "jpg"    },
	{Descr = "BMP images",           Value = "bmp"    },
	{Descr = "PNG images",           Value = "png"    },
	{Descr = "Direct Draw surfaces", Value = "dds"    },
	["tga"]	= {Descr = "TGA images",           Value = "tga"    },
	["jpg"]	= {Descr = "JPEG images",          Value = "jpg"    },
	["bmp"]	= {Descr = "BMP images",           Value = "bmp"    },
	["png"]	= {Descr = "PNG images",           Value = "png"    },
	["dds"]	= {Descr = "Direct Draw surfaces", Value = "dds"    },
}
SCENED_TEXTURE_OPEN_FILE_FILTER = {}
for k,v in pairs(SCENED_IMAGE_OPEN_FILE_FILTER) do SCENED_TEXTURE_OPEN_FILE_FILTER[k] = v end
table.insert(SCENED_TEXTURE_OPEN_FILE_FILTER, {Descr = "Animated images", Value = "atex"})
SCENED_TEXTURE_OPEN_FILE_FILTER["atex"] = {Descr = "Animated images", Value = "atex"}

SCENED_IMAGE_SAVE_FILE_FILTER =
{
	{Descr = "TGA images",           Value = "tga"    },
	{Descr = "JPEG images",          Value = "jpg"    },
	{Descr = "BMP images",           Value = "bmp"    },
	{Descr = "Direct Draw surfaces", Value = "dds"    },
	["tga"]	= {Descr = "TGA images",           Value = "tga"    },
	["jpg"]	= {Descr = "JPEG images",          Value = "jpg"    },
	["bmp"]	= {Descr = "BMP images",           Value = "bmp"    },
	["dds"]	= {Descr = "Direct Draw surfaces", Value = "dds"    },
}
SCENED_TEXTURE_SAVE_FILE_FILTER = {}
for k,v in pairs(SCENED_IMAGE_SAVE_FILE_FILTER) do SCENED_TEXTURE_SAVE_FILE_FILTER[k] = v end

SCENED_ANIMATED_MESH_FILE_FILTER =
{
	[1]		= { Descr="MD2 meshes",		Value="md2" },
	[2]		= { Descr="3DS meshes",		Value="3ds" },
	[3]		= { Descr="CAL3D meshes",	Value="cfg" },
	["md2"]	= { Descr="MD2 meshes",		Value="md2" },
	["3ds"]	= { Descr="3DS meshes",		Value="3ds" },
	["cfg"]	= { Descr="CAL3D meshes",	Value="cfg" },
}

require "scened/scened_controller"

local _ScenedInitialized = false
local _ScenedOK = false

local _ScenedStartStopCallback = nil

local _ScenedFirstStartup = true

function _ScenedInit()

	if _ScenedInitialized then
		return _ScenedOK
	end
	_ScenedInitialized = true
	LOG_INFO("Initializing Scened . . .")
	MyLogger:increaseFormatLevel()	
	
	_ScenedInitImpl()

	LOG_INFO("Scened initialized")
	MyLogger:decreaseFormatLevel()
	_ScenedOK = true
	return _ScenedOK
end

local _ScenedCameraInitial = nil
local _ScenedCameraMaya = nil

function _ScenedStart()
	if not _ScenedInit() then
		return
	end
	if MyScnMgr:isSceneEditorStarted() then
		LOG_WARN("Scene Editor already started")
		return
	end
	
	MyScnMgr:onStartSceneEditor()
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_CREATE_SCENE_NODE,		"_ScenedOnCreateSceneNode")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_DELETE_SCENE_NODE,		"_ScenedOnDeleteSceneNode")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_SELECT_SCENE_NODE,		"_ScenedOnSelectSceneNode")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_DESELECT_SCENE_NODE,	"_ScenedOnDeselectSceneNode")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_BEFORE_LOAD_SCENE,		"_ScenedOnBeforeLoadScene")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_AFTER_LOAD_SCENE,		"_ScenedOnAfterLoadScene")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_BEFORE_SAVE_SCENE,		"_ScenedOnBeforeSaveScene")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_AFTER_SAVE_SCENE,		"_ScenedOnAfterSaveScene")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_GAME_STARTED,			"_ScenedOnGameStarted")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_GAME_STOPPED,			"_ScenedOnGameStopped")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_ANIMATION_BEGIN,		"_ScenedOnAnimationBegin")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_ANIMATION_END,			"_ScenedOnAnimationEnd")

	_ScenedCameraInitial = MyScnMgr:getActiveCamera()
	_ScenedCameraMaya = MyScnMgr:addCameraSceneNodeMaya()
	_ScenedCameraMaya:setSceneEditorCamera(true)
	_ScenedCameraMaya:lockObject()

	local camera = _ScenedCameraInitial
	if camera ~= nil then
		_ScenedCameraMaya:setPosition(camera:getPosition())
		_ScenedCameraMaya:setTarget(camera:getTarget())
		_ScenedCameraMaya:updateAbsoluteTransformation()
	end
	MyScnMgr:setActiveCamera(_ScenedCameraMaya)

	_ScenedCameraMaya:setFarValue(15000.0)

	_ScenedStartImpl()

	if _ScenedFirstStartup then
		_ScenedFirstStartup = false
		Scened.showPanel()
	end

	if _ScenedStartStopCallback ~= nil then
		_ScenedStartStopCallback(true)
	end

end
Scened.start = _ScenedStart

function _ScenedStop()
	if not _ScenedInit() then
		return
	end
	if not MyScnMgr:isSceneEditorStarted() then
		LOG_WARN("There is no Scene Editor to stop")
		return
	end

	MyScnMgr:clearSelectionList()
	
	_ScenedStopImpl()
	
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_CREATE_SCENE_NODE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_DELETE_SCENE_NODE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_SELECT_SCENE_NODE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_DESELECT_SCENE_NODE,	"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_BEFORE_LOAD_SCENE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_AFTER_LOAD_SCENE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_AFTER_LOAD_SCENE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_BEFORE_SAVE_SCENE,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_GAME_STARTED,			"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_GAME_STOPPED,			"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_ANIMATION_BEGIN,		"")
	MyScript:setScriptCallback(scr.ESCT_SCENE_EDITOR_ON_ANIMATION_END,			"")
	MyScnMgr:onStopSceneEditor()
	
	local camera = _ScenedCameraInitial
	_ScenedCameraInitial = nil
	
	_ScenedCameraMaya:unlockObject()
	MyScnMgr:addToDeletionQueue(_ScenedCameraMaya)
	_ScenedCameraMaya = nil

	if camera == nil then
		local player = MyGameMgr:getMainPlayerSceneNode()
		if player ~= nil then
			camera = player:getCamera()
		end
	end
	MyScnMgr:setActiveCamera(camera)
	
	if _ScenedStartStopCallback ~= nil then
		_ScenedStartStopCallback(false)
	end
end
Scened.stop = _ScenedStop

function _ScenedUpdate(timeMs)
	if not Scened.isStarted() then
		return
	end
	_ScenedUpdateImpl(timeMs)
end
Scened.update = _ScenedUpdate

function _ScenedUpdateControls()
	if not Scened.isStarted() then
		return
	end
	Scened.updateMenuItemSaveScene()
	_ScenedUpdateControlsImpl()
end
Scened.updateControls = _ScenedUpdateControls

function _ScenedIsStarted()
	return MyScnMgr:isSceneEditorStarted()
end
Scened.isStarted = _ScenedIsStarted

function _ScenedChangeState()
	if Scened.isStarted() then
		_ScenedChangeStateImpl()
	end	
end
Scened.changeState = _ScenedChangeState

function _ScenedShowPanel()
	if Scened.isStarted() then
		_ScenedShowPanelImpl()
	end
end
Scened.showPanel = _ScenedShowPanel

function _ScenedHidePanel()
	if Scened.isStarted() then
		_ScenedHidePanelImpl()
	end
end
Scened.hidePanel = _ScenedHidePanel

function _ScenedExit()
	if not Scened.isStarted() then
		return
	end
	_ScenedExitImpl()
	Scened.hidePanel()
end
Scened.exit = _ScenedExit

function _ScenedOnEvent(event)
	if not Scened.isStarted() then
		return false
	end
	if event.EventType == io.EET_KEY_INPUT_EVENT then
		if event.KeyInput.Key == io.EKC_F3 then
			if event.KeyInput.Event == io.EKIE_KEY_PRESSED_UP then
				Scened.startStopGame()
			end
			return true
		elseif event.KeyInput.Key == io.EKC_F4 then
			if event.KeyInput.Event == io.EKIE_KEY_PRESSED_UP then
				Scened.suspendResumeTimer()
			end
			return true
		elseif event.KeyInput.Key == io.EKC_F5 then
			if event.KeyInput.Event == io.EKIE_KEY_PRESSED_UP then
				Scened.reloadMaterials()
			end
			return true
		elseif event.KeyInput.Key == io.EKC_F8 then
			if event.KeyInput.Event == io.EKIE_KEY_PRESSED_UP then
				Scened.makeScreenShot()
			end
			return true
		elseif event.KeyInput.Control and event.KeyInput.Key == io.EKC_BACKSPACE then
			MyScnMgr:clearSelectionList()
			return true
		end
	elseif event.EventType == io.EET_MOUSE_INPUT_EVENT then
		if event.MouseInput.Event == io.EMIE_LMOUSE_PRESSED_DOWN or 
				event.MouseInput.Event == io.EMIE_LMOUSE_LEFT_UP then
			if event.MouseInput.Control then
				if event.MouseInput.Event == io.EMIE_LMOUSE_PRESSED_DOWN then
					local game_node = MyGameMgr:getGameNodeFromScreenCoordinates(
						MyCursor:getPosition())
					local scene_node = nil
					if game_node ~= nil then
						scene_node = game_node:getSceneNode()
					end
					if scene_node ~= nil and game_node ~= nil then
						local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
						local already_selected = false
						for i = 0, sel_nodes - 1 do
							if core.isPtrsEq(MyScnMgr:getSelectedSceneNode(i), scene_node) then
								already_selected = true
								break
							end								
						end
						if already_selected then
							MyScnMgr:remSceneNodeFromSelectionList(scene_node)
						else
							MyScnMgr:addSceneNodeToSelectionList(scene_node)
						end
					end
				end
				return true
			end
		end		
	end
	return false
end
Scened.onEvent = _ScenedOnEvent

function _ScenedStartStopGame()
	if MyGameMgr:isGameStarted() then
		Scened.stopGame()
	else
		Scened.startGame()
	end
end
Scened.startStopGame = _ScenedStartStopGame

function _ScenedStartGame()
	MyGameMgr:startGame()
end
Scened.startGame = _ScenedStartGame

function _ScenedStopGame()
	MyGameMgr:stopGame()
end
Scened.stopGame = _ScenedStopGame

function _ScenedSuspendResumeTimer()
	if MyTimer:isSuspended() then
		Scened.resumeTimer()
	else
		Scened.suspendTimer()
	end
end
Scened.suspendResumeTimer = _ScenedSuspendResumeTimer

function _ScenedSuspendTimer()
	local menuItem = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Menu.Options.Controls.SuspendResumeTimer"), "CEGUI::MenuItem")
	MyTimer:suspend()
	if menuItem ~= nil then
		menuItem:setText("Resume timer (F4)")
	end
end
Scened.suspendTimer = _ScenedSuspendTimer

function _ScenedResumeTimer()
	local menuItem = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Menu.Options.Controls.SuspendResumeTimer"), "CEGUI::MenuItem")
	MyTimer:resume()
	if menuItem ~= nil then
		menuItem:setText("Suspend timer (F4)")
	end
end
Scened.resumeTimer = _ScenedResumeTimer

function _ScenedReloadMaterials()
	if OPTIONS.CacheShaders == true then
		local tmp_dir = MyResMgr:getMediaDirFull(res.EMT_TEMP_DATA)
		MyDriver:loadGPUProgramsFromDir(tmp_dir, ShaderGen.getCurrentTag(),true)
	end
	local mat_dir = MyResMgr:getMediaDirFull(res.EMT_MATERIALS)
	MyMatMgr:loadMaterialsFromDir(mat_dir, true, true)
end
Scened.reloadMaterials = _ScenedReloadMaterials

function _ScenedSetStartStopCallback(callback)
	_ScenedStartStopCallback = callback
end
Scened.setStartStopCallback = _ScenedSetStartStopCallback

function _ScenedOnCreateSceneNode(arg)
	local scene_node = tolua.cast(arg, "scn::ISceneNode")
	local game_node = scene_node:getGameNode()
	--LOG_INFO("SCENE NODE "..tostring(scene_node).." "..
	--	scn.getSceneNodeTypeReadableName(scene_node:getSceneNodeType()))
end

function _ScenedOnDeleteSceneNode(arg)
	local scene_node = tolua.cast(arg, "scn::ISceneNode")
	if core.isPtrsEq(scene_node, _ScenedCameraInitial) then
		_ScenedCameraInitial = nil
	end
	--LOG_INFO("~SCENE NODE "..tostring(scene_node).." "..
	--	scn.getSceneNodeTypeReadableName(scene_node:getSceneNodeType()))
end

function _ScenedOnSelectSceneNode(arg)
	local scene_node = tolua.cast(arg, "scn::ISceneNode")
	_ScenedOnSelectSceneNodeImpl(scene_node)
end

function _ScenedOnDeselectSceneNode(arg)
	local scene_node = tolua.cast(arg, "scn::ISceneNode")
	_ScenedOnDeselectSceneNodeImpl(scene_node)
end

function _ScenedOnBeforeLoadScene()
end

local cam_pos = core.vector3df(0, 0, 0)
local cam_tgt = core.vector3df(0, 0, 0)

function _ScenedOnAfterLoadScene()
	local camera = Scened.getWorkCamera()
	local player = MyGameMgr:getMainPlayerSceneNode()
	if player ~= nil and camera ~= nil then
		cam_pos:set(player:getTransformedBoundingBox():getCenter())
		cam_tgt:set(cam_pos.X + 1.0, cam_pos.Y - 1.0, cam_pos.Z + 1.0)
		camera:setPosition(cam_pos)	
		camera:setTarget(cam_tgt)
		camera:updateAbsoluteTransformation()
	end
	MyGameMgr:stopGame()
	Scened.updateControls()
end

function _ScenedOnBeforeSaveScene()
end

local function _ScenedUpdateMenuItemSaveScene()
	local game_scene_name = MyGameMgr:getGameSceneFileName()
	local menuItem = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Menu.File.SceneSave"), "CEGUI::MenuItem")	
	if menuItem ~= nil then
		if game_scene_name == nil or game_scene_name == "noname game scene" then
			menuItem:disable()
		else
			menuItem:enable()
		end
	end
end
Scened.updateMenuItemSaveScene = _ScenedUpdateMenuItemSaveScene

function _ScenedOnAfterSaveScene()
	Scened.updateMenuItemSaveScene()
end

local function _ScenedGetWorkCamera()
	return _ScenedCameraMaya
end
Scened.getWorkCamera = _ScenedGetWorkCamera
