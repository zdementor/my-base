
local _Ctrls =
{
	Root = {},
	Editboxes =
	{
		NameEditbox = {},
		IDEditbox = {},
		ParentNameEditbox = {},
		ParentIDEditbox = {},
	},
	Buttons =
	{
		FindParentBtn = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.system-search"}
	},
}

function _ScenedInitPropsControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Props.Root")
	CEGUIWinMgr:getWindow("Scened.PropertiesTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	for key, value in pairs(_Ctrls.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Props."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", "_ScenedPropsEditboxTextAccepted")
		value.Ctrl:subscribeEvent("KeyUp", "_ScenedPropsEditboxKeyUp")
	end
	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Props."..key), "CEGUI::ButtonBase")
		if value.ImageName ~= nil then
			value.Ctrl:setProperty("NormalImage",	value.ImageName)
			value.Ctrl:setProperty("HoverImage",	value.ImageName)
			value.Ctrl:setProperty("PushedImage",	value.ImageName)
			value.Ctrl:setText("")
		end
		value.Ctrl:subscribeEvent("Clicked", "_ScenedPropsWidgetClicked")
	end

	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedPropsUpdate,
		updateControls = _ScenedUpdatePropsControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end

function _ScenedPropsUpdate()
end

function _ScenedUpdatePropsControls()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		_Ctrls.Editboxes.NameEditbox.Ctrl:setText(scene_node:getName())
		_Ctrls.Editboxes.IDEditbox.Ctrl:setText(string.format("%d", scene_node:getID()))
		local parent_scene_node = scene_node:getParent()
		if parent_scene_node ~= nil then
			_Ctrls.Editboxes.ParentNameEditbox.Ctrl:setText(parent_scene_node:getName())
			_Ctrls.Editboxes.ParentIDEditbox.Ctrl:setText(string.format("%d", parent_scene_node:getID()))
		else
			_Ctrls.Editboxes.ParentNameEditbox.Ctrl:setText("")
			_Ctrls.Editboxes.ParentIDEditbox.Ctrl:setText("")
		end
	end
end

function _ScenedPropsEditboxTextAccepted(args)

	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local editbox = tolua.cast(we.window, "CEGUI::Editbox")
	local value = tonumber(editbox:getText())
	
	if name == "Scened.Props.IDEditbox" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		if sel_nodes == 1 then
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			local id = tonumber(value)
			scene_node:setID(id)
		end
	end
	editbox:setCaratIndex(0)
	editbox:deactivate()

	_ScenedUpdatePropsControls()
end

function _ScenedPropsWidgetClicked(args)

	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()

	if name == "Scened.Props.FindParentBtn" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		if sel_nodes == 1 then
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			local root_scene_node = MyScnMgr:getRootSceneNode()
			local parent_scene_node = scene_node:getParent()
			local parent_game_node = nil
			while parent_scene_node ~= nil do
				parent_game_node = parent_scene_node:getGameNode()
				if parent_game_node ~= nil then
					break
				end
				parent_scene_node = parent_scene_node:getParent()
				if core.isPtrsEq(parent_scene_node, root_scene_node) then
					break
				end
			end
			if parent_game_node ~= nil then
				MyScnMgr:clearSelectionList()
				MyScnMgr:addSceneNodeToSelectionList(parent_scene_node)
			end
		end	
	end

end

function _ScenedPropsEditboxKeyUp(args)

	local we = CEGUI.toWindowEventArgs(args)
	local ke = CEGUI.toKeyEventArgs(args)
	local name = we.window:getName()
	
	if name == "Scened.Props.NameEditbox" then
		if ke.scancode == CEGUI.Key.Return then
			local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
			if sel_nodes == 1 then
				local editbox = tolua.cast(we.window, "CEGUI::MultiLineEditbox")	
				local scene_node = MyScnMgr:getSelectedSceneNode(0)
				local name = editbox:getText()
				name = string.gsub(name, "%c+", "") -- remove control characters
				name = string.gsub(name, "%s+", "_") -- remove spaces
				scene_node:setName(name)
				editbox:setCaratIndex(0)
				editbox:deactivate()
				Scened.updateControls()
			end	
		end
	end

end
