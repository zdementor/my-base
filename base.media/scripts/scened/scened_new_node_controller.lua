
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		ClearGameNodeTypeBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"	},
		ClearDynamicObjectTypeBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"	},
		OKBtn						= {Ctrl = nil, ImageName = nil	},
		CancelBtn					= {Ctrl = nil, ImageName = nil	},
	},
	SceneNodeTypeCombo = {},
	GameNodeTypeCombo = {},
	DynamicObjectTypeCombo = {},	
}

function _ScenedInitNewNodeControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_new_node.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.NewNode.Root")
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	Helper.GUI.initButtons("Scened.NewNode.", _Ctrls.Buttons, "_ScenedNewNodeWidgetClicked")
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Scened.NewNode.Frame")
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", "_ScenedNewNodeWidgetClicked")
	
	_Ctrls.SceneNodeTypeCombo.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NewNode.SceneNodeTypeCombo"), "CEGUI::Combobox")
	_Ctrls.GameNodeTypeCombo.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NewNode.GameNodeTypeCombo"), "CEGUI::Combobox")
	_Ctrls.DynamicObjectTypeCombo.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NewNode.DynamicObjectTypeCombo"), "CEGUI::Combobox")
	
	_Ctrls.SceneNodeTypeCombo.Ctrl:resetList()
	for i = 0, scn.ESNT_UNKNOWN_SCENE_NODE - 1 do
		Helper.GUI.addListboxItem(_Ctrls.SceneNodeTypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			scn.getSceneNodeTypeReadableName(i), i, nil)
	end
	local item = _Ctrls.SceneNodeTypeCombo.Ctrl:getListboxItemFromIndex(0)
	_Ctrls.SceneNodeTypeCombo.Ctrl:setItemSelectState(item, true)

	_Ctrls.GameNodeTypeCombo.Ctrl:resetList()
	for i = 0, game.EGNT_UNKNOWN - 1 do
		Helper.GUI.addListboxItem(_Ctrls.GameNodeTypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			game.getGameNodeTypeReadableName(i), i, nil)
	end
	_Ctrls.DynamicObjectTypeCombo.Ctrl:resetList()
	for i = 0, dyn.E_DYNAMIC_OBJECT_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.DynamicObjectTypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			dyn.getDynamicObjectTypeReadableName(i), i, nil)
	end
	
	_ScenedClearGameNodeType()
	_ScenedClearDynamicObjectType()

	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
	
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedNewNodeUpdate,
		updateControls = _ScenedUpdateNewNodeControls,
		onSelectSceneNode = _ScenedNewNodeOnSelectSceneNode,
		onDeselectSceneNode = _ScenedNewNodeOnDeselectSceneNode,}
end

function _ScenedNewNodeUpdate()
end

function _ScenedUpdateNewNodeControls()
end

function _ScenedNewNodeOnSelectSceneNode(scene_node)
end

function _ScenedNewNodeOnDeselectSceneNode(scene_node)
end

local node_params = game.SGameNodeCommonParams()

function _ScenedNewNode(scn_type, game_type, dyn_type)
	local scene_node = nil
	if scn_type == nil then
		return scene_node
	end
	if scn_type < 0 or scn_type >= scn.ESNT_UNKNOWN_SCENE_NODE then
		return scene_node
	end

	LOG_INFO("Creating scene node "..
		scn.getSceneNodeTypeReadableName(scn_type).." . . .")

	node_params.Scene.Parameters.ID = -1
	node_params.Scene.Parameters.ParentID = -1
	node_params.Scene.Parameters.Type = scn_type
	node_params.Scene.Transformation.Position:set(0.0, 0.0, 0.0)
	node_params.Scene.Transformation.Rotation:set(0.0, 0.0, 0.0)
	node_params.Scene.Transformation.Scale:set(1.0, 1.0, 1.0)
	
	local scene_node = nil
	if game_type == nil then
		scene_node = MyScnMgr:createSceneNode(node_params.Scene, true)
	else
		node_params.Game.ClassName:set("Standard")
		node_params.Game.Type = game_type
		node_params.Game.MovSpeed = 0.0
		node_params.Game.RotSpeed = 0.0
		node_params.Game.Life = 100.0
		local game_node = MyGameMgr:createGameNode(node_params, true)
		if game_node ~= nil then
			scene_node = game_node:getSceneNode()
		end
	end
	if scene_node ~= nil then
		MyScnMgr:clearSelectionList()
		MyScnMgr:addSceneNodeToSelectionList(scene_node)
	else
		LOG_ERR("Can't create scene node "..
			scn.getSceneNodeTypeReadableName(scn_type))
	end
	_Ctrls.Root.Ctrl:setVisible(false)
	return scene_node
end

function _ScenedClearGameNodeType()
	local sel_item = _Ctrls.GameNodeTypeCombo.Ctrl:getSelectedItem()
	if sel_item ~= nil then
		_Ctrls.GameNodeTypeCombo.Ctrl:setItemSelectState(sel_item, false)
	end
	_Ctrls.GameNodeTypeCombo.Ctrl:setText("N/A")
end

function _ScenedClearDynamicObjectType()
	local sel_item = _Ctrls.DynamicObjectTypeCombo.Ctrl:getSelectedItem()
	if sel_item ~= nil then
		_Ctrls.DynamicObjectTypeCombo.Ctrl:setItemSelectState(sel_item, false)
	end
	_Ctrls.DynamicObjectTypeCombo.Ctrl:setText("N/A")
end

function _ScenedNewNodeWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.NewNode.ClearGameNodeTypeBtn" then
		_ScenedClearGameNodeType()
	elseif name == "Scened.NewNode.ClearDynamicObjectTypeBtn" then
		_ScenedClearDynamicObjectType()
	elseif name == "Scened.NewNode.OKBtn" then
			local item = nil
			local gtype = nil
			item = _Ctrls.GameNodeTypeCombo.Ctrl:getSelectedItem()
			if item ~= nil then
				gtype = item:getID()
			end
			local dtype =  nil
			item = _Ctrls.DynamicObjectTypeCombo.Ctrl:getSelectedItem()
			if item ~= nil then
				dtype = item:getID()
			end
			local stype = scn.ESNT_ANIMATED_MESH_SCENE_NODE
			item = _Ctrls.SceneNodeTypeCombo.Ctrl:getSelectedItem()
			if item ~= nil then
				stype = item:getID()
			end
			_ScenedNewNode(stype, gtype, dtype)
	elseif name == "Scened.NewNode.CancelBtn" then
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Scened.NewNode.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
	end
end
