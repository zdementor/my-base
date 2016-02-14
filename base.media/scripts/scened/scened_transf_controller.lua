
local _Ctrls =
{
	ButtonsGroup = nil,
	Root = {},
	Editboxes =
	{
		PosXEditbox	= {},
		PosYEditbox	= {},
		PosZEditbox	= {},
		RotXEditbox	= {},
		RotYEditbox	= {},
		RotZEditbox	= {},
		SclXEditbox	= {},
		SclYEditbox	= {},
		SclZEditbox	= {},
	},
	DeltaEditboxes =
	{
		PosDeltaEditbox	= {},
		RotDeltaEditbox	= {},
		SclDeltaEditbox	= {},
	},
	Labels =
	{
		PosXLabel	= {},
		PosYLabel	= {},
		PosZLabel	= {},
		RotXLabel	= {},
		RotYLabel	= {},
		RotZLabel	= {},
		SclXLabel	= {},
		SclYLabel	= {},
		SclZLabel	= {},
	},
	Buttons =
	{
		PosPlusXBtn		= {},
		PosMinusXBtn	= {},
		PosPlusYBtn		= {},
		PosMinusYBtn	= {},
		PosPlusZBtn		= {},
		PosMinusZBtn	= {},
		RotPlusXBtn		= {},
		RotMinusXBtn	= {},
		RotPlusYBtn		= {},
		RotMinusYBtn	= {},
		RotPlusZBtn		= {},
		RotMinusZBtn	= {},
		SclPlusXBtn		= {},
		SclMinusXBtn	= {},
		SclPlusYBtn		= {},
		SclMinusYBtn	= {},
		SclPlusZBtn		= {},
		SclMinusZBtn	= {},
		MoveHereBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-jump"},
		JumpToBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.system-search"},
	},
	LockScaleCheck = nil,
}

function _ScenedInitTransfControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_common.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Transf.Root")
	CEGUIWinMgr:getWindow("Scened.TransformTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	_Ctrls.ButtonsGroup = CEGUIWinMgr:getWindow("Scened.Transf.ButtonsGroup")
	_Ctrls.LockScaleCheck = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.Transf.LockScaleCheck"), "CEGUI::Checkbox")
	
	Helper.GUI.initButtons("Scened.Transf.", _Ctrls.Buttons, "_ScenedTransfWidgetClicked")
	
	for key, value in pairs(_Ctrls.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Transf."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", "_ScenedTransfEditboxTextAccepted")
	end
	for key, value in pairs(_Ctrls.DeltaEditboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Transf."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", "_ScenedTransfEditboxTextAccepted")
	end
	for key, value in pairs(_Ctrls.Labels) do
		value.Ctrl = CEGUIWinMgr:getWindow("Scened.Transf."..key)
	end

	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedTransfUpdate,
		updateControls = _ScenedUpdateTransfControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end

function _ScenedTransfUpdate()

	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node ~= nil then
			local pos = scene_node:getPosition()
			local rot = scene_node:getRotation()
			local scl = scene_node:getScale()
			local tmp = {}
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.PosXEditbox.Ctrl, Value = pos.X})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.PosYEditbox.Ctrl, Value = pos.Y})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.PosZEditbox.Ctrl, Value = pos.Z})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.RotXEditbox.Ctrl, Value = rot.X})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.RotYEditbox.Ctrl, Value = rot.Y})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.RotZEditbox.Ctrl, Value = rot.Z})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.SclXEditbox.Ctrl, Value = scl.X})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.SclYEditbox.Ctrl, Value = scl.Y})
			table.insert(tmp, {Editbox = _Ctrls.Editboxes.SclZEditbox.Ctrl, Value = scl.Z})
			for i = 1, table.getn(tmp) do
				if not tmp[i].Editbox:hasInputFocus() then
					if tonumber(tmp[i].Editbox:getText()) ~= tmp[i].Value then
						tmp[i].Editbox:setText(string.format("%.5f", tmp[i].Value))
					end
				end
			end
		end
	end

end

function _ScenedUpdateTransfControls()

	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local vis = false
	if sel_nodes == 1 then
		vis = true
	end

	for key, value in pairs(_Ctrls.Editboxes) do
		value.Ctrl:setVisible(vis)
	end
	for key, value in pairs(_Ctrls.Labels) do
		value.Ctrl:setVisible(vis)
	end
	if vis then
		_Ctrls.ButtonsGroup:setProperty("UnifiedAreaRect", "{{0.45,0},{0,0},{1,0},{1,0}}")
		_Ctrls.Root.Ctrl:getParent():removeChildWindow(_Ctrls.Root.Ctrl)
		CEGUIWinMgr:getWindow("Scened.TransformTab"):addChildWindow(_Ctrls.Root.Ctrl)
	else
		_Ctrls.ButtonsGroup:setProperty("UnifiedAreaRect", "{{0.1,0},{0,0},{1,0},{1,0}}")
		_Ctrls.Root.Ctrl:getParent():removeChildWindow(_Ctrls.Root.Ctrl)
		CEGUIWinMgr:getWindow("Scened.ClientArea"):addChildWindow(_Ctrls.Root.Ctrl)
	end
end

function _ScenedTransfSet(delta, posX, posY, posZ, rotX, rotY, rotZ, sclX, sclY, sclZ)
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	for i = 0, sel_nodes - 1 do
		local scene_node = MyScnMgr:getSelectedSceneNode(i)
		if scene_node ~= nil then
			local node_pos = core.vector3df()
			local node_rot = core.vector3df()
			local node_scl = core.vector3df()
			node_pos:set(scene_node:getPosition())
			node_rot:set(scene_node:getRotation())
			node_scl:set(scene_node:getScale())
			if delta then
				node_pos:set(node_pos.X + posX, node_pos.Y + posY, node_pos.Z + posZ)
				node_rot:set(node_rot.X + rotX, node_rot.Y + rotY, node_rot.Z + rotZ)
				node_scl:set(node_scl.X + sclX, node_scl.Y + sclY, node_scl.Z + sclZ)
			else
				node_pos:set(posX, posY, posZ)
				node_rot:set(rotX, rotY, rotZ)
				node_scl:set(sclX, sclY, sclZ)
			end
			scene_node:setPosition(node_pos)
			scene_node:setRotation(node_rot)
			scene_node:setScale(node_scl)
		end
	end
end

local position = core.vector3df(0, 0, 0)
local cam_pos = core.vector3df(0, 0, 0)
local cam_tgt = core.vector3df(0, 0, 0)

function _ScenedTransfWidgetClicked(args)

	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()

	local delta_pos = tonumber(_Ctrls.DeltaEditboxes.PosDeltaEditbox.Ctrl:getText())
	local delta_rot = tonumber(_Ctrls.DeltaEditboxes.RotDeltaEditbox.Ctrl:getText())
	local delta_scl = tonumber(_Ctrls.DeltaEditboxes.SclDeltaEditbox.Ctrl:getText())
	local other_scl = 0
	if _Ctrls.LockScaleCheck:isSelected() then
		other_scl = delta_scl
	end
		
	if name == "Scened.Transf.PosPlusXBtn" then
		_ScenedTransfSet(true, delta_pos, 0, 0, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.PosMinusXBtn" then
		_ScenedTransfSet(true,-delta_pos, 0, 0, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.PosPlusYBtn" then
		_ScenedTransfSet(true, 0, delta_pos, 0, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.PosMinusYBtn" then
		_ScenedTransfSet(true, 0,-delta_pos, 0, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.PosPlusZBtn" then
		_ScenedTransfSet(true, 0, 0, delta_pos, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.PosMinusZBtn" then
		_ScenedTransfSet(true, 0, 0,-delta_pos, 0, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.RotPlusXBtn" then
		_ScenedTransfSet(true, 0, 0, 0, delta_rot, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.RotMinusXBtn" then
		_ScenedTransfSet(true, 0, 0, 0,-delta_rot, 0, 0, 0, 0, 0)
	elseif name == "Scened.Transf.RotPlusYBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, delta_rot, 0, 0, 0, 0)
	elseif name == "Scened.Transf.RotMinusYBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0,-delta_rot, 0, 0, 0, 0)
	elseif name == "Scened.Transf.RotPlusZBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, delta_rot, 0, 0, 0)
	elseif name == "Scened.Transf.RotMinusZBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0,-delta_rot, 0, 0, 0)
	elseif name == "Scened.Transf.SclPlusXBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0, delta_scl, other_scl, other_scl)
	elseif name == "Scened.Transf.SclMinusXBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0,-delta_scl,-other_scl,-other_scl)
	elseif name == "Scened.Transf.SclPlusYBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0, other_scl, delta_scl, other_scl)
	elseif name == "Scened.Transf.SclMinusYBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0,-other_scl,-delta_scl,-other_scl)
	elseif name == "Scened.Transf.SclPlusZBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0, other_scl, other_scl, delta_scl)
	elseif name == "Scened.Transf.SclMinusZBtn" then
		_ScenedTransfSet(true, 0, 0, 0, 0, 0, 0,-other_scl,-other_scl,-delta_scl)
	elseif name == "Scened.Transf.MoveHereBtn" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		if sel_nodes > 0 then
			cam_pos:set(MyScnMgr:getActiveCamera():getAbsolutePosition())
			for i = 0, sel_nodes - 1 do
				local scene_node = MyScnMgr:getSelectedSceneNode(i)
				local pos = scene_node:getAbsolutePosition()
				if i == 0 then
					position:set(pos)
				else
					position = position + pos
				end
			end
			position = position / sel_nodes
			position = cam_pos - position			
			_ScenedTransfSet(true, position.X, position.Y, position.Z, 0, 0, 0, 0, 0, 0)
		end
	elseif name == "Scened.Transf.JumpToBtn" then
		local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
		local camera = MyScnMgr:getActiveCamera()
		if sel_nodes > 0 and camera ~= nil then
			local scene_node = MyScnMgr:getSelectedSceneNode(0)
			cam_pos:set(scene_node:getTransformedBoundingBox():getCenter())
			cam_tgt:set(cam_pos.X + 1.0, cam_pos.Y - 1.0, cam_pos.Z + 1.0)
			camera:setPosition(cam_pos)	
			camera:setTarget(cam_tgt)
			camera:updateAbsoluteTransformation()
		end
	end
end

function _ScenedTransfEditboxTextAccepted(args)

	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local digits = 5

	local editbox = tolua.cast(we.window, "CEGUI::Editbox")
	local value = tonumber(editbox:getText())

	if name == "Scened.Transf.PosXEditbox" then
	elseif name == "Scened.Transf.PosYEditbox" then
	elseif name == "Scened.Transf.PosZEditbox" then
	elseif name == "Scened.Transf.PosDeltaEditbox" then
		if value <= 0.0 then
			value = 100.0
		end
		digits = 1
	elseif name == "Scened.Transf.RotXEditbox" then
	elseif name == "Scened.Transf.RotYEditbox" then
	elseif name == "Scened.Transf.RotZEditbox" then
		if value <= 0.0 then
			value = 15.0
		end
		digits = 1
	elseif name == "Scened.Transf.RotDeltaEditbox" then
	elseif name == "Scened.Transf.SclXEditbox" then
	elseif name == "Scened.Transf.SclYEditbox" then
	elseif name == "Scened.Transf.SclZEditbox" then
	elseif name == "Scened.Transf.SclDeltaEditbox" then
		if value <= 0.0 then
			value = 0.1
		end
		digits = 1		
	end

	if digits == 5 then
		editbox:setText(string.format("%.5f", value))
	else
		editbox:setText(string.format("%.1f", value))
	end
	editbox:setCaratIndex(0)
	editbox:deactivate()
	
	_ScenedTransfSet(false,
		tonumber(_Ctrls.Editboxes.PosXEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.PosYEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.PosZEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.RotXEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.RotYEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.RotZEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.SclXEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.SclYEditbox.Ctrl:getText()),
		tonumber(_Ctrls.Editboxes.SclZEditbox.Ctrl:getText()))
end
