
require "scened/scened_new_node_controller"
require "scened/scened_transf_controller"
require "scened/scened_props_controller"
require "scened/scened_material_controller"
require "scened/scened_scn_animated_mesh_controller"
require "scened/scened_scn_ktree_controller"
require "scened/scened_scn_particle_system_controller"
require "scened/scened_scn_billboard_controller"
require "scened/scened_scn_light_controller"
require "scened/scened_scn_lensflare_controller"
require "scened/scened_scn_terrain_controller"
require "scened/scened_scn_sky_controller"

local _Ctrls =
{
	Menubar = nil,
	NodeExplorer =
	{
		Root = nil,
		ClientArea = nil,
	},
	StatusInfoText	= nil,
	NewNodeDialog	= nil,
	NodeTypeCombo	= nil,
	NodeNameCombo	= nil,
	Buttons =
	{
		NewNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-new"		},
		SaveNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save"		},
		SaveAsNodeButton= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save-as"	},
		OpenNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"		},
		ReloadNodeButton= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh"		},
		CloneNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-copy"			},
		EditNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-xml-text"		},
		DropNodeButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-delete"		},
		ShowButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-bottom"			},
		HideButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-top"				},
	},
	MenuItems =
	{},
	Layouts =
	{},
	SceneNodeTabControl = nil,
	CommonTabControl = nil,
	ClientArea = nil,
}

local SCENED_STATE_APPEARING	= 0
local SCENED_STATE_VISIBLE		= 1
local SCENED STATE_DISAPPEARING	= 2
local SCENED_STATE_INVISIBLE	= 3

local _ScenedState = SCENED_STATE_INVISIBLE
local _ScenedStateNext = SCENED_STATE_INVISIBLE
local _ScenedAppearTimeMs = 500
local _ScenedStartStateChangingTimeMs = 0
local _ScenedStateChangeProgrees = 0.0
local _ScenedWantToExit = false

local SCENED_MENU_NAME_SHOW_SHADOW_DEBUG_DATA = "Show shadows debug data"
local SCENED_MENU_NAME_HIDE_SHADOW_DEBUG_DATA = "Hide shadows debug data"
local SCENED_MENU_NAME_SHOW_LIGHTS = "Show lights"
local SCENED_MENU_NAME_HIDE_LIGHTS = "Hide lights"
local SCENED_MENU_NAME_SHOW_TERRAIN_NORMALS = "Show terrain normals"
local SCENED_MENU_NAME_HIDE_TERRAIN_NORMALS = "Hide terrain normals"

function _ScenedInitImpl()

	CEGUISchemeMgr:create(OPTIONS.ScenedGUI.Scheme, OPTIONS.ScenedGUI.ResourceGroup)
	CEGUISchemeMgr:create(OPTIONS.ScenedGUI.SchemeAliases, OPTIONS.ScenedGUI.ResourceGroup)
	
	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	CEGUIWinMgr:loadWindowLayout("myengine_scened_main.layout", "", "CommonLayouts")

	_Ctrls.SceneNodeTabControl = tolua.cast(CEGUIWinMgr:getWindow("Scened.SceneNodeTabControl"), "CEGUI::TabControl")
	_Ctrls.CommonTabControl = tolua.cast(CEGUIWinMgr:getWindow("Scened.CommonTabControl"), "CEGUI::TabControl")
	_Ctrls.ClientArea = CEGUIWinMgr:getWindow("Scened.ClientArea")
	
	_Ctrls.Menubar = tolua.cast(CEGUIWinMgr:getWindow("Scened.Menu"), "CEGUI::Menubar")	
	_Ctrls.NodeExplorer.Root = CEGUIWinMgr:getWindow("Scened.NodeExplorer")
	_Ctrls.NodeExplorer.ClientArea = CEGUIWinMgr:getWindow("Scened.ClientArea")
	_Ctrls.StatusInfoText = CEGUIWinMgr:getWindow("Scened.StatusInfoText")
	_Ctrls.NodeTypeCombo = tolua.cast(CEGUIWinMgr:getWindow("Scened.NodeTypeCombo"), "CEGUI::Combobox")
	_Ctrls.NodeNameCombo = tolua.cast(CEGUIWinMgr:getWindow("Scened.NodeNameCombo"), "CEGUI::Combobox")
	
	_Ctrls.NodeTypeCombo:resetList()
	for i = 0, game.E_GAME_NODE_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.NodeTypeCombo, OPTIONS.ScenedGUI.SchemeName,
			game.getGameNodeTypeReadableName(i), i, nil)
	end

	_Ctrls.NodeTypeCombo:subscribeEvent("ListSelectionAccepted", "_ScenedComboHandler")
	_Ctrls.NodeNameCombo:subscribeEvent("ListSelectionAccepted", "_ScenedComboHandler")
	
	_Ctrls.NodeNameCombo:setSortingEnabled(false)
	
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Menubar)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.NodeExplorer.Root)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.StatusInfoText)
	
	_Ctrls.Menubar:setVisible(false)
	_Ctrls.NodeExplorer.Root:setVisible(false)
	_Ctrls.StatusInfoText:setVisible(false)
	
	_Ctrls.StatusInfoText:setText("")

	table.insert(_Ctrls.Layouts, _ScenedInitTransfControls())
	table.insert(_Ctrls.Layouts, _ScenedInitPropsControls())
	table.insert(_Ctrls.Layouts, _ScenedInitMaterialControls())
	table.insert(_Ctrls.Layouts, _ScenedInitAnimatedMeshControls())
	table.insert(_Ctrls.Layouts, _ScenedInitKTreeControls())
	table.insert(_Ctrls.Layouts, _ScenedInitParticleSystemControls())
	table.insert(_Ctrls.Layouts, _ScenedInitBillboardControls())
	table.insert(_Ctrls.Layouts, _ScenedInitLightControls())
	table.insert(_Ctrls.Layouts, _ScenedInitLensFlareControls())
	table.insert(_Ctrls.Layouts, _ScenedInitTerrainControls())
	table.insert(_Ctrls.Layouts, _ScenedInitSkyControls())
	
	local new_node_layout = _ScenedInitNewNodeControls()
	table.insert(_Ctrls.Layouts, new_node_layout)
	_Ctrls.NewNodeDialog = new_node_layout.Ctrl
	
	Helper.GUI.initButtons("Scened.", _Ctrls.Buttons, _ScenedWidgetClicked)

	_Ctrls.Menubar:setItemSpacing(5)
	
	-- File
	local popupMenu = tolua.cast(CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.FileMenu"), "CEGUI::PopupMenu")
	
	local menuItem = CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.File.SceneReset")
	menuItem:setText("Reset scene")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)
	
	menuItem = CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.File.SceneOpen")
	menuItem:setText("Open scene")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)
	
	menuItem = CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.File.SceneSave")
	menuItem:setText("Save scene")
	menuItem:disable()
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)
	
	menuItem = CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.File.SceneSaveAs")
	menuItem:setText("Save scene as...")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)
	
	menuItem = tolua.cast(CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.File"), "CEGUI::MenuItem")
	menuItem:setText("File")
	menuItem:setPopupMenu(popupMenu)
	_Ctrls.Menubar:addChildWindow(menuItem)		

	-- Actions
	
	popupMenu = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.ActionsMenu"),
		"CEGUI::PopupMenu")
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Actions"),
		"CEGUI::MenuItem")
	menuItem:setText("Actions")
	menuItem:setPopupMenu(popupMenu)
	_Ctrls.Menubar:addChildWindow(menuItem)	
	
	-- Actions->Reload materials
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Actions.ReloadMaterials"),
		"CEGUI::MenuItem")
	menuItem:setText("Reload materials (F5)")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)

	-- Tools
	
	popupMenu = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.ToolsMenu"),
		"CEGUI::PopupMenu")
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Tools"),
		"CEGUI::MenuItem")
	menuItem:setText("Tools")
	menuItem:setPopupMenu(popupMenu)
	_Ctrls.Menubar:addChildWindow(menuItem)	
	
	-- Tools->NormalMap converter
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Tools.NormalMapConverter"),
		"CEGUI::MenuItem")
	menuItem:setText("NormalMap converter...")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)

	-- Tools->ImageViewer
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Tools.ImageViewer"),
		"CEGUI::MenuItem")
	menuItem:setText("Image viewer...")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)

	-- Options
	
	popupMenu = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.OptionsMenu"),
		"CEGUI::PopupMenu")
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options"),
		"CEGUI::MenuItem")
	menuItem:setText("Options")
	menuItem:setPopupMenu(popupMenu)
	_Ctrls.Menubar:addChildWindow(menuItem)	
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.SceneProps"),
		"CEGUI::MenuItem")
	menuItem:setText("Scene properties...")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu:addChildWindow(menuItem)

	-- Options->Controls
	
	local popupMenu1 = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.Options.ControlsMenu"),
		"CEGUI::PopupMenu")
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.Controls.StartStopGame"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.StartStopGame = menuItem
	if MyGameMgr:isGameStarted() then
		menuItem:setText("Stop game (F3)")
	else
		menuItem:setText("Start game (F3)")
	end
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu1:addChildWindow(menuItem)
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.Controls.SuspendResumeTimer"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.SuspendResumeTimer = menuItem
	if not MyTimer:isSuspended() then
		menuItem:setText("Suspend timer (F4)")
	else
		menuItem:setText("Resume timer (F4)")
	end
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu1:addChildWindow(menuItem)
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.Controls"),
		"CEGUI::MenuItem")
	menuItem:setText("Controls")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	menuItem:setPopupMenu(popupMenu1)
	popupMenu:addChildWindow(menuItem)
	
	
	-- Options->View

	popupMenu1 = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.Options.ViewMenu"),
		"CEGUI::PopupMenu")
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.ShowHideShadowsDebugData"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.ShowHideShadowsDebugData = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu1:addChildWindow(menuItem)
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.ShowHideLights"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.ShowHideLights = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu1:addChildWindow(menuItem)
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.RenderTerrainNormals"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.RenderTerrainNormals = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu1:addChildWindow(menuItem)

	-- Options->View->PolygonMode
	local popupMenu2 = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/PopupMenu", "Scened.Menu.Options.View.PolygonModeMenu"),
		"CEGUI::PopupMenu")	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.PolygonMode.Solid"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.PolygonModeSolid = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu2:addChildWindow(menuItem)
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.PolygonMode.Wireframe"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.PolygonModeWireframe = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu2:addChildWindow(menuItem)
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.PolygonMode.Point"),
		"CEGUI::MenuItem")
	_Ctrls.MenuItems.PolygonModePoint = menuItem
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	popupMenu2:addChildWindow(menuItem)
	
	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View.PolygonMode"),
		"CEGUI::MenuItem")
	menuItem:setText("Polygon mode")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	menuItem:setPopupMenu(popupMenu2)
	popupMenu1:addChildWindow(menuItem)

	menuItem = tolua.cast(
		CEGUIWinMgr:createWindow("MyScened/MenuItem", "Scened.Menu.Options.View"),
		"CEGUI::MenuItem")
	menuItem:setText("View")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	menuItem:setPopupMenu(popupMenu1)
	popupMenu:addChildWindow(menuItem)

	-- Exit
	menuItem = tolua.cast(CEGUIWinMgr:createWindow(
		"MyScened/MenuItem", "Scened.Menu.Exit"), "CEGUI::MenuItem")
	menuItem:setText("Exit")
	menuItem:subscribeEvent("Clicked", "_ScenedWidgetClicked")
	_Ctrls.Menubar:addChildWindow(menuItem)

end

function _ScenedStartImpl()
	_ScenedWantToExit = false
	_ScenedSetVisible(true)
	Scened.updateControls()
end

function _ScenedStopImpl()
	_ScenedSetVisible(false)
	_Ctrls.NewNodeDialog:setVisible(false)
end

local _curYPos = CEGUI.UDim(0.0, 0.0)

function _ScenedUpdateImpl(timeMs)
	_ScenedState = _ScenedStateNext
	if _ScenedState == SCENED_STATE_APPEARING or
			_ScenedState == SCENED_STATE_DISAPPEARING then
		local nowMs = MyDevice:getDeviceSystemTime()
		_ScenedStateChangeProgrees = (nowMs - _ScenedStartStateChangingTimeMs) / _ScenedAppearTimeMs
		if _ScenedStateChangeProgrees >= 1.0 then
			_ScenedStateChangeProgrees = 1.0
		end
		if _ScenedState == SCENED_STATE_APPEARING then
			_curYPos.scale = -1.0 + _ScenedStateChangeProgrees
		else
			_curYPos.scale = -_ScenedStateChangeProgrees
		end
		if _curYPos.scale < -0.96 then
			_curYPos.scale = -0.96
		end
		_Ctrls.NodeExplorer.Root:setYPosition(_curYPos)
		if _ScenedStateChangeProgrees == 1.0 then
			if _ScenedState == SCENED_STATE_APPEARING then
				_ScenedState = SCENED_STATE_VISIBLE
				_Ctrls.Buttons.ShowButton.Ctrl:setVisible(false)
				_Ctrls.Buttons.HideButton.Ctrl:setVisible(true)
				local area = CEGUI.URect(_Ctrls.NodeExplorer.ClientArea:getArea().min, _Ctrls.NodeExplorer.ClientArea:getArea().max)
				area.max.y.scale = 1.0
				_Ctrls.NodeExplorer.ClientArea:setArea(area)
			else
				_ScenedState = SCENED_STATE_INVISIBLE
				_Ctrls.Buttons.ShowButton.Ctrl:setVisible(true)
				_Ctrls.Buttons.HideButton.Ctrl:setVisible(false)
			end
		end
	end
	_ScenedStateNext = _ScenedState
	if _ScenedWantToExit and _ScenedState == SCENED_STATE_INVISIBLE then
		Scened.stop()
	end
	
	for k, v in pairs(_Ctrls.Layouts) do
		v.update(timeMs)
	end
end

function _ScenedChangeStateImpl()
	_ScenedStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
	if _ScenedState == SCENED_STATE_INVISIBLE then
		_ScenedStateNext = SCENED_STATE_APPEARING
	elseif _ScenedState == SCENED_STATE_VISIBLE then
		_ScenedStateNext = SCENED_STATE_DISAPPEARING
	elseif _ScenedState == SCENED_STATE_APPEARING then
		_ScenedStartStateChangingTimeMs =
			_ScenedStartStateChangingTimeMs - _ScenedAppearTimeMs * (1.0 - _ScenedStateChangeProgrees)
		_ScenedStateNext = SCENED_STATE_DISAPPEARING
	elseif _ScenedState == SCENED_STATE_DISAPPEARING then
		_ScenedStartStateChangingTimeMs =
			_ScenedStartStateChangingTimeMs - _ScenedAppearTimeMs * (1.0 - _ScenedStateChangeProgrees)
		_ScenedStateNext = SCENED_STATE_APPEARING
	end
end

function _ScenedShowPanelImpl()
	if _ScenedState == SCENED_STATE_INVISIBLE then
		_ScenedStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
		_ScenedStateNext = SCENED_STATE_APPEARING
	elseif _ScenedState == SCENED_STATE_DISAPPEARING then
		_ScenedStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
		_ScenedStartStateChangingTimeMs =
			_ScenedStartStateChangingTimeMs - _ScenedAppearTimeMs * (1.0 - _ScenedStateChangeProgrees)
		_ScenedStateNext = SCENED_STATE_APPEARING
	end
end

function _ScenedHidePanelImpl()
	if _ScenedState == SCENED_STATE_VISIBLE then
		_ScenedStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
		_ScenedStateNext = SCENED_STATE_DISAPPEARING
	elseif _ScenedState == SCENED_STATE_APPEARING then
		_ScenedStartStateChangingTimeMs = MyDevice:getDeviceSystemTime()
		_ScenedStartStateChangingTimeMs =
			_ScenedStartStateChangingTimeMs - _ScenedAppearTimeMs * (1.0 - _ScenedStateChangeProgrees)
		_ScenedStateNext = SCENED_STATE_DISAPPEARING
	end
end

function _ScenedExitImpl()
	_ScenedWantToExit = true
end

function _ScenedUpdateNodeNameCombo()
	if _Ctrls.NodeNameCombo:getItemCount() ~= 0 then
		local sel_item = _Ctrls.NodeNameCombo:getSelectedItem()
		if sel_item ~= nil then
			_Ctrls.NodeNameCombo:setItemSelectState(sel_item, false)
		end
		_Ctrls.NodeNameCombo:resetList()		
	end
	local sel_item = _Ctrls.NodeTypeCombo:getSelectedItem()
	if sel_item ~= nil then
		local sel_idx = -1
		local game_node_sel = nil
		if MyScnMgr:getSelectedSceneNodesCount() == 1 then
			game_node_sel = MyScnMgr:getSelectedSceneNode(0):getGameNode()
		end
		local game_node_type = sel_item:getID()
		local game_node_count = MyGameMgr:getGameNodesCount(game_node_type)
		for i = 0, game_node_count - 1 do
			local game_node = MyGameMgr:getGameNode(game_node_type, i)
			if core.isPtrsEq(game_node, game_node_sel) then
				sel_idx = i
			end
			Helper.GUI.addListboxItem(_Ctrls.NodeNameCombo, OPTIONS.ScenedGUI.SchemeName,
				string.format("%d. - %s", i, game_node:getSceneNode():getName()), i, game_node)
		end
		if sel_idx ~= -1 then
			_Ctrls.NodeNameCombo:setItemSelectState(sel_idx, true)
		end
	end
end

function _ScenedUpdatePolygonModeMenuItems()
	local menuPolygonItemsTbl = {
		[vid.EPFM_SOLID     + 1] = _Ctrls.MenuItems.PolygonModeSolid,
		[vid.EPFM_WIREFRAME + 1] = _Ctrls.MenuItems.PolygonModeWireframe,
		[vid.EPFM_POINT     + 1] = _Ctrls.MenuItems.PolygonModePoint}
	for mode = vid.EPFM_SOLID, vid.E_POLYGON_FILL_MODE_COUNT - 1 do
		local menuItem = menuPolygonItemsTbl[mode + 1]
		menuItem:setText(vid.getPolygonFillModeReadableName(mode))
		if MyDriver:getPolygonFillMode() == mode then
			menuItem:setText(menuItem:getText().."*")
		end
	end
end

function _ScenedUpdateControlsImpl()

	local disableCol = "FF404040"
	local enableCol  = "FFFFFFFF"
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()

	if sel_nodes > 1 then
		local game_node0 = MyScnMgr:getSelectedSceneNode(0):getGameNode()
		local multi_types = false
		for i = 1, sel_nodes - 1 do
			local game_node = MyScnMgr:getSelectedSceneNode(i):getGameNode()
			if game_node0 ~= nil and game_node ~= nil then
				if game_node0:getGameNodeType() ~= game_node:getGameNodeType() then
					multi_types = true
					break
				end
			end
		end
		_Ctrls.NodeNameCombo:setText("multi nodes select...")
		_Ctrls.NodeNameCombo:getEditbox():setProperty("NormalTextColour", disableCol)
		_Ctrls.NodeNameCombo:disable()
		local sel_item = _Ctrls.NodeTypeCombo:getSelectedItem()
		if sel_item ~= nil then
			_Ctrls.NodeTypeCombo:setItemSelectState(sel_item, false)
		end
		if multi_types then
			_Ctrls.NodeTypeCombo:setText("multi types select")
			_Ctrls.NodeTypeCombo:getEditbox():setProperty("NormalTextColour", disableCol)
			_Ctrls.NodeTypeCombo:disable()
		else
			_Ctrls.NodeTypeCombo:enable()
			_Ctrls.NodeTypeCombo:getEditbox():setProperty("NormalTextColour", enableCol)
			if game_node0 ~= nil then
				_Ctrls.NodeTypeCombo:setItemSelectState(game_node0:getGameNodeType(), true)
			end
		end
	else
		local game_node0 = nil
		if sel_nodes == 1 then
			game_node0 = MyScnMgr:getSelectedSceneNode(0):getGameNode()
		end
		if game_node0 ~= nil then
			_Ctrls.NodeTypeCombo:setItemSelectState(game_node0:getGameNodeType(), true)
		else
			local sel_item = _Ctrls.NodeNameCombo:getSelectedItem()
			if sel_item ~= nil then
				_Ctrls.NodeNameCombo:setItemSelectState(sel_item, false)
			end
			sel_item = _Ctrls.NodeTypeCombo:getSelectedItem()
			if sel_item ~= nil then
				_Ctrls.NodeTypeCombo:setItemSelectState(sel_item, false)
			end
		end
		_Ctrls.NodeNameCombo:getEditbox():setProperty("NormalTextColour", enableCol)
		_Ctrls.NodeTypeCombo:getEditbox():setProperty("NormalTextColour", enableCol)
		_Ctrls.NodeNameCombo:enable()
		_Ctrls.NodeTypeCombo:enable()
	end
	
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		local game_node = scene_node:getGameNode()
		local pos = scene_node:getAbsolutePosition()
		local game_node_type_str = "N/A"
		if game_node ~= nil then
			game_node_type_str = game.getGameNodeTypeReadableName(
				game_node:getGameNodeType())
		end
		_Ctrls.StatusInfoText:setText(string.format(
			"  Abs.position { X = %.4f, Y = %.4f, Z = %.4f }\n"..
			"Selected node (%s / %s)",
			pos.X, pos.Y, pos.Z,
			game_node_type_str,
			scn.getSceneNodeTypeReadableName(scene_node:getSceneNodeType())))
	elseif sel_nodes > 1 then
		_Ctrls.StatusInfoText:setText(string.format("Selected nodes %d", sel_nodes))
	else
		_Ctrls.StatusInfoText:setText("")
	end
	
	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl:disable()	
	end
	_Ctrls.Buttons.ShowButton.Ctrl:enable()
	_Ctrls.Buttons.HideButton.Ctrl:enable()
	
	if sel_nodes > 0 then
		if sel_nodes == 1 then
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			if not _Ctrls.SceneNodeTabControl:isVisible() then
				_Ctrls.SceneNodeTabControl:setSelectedTabAtIndex(0)
				_Ctrls.CommonTabControl:setSelectedTabAtIndex(0)
			end
			_Ctrls.Buttons.SaveAsNodeButton.Ctrl:enable()
			local fname = scene_node:getFileName()
			if MyFileSys:existFile(fname) then
				_Ctrls.Buttons.SaveNodeButton.Ctrl:enable()
				_Ctrls.Buttons.ReloadNodeButton.Ctrl:enable()
				_Ctrls.Buttons.EditNodeButton.Ctrl:enable()
			end
		else
			_Ctrls.Buttons.SaveNodeButton.Ctrl:enable()
			_Ctrls.Buttons.ReloadNodeButton.Ctrl:enable()
		end
		local clone_enabled = true
		for i = 0, sel_nodes - 1 do
			local game_node = MyScnMgr:getSelectedSceneNode(i):getGameNode()
			if game_node == nil then
				clone_enabled = false
				break
			end
		end
		if clone_enabled then
			_Ctrls.Buttons.CloneNodeButton.Ctrl:enable()
		end
		_Ctrls.Buttons.DropNodeButton.Ctrl:enable()
	else
		_Ctrls.Buttons.NewNodeButton.Ctrl:enable()
		_Ctrls.Buttons.OpenNodeButton.Ctrl:enable()
	end
	_Ctrls.ClientArea:setVisible(sel_nodes > 0)
	_Ctrls.SceneNodeTabControl:setVisible(sel_nodes == 1)

	_ScenedUpdateNodeNameCombo()

	local menuItem = _Ctrls.MenuItems.ShowHideShadowsDebugData
	if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_SHADOW_DEBUG_DATA) then
		menuItem:setText(SCENED_MENU_NAME_SHOW_SHADOW_DEBUG_DATA)
	else
		menuItem:setText(SCENED_MENU_NAME_HIDE_SHADOW_DEBUG_DATA)
	end
	
	menuItem = _Ctrls.MenuItems.ShowHideLights
	if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_LIGHTS) then
		menuItem:setText(SCENED_MENU_NAME_SHOW_LIGHTS)
	else
		menuItem:setText(SCENED_MENU_NAME_HIDE_LIGHTS)
	end

	menuItem = _Ctrls.MenuItems.RenderTerrainNormals
	if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_TERRAIN_NORMALS) then
		menuItem:setText(SCENED_MENU_NAME_SHOW_TERRAIN_NORMALS)
	else
		menuItem:setText(SCENED_MENU_NAME_HIDE_TERRAIN_NORMALS)
	end
	
	_ScenedUpdatePolygonModeMenuItems()
	
	for k, v in pairs(_Ctrls.Layouts) do
		v.updateControls()
	end
end

function _ScenedOnSelectSceneNodeImpl(scene_node)
	scene_node:setDebugDataVisible(true)
	--LOG_INFO(string.format("Selected scene node (%s)",
	--	tostring(scene_node)))
	for k, v in pairs(_Ctrls.Layouts) do
		if v.onSelectSceneNode ~= nil then
			v.onSelectSceneNode(scene_node)
		end
	end
	Scened.updateControls()
end

function _ScenedOnDeselectSceneNodeImpl(scene_node)
	scene_node:setDebugDataVisible(false)
	--LOG_INFO(string.format("Deselected scene node (%s)",
	--	tostring(scene_node)))
	if scene_node:getGameNode() == nil then
		MyScnMgr:addToDeletionQueue(scene_node)
	end
	for k, v in pairs(_Ctrls.Layouts) do
		if v.onDeselectSceneNode ~= nil then
			v.onDeselectSceneNode(scene_node)
		end
	end
	Scened.updateControls()
end

function _ScenedUpdateSameSceneNodes(scene_node)
	local file_name = scene_node:getFileName()
	LOG_INFO("Update "..file_name)
	local other_scene_node = MyScnMgr:getFirstSceneNodeInTypeList(scene_node:getSceneNodeType())
	while other_scene_node ~= nil do
		if not core.isPtrsEq(other_scene_node, scene_node) then
			if file_name == other_scene_node:getFileName() then
				LOG_INFO("UPDATE "..tostring(other_scene_node).." "..
					scn.getSceneNodeTypeReadableName(other_scene_node:getSceneNodeType()))
			end			
		end
		other_scene_node = MyScnMgr:getNextSceneNodeInTypeList(other_scene_node)
	end
end

function _ScenedReloadSelectedNodes()
	local sel_nodes_table = {}
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	for i = 0, sel_nodes - 1 do
		local scene_node = MyScnMgr:getSelectedSceneNode(i)
		table.insert(sel_nodes_table, scene_node)
	end
	for i = 1, table.getn(sel_nodes_table) do
		local scene_node = sel_nodes_table[i]
		local game_node = scene_node:getGameNode()
		if game_node ~= nil then
			game_node = MyGameMgr:reloadGameNode(game_node)
			if game_node ~= nil then
				MyScnMgr:addSceneNodeToSelectionList(game_node:getSceneNode())
			end
		else
			scene_node = MyScnMgr:reloadSceneNode(scene_node)
			if scene_node ~= nil then
				MyScnMgr:addSceneNodeToSelectionList(scene_node)
			end
		end
	end
	MyScnMgr:setActiveCamera(Scened:getWorkCamera())
end

local _SceneFileFilter		= {
	[1]       = {Descr = "Scene file",	Value = "scene"	},
	["scene"] = {Descr = "Scene file",	Value = "scene"	},}
	
local _SceneNodeFileFilter	= {
	[1]      = {Descr = "Game Node",	Value = "node"	},
	[2]		 = {Descr = "Scene Node",	Value = "xml"	},
	["node"] = {Descr = "Game Node",	Value = "node"	},
	["xml"]  = {Descr = "Scene Node",	Value = "xml"	},}

SCENE_NODE_FILE_FILTER = _SceneNodeFileFilter

local function _ScenedToolImageViewerOpenImage(full_file_name)
end

local function _ScenedToolImageViewerReloadImage(full_file_name)
end

local function _ScenedToolImageViewerSaveImage(full_file_name)
end

local function _ScenedToolImageViewerClosedImage()
end

function _ScenedWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Menu.File.SceneSaveAs" then
		local file_name = core.extractFileName(MyGameMgr:getGameSceneFileName())
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save Scene As...",
			string.format("%sscenes/", MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS)),
			file_name,
			_SceneFileFilter, _ScenedSaveSceneAccepted)
	elseif name == "Scened.Menu.File.SceneSave" then
		MyGameMgr:saveGameScene()
	elseif name == "Scened.Menu.File.SceneOpen" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Scene...",
			string.format("%sscenes/", MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS)), nil,
			_SceneFileFilter, _ScenedOpenSceneAccepted)
	elseif name == "Scened.Menu.File.SceneReset" then
		MyGameMgr:clearGameScene()
		Scened.updateControls()
	elseif name == "Scened.Menu.Actions.ReloadMaterials" then
		Scened.reloadMaterials()
	elseif name == "Scened.Menu.Tools.NormalMapConverter" then
		Helper.GUI.Scened.NormalMapConverterDialog.show()
	elseif name == "Scened.Menu.Tools.ImageViewer" then
		local file_name = nil
		Helper.GUI.ImageEditor.show(0,
			bit.bor(IMAGE_EDITOR_FLAGS.OPEN, IMAGE_EDITOR_FLAGS.SAVE, IMAGE_EDITOR_FLAGS.RELOAD),
			"Image Viewer", file_name,
			_ScenedToolImageViewerOpenImage,
			_ScenedToolImageViewerReloadImage,
			_ScenedToolImageViewerSaveImage,
			_ScenedToolImageViewerClosedImage)	
	elseif name == "Scened.Menu.Options.SceneProps" then
		Helper.GUI.Scened.ScenePropsDialog.show()		
	elseif name == "Scened.Menu.Exit" then
		Scened.exit()
	elseif name == "Scened.NewNodeButton" then
		_Ctrls.NewNodeDialog:setVisible(true)
		_Ctrls.NewNodeDialog:moveToFront()
	elseif name == "Scened.SaveNodeButton" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		for i = 0, sel_nodes - 1 do
			local scene_node = MyScnMgr:getSelectedSceneNode(i)
			local text = Helper.getSceneNodeFileText(scene_node)
			local full_file_name = scene_node:getFileName()
			local file = io.open (full_file_name, "w")
			if file ~= nil then
				file:write(text)
				file:close()
				_ScenedUpdateSameSceneNodes(scene_node)
			end
		end	
	elseif name == "Scened.SaveAsNodeButton" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		if sel_nodes == 1 then
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			local full_file_name = scene_node:getFileName()
			local file_path = MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS)
			local file_name = nil
			if MyFileSys:existFile(full_file_name) then
				file_path = core.extractFilePath(full_file_name)
				file_name = core.extractFileName(full_file_name)
			end
			Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save Node As...",
				file_path, file_name, _SceneNodeFileFilter, _ScenedSaveAsNodeAccepted)
		end
	elseif name == "Scened.OpenNodeButton" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Node...",
			MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS), nil,
			_SceneNodeFileFilter, _ScenedOpenNodeAccepted)
	elseif name == "Scened.ReloadNodeButton" then
		_ScenedReloadSelectedNodes()
	elseif name == "Scened.CloneNodeButton" then
		local scene_nodes_new = {}
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		for i = 0, sel_nodes - 1 do
			local scene_node = MyScnMgr:getSelectedSceneNode(i)
			local game_node = scene_node:getGameNode()
			if game_node ~= nil then
				local game_node_new = MyGameMgr:cloneGameNode(game_node, false)
				if game_node_new ~= nil then
					table.insert(scene_nodes_new, game_node_new:getSceneNode())
				end
			end
		end
		MyScnMgr:clearSelectionList()
		for k, v in scene_nodes_new do
			MyScnMgr:addSceneNodeToSelectionList(v)
		end
	elseif name == "Scened.EditNodeButton" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		if sel_nodes > 0 then
			Scened.hidePanel()
			MyScnMgr:freezeSelectionList()
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			local file_name = scene_node:getFileName()
			Helper.GUI.TextEditor.show(0,
				bit.bor(TEXT_EDITOR_FLAGS.SAVE, TEXT_EDITOR_FLAGS.RELOAD),
				"XML Editor", file_name,
				_ScenedTextEditorOpenNodeXMLFile,
				_ScenedTextEditorReloadNodeXMLFile,
				_ScenedTextEditorSaveNodeXMLFile,
				_ScenedTextEditorClosedXMLFile)
			Helper.GUI.TextEditor.setText(Helper.getSceneNodeFileText(scene_node))
		end
	elseif name == "Scened.DropNodeButton" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		for i = 0, sel_nodes - 1 do
			MyScnMgr:addToDeletionQueue(MyScnMgr:getSelectedSceneNode(i))
		end
	elseif name == "Scened.ShowButton" then
		Scened.showPanel()
	elseif name == "Scened.HideButton" then
		Scened.hidePanel()
	elseif name == _Ctrls.MenuItems.StartStopGame:getName() then
		Scened.startStopGame()
	elseif name == _Ctrls.MenuItems.SuspendResumeTimer:getName() then
		Scened.suspendResumeTimer()
	elseif name == _Ctrls.MenuItems.ShowHideShadowsDebugData:getName() then
		if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_SHADOW_DEBUG_DATA) then
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_SHADOW_DEBUG_DATA, true)
			_Ctrls.MenuItems.ShowHideShadowsDebugData:setText(SCENED_MENU_NAME_HIDE_SHADOW_DEBUG_DATA)
		else
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_SHADOW_DEBUG_DATA, false)
			_Ctrls.MenuItems.ShowHideShadowsDebugData:setText(SCENED_MENU_NAME_SHOW_SHADOW_DEBUG_DATA)
		end
	elseif name == _Ctrls.MenuItems.ShowHideLights:getName() then
		if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_LIGHTS) then
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_LIGHTS, true)
			_Ctrls.MenuItems.ShowHideLights:setText(SCENED_MENU_NAME_HIDE_LIGHTS)
		else
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_LIGHTS, false)
			_Ctrls.MenuItems.ShowHideLights:setText(SCENED_MENU_NAME_SHOW_LIGHTS)
		end	
	elseif name == _Ctrls.MenuItems.RenderTerrainNormals:getName() then
		if not MyScnMgr:getSceneRenderFlag(scn.ESRF_RENDER_TERRAIN_NORMALS) then
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_TERRAIN_NORMALS, true)
			_Ctrls.MenuItems.RenderTerrainNormals:setText(SCENED_MENU_NAME_HIDE_TERRAIN_NORMALS)
		else
			MyScnMgr:setSceneRenderFlag(scn.ESRF_RENDER_TERRAIN_NORMALS, false)
			_Ctrls.MenuItems.RenderTerrainNormals:setText(SCENED_MENU_NAME_SHOW_TERRAIN_NORMALS)
		end
	elseif name == _Ctrls.MenuItems.PolygonModeSolid:getName() then
		MyDriver:setPolygonFillMode(vid.EPFM_SOLID)
		_ScenedUpdatePolygonModeMenuItems()
	elseif name == _Ctrls.MenuItems.PolygonModeWireframe:getName() then
		MyDriver:setPolygonFillMode(vid.EPFM_WIREFRAME)
		_ScenedUpdatePolygonModeMenuItems()
	elseif name == _Ctrls.MenuItems.PolygonModePoint:getName() then
		MyDriver:setPolygonFillMode(vid.EPFM_POINT)
		_ScenedUpdatePolygonModeMenuItems()		
	end

end

function _ScenedNewSceneAccepted(full_file_name, rel_file_name)
	LOG_INFO("New scene "..full_file_name)
end

function _ScenedSaveSceneAccepted(full_file_name, rel_file_name)
	MyGameMgr:saveGameScene("./scenes/"..	rel_file_name)
end

function _ScenedOpenSceneAccepted(full_file_name, rel_file_name)
	MyGameMgr:loadGameScene("./scenes/"..	rel_file_name)
end

function _ScenedOpenNodeAccepted(full_file_name, rel_file_name)
	local scene_only = false
	local file_ext = core.extractFileExt(full_file_name)
	local scene_node = nil
	if file_ext == "xml" then
		scene_node = MyScnMgr:loadSceneNode(full_file_name)
	elseif file_ext == "node" then
		local game_node = MyGameMgr:loadGameNode(full_file_name)
		if game_node ~= nil then
			scene_node = game_node:getSceneNode()
		end
	end
	if scene_node ~= nil then
		MyScnMgr:clearSelectionList()
		MyScnMgr:addSceneNodeToSelectionList(scene_node)		
	end
end

function _ScenedSaveAsNodeAccepted(full_file_name, rel_file_name)
	LOG_INFO("Save scene node as "..full_file_name)
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getGameNode() == nil then
			full_file_name = string.gsub(full_file_name, ".node", ".xml")
		end
		local scene_only = false
		local file_ext = core.extractFileExt(full_file_name)
		if file_ext == "xml" then
			scene_only = true
		end
		local text = Helper.getSceneNodeFileText(scene_node, scene_only)
		local file = io.open (full_file_name, "w")
		if file ~= nil then
			file:write(text)
			file:close()
			scene_node:setFileName(full_file_name)
			_ScenedUpdateSameSceneNodes(scene_node)
			Scened.updateControls()
		end
	end	
end

function _ScenedTextEditorOpenNodeXMLFile()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Node...",
			core.extractFilePath(scene_node:getFileName()), nil,
			_SceneNodeFileFilter, _ScenedTextEditorOpenNodeXMLFileAccepted)
	end
end

function _ScenedTextEditorOpenNodeXMLFileAccepted(full_file_name, rel_file_name)
	Helper.GUI.TextEditor.setFileName(full_file_name, true)
end

function _ScenedTextEditorReloadNodeXMLFile()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		MyScnMgr:unfreezeSelectionList()
		_ScenedReloadSelectedNodes()
		MyScnMgr:freezeSelectionList()		
	end
	sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		local file_name = scene_node:getFileName()
		Helper.GUI.TextEditor.setFileName(file_name, false)		
		Helper.GUI.TextEditor.setText(Helper.getSceneNodeFileText(scene_node))
	end
end

function _ScenedTextEditorSaveNodeXMLFile()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		local full_file_name = scene_node:getFileName()
		local file = io.open (full_file_name, "w")
		if file ~= nil then
			file:write(Helper.GUI.TextEditor.getText())
			file:close()
			_ScenedUpdateSameSceneNodes(scene_node)
		end
		_ScenedTextEditorReloadNodeXMLFile()
	end
end

function _ScenedTextEditorClosedXMLFile()
	Scened.showPanel()
	MyScnMgr:unfreezeSelectionList()
end

function _ScenedComboHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.NodeTypeCombo" then
		local item = _Ctrls.NodeTypeCombo:getSelectedItem()
		local idx = _Ctrls.NodeTypeCombo:getItemIndex(item)
		_ScenedUpdateNodeNameCombo()
	elseif name == "Scened.NodeNameCombo" then
		local item = _Ctrls.NodeNameCombo:getSelectedItem()
		local data = item:getUserData()
		local game_node = tolua.cast(data, "game::IGameNode")
		if game_node ~= nil then
			MyScnMgr:clearSelectionList()
			MyScnMgr:addSceneNodeToSelectionList(game_node:getSceneNode())
		end
	end
end

function _ScenedSetVisible(visible)
	_Ctrls.Menubar:setVisible(visible)
	_Ctrls.NodeExplorer.Root:setVisible(visible)
	_Ctrls.StatusInfoText:setVisible(visible)
end

function _ScenedOnGameStarted()
	_Ctrls.MenuItems.StartStopGame:setText("Stop game (F3)")
	Scened.resumeTimer()
end

function _ScenedOnGameStopped()
	_Ctrls.MenuItems.StartStopGame:setText("Start game (F3)")
	Scened.suspendTimer()
end

function _ScenedOnAnimationBegin(game_node, animation_idx)
	for k, v in pairs(_Ctrls.Layouts) do
		if v.onAnimationStart ~= nil then
			v.onAnimationStart(game_node, animation_idx)
		end
	end
end

function _ScenedOnAnimationEnd(game_node, animation_idx)
	for k, v in pairs(_Ctrls.Layouts) do
		if v.onAnimationEnd ~= nil then
			v.onAnimationEnd(game_node, animation_idx)
		end
	end
end
