
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn		= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
		ReloadBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh" }
	},
	Listbox = {},
}

local _Callbacks =
{
	SelectionAccept = nil,
}

local _MaterialSelectionInitMaterial = vid.SMaterial()

local function _MaterialSelectionDialogClickHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.MaterialSelectionDialog.OKBtn" then
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Helper.MaterialSelectionDialog.CancelBtn" or
			name == "Helper.MaterialSelectionDialog.Frame" then
		_Callbacks.SelectionAccept(_MaterialSelectionInitMaterial)
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Helper.MaterialSelectionDialog.ReloadBtn" then
		Scened.reloadMaterials()
		_MaterialSelectionReload()
	end
end

local function _MaterialSelectionMouseDoubleClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.MaterialSelectionDialog.Listbox" then
	end
end

local function _MaterialSelectionSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.MaterialSelectionDialog.Listbox" then
		local item = _Ctrls.Listbox.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local idx = item:getID()
			local mat = MyMatMgr:getMaterial(idx)
			_Callbacks.SelectionAccept(mat)
		end
	end
end

function _MaterialSelectionReload()
	_Ctrls.Listbox.Ctrl:resetList()	
	local mat_cnt = MyMatMgr:getMaterialsCount()
	for i = 0, mat_cnt - 1 do
		local mat = MyMatMgr:getMaterial(i)
		local mat_name = MyMatMgr:getMaterialName(i)
		Helper.GUI.addMultiColumnListItem(_Ctrls.Listbox.Ctrl, OPTIONS.HelperGUI.SchemeName, {
				[1] = {Name = mat_name,							ID = i, Data = nil, Color = color},
				[2] = {Name = tostring(mat:getPassesCount()),	ID = i, Data = nil, Color = color},
				})
	end
	local mat_idx = MyMatMgr:findMaterialByHash(_MaterialSelectionInitMaterial:getHash())
	if mat_idx ~= -1 then
		_Ctrls.Listbox.Ctrl:setItemSelectState(CEGUI.MCLGridRef(mat_idx, 0), true)
	end
end

function _MaterialSelectionDialogInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_material_selection_dialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.MaterialSelectionDialog.Frame")
	
	Helper.GUI.initButtons("Helper.MaterialSelectionDialog.",
		_Ctrls.Buttons, _MaterialSelectionDialogClickHandler)
	
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _MaterialSelectionDialogClickHandler)
	
	_Ctrls.Listbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.MaterialSelectionDialog.Listbox"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(_Ctrls.Listbox.Ctrl, 0, {
			[1] = {Name = "Name",	Dim = CEGUI.UDim(0.80, 0)},
			[2] = {Name = "Passes",	Dim = CEGUI.UDim(0.15, 0)},
			})
	_Ctrls.Listbox.Ctrl:subscribeEvent("MouseDoubleClick",	_MaterialSelectionMouseDoubleClick)
	_Ctrls.Listbox.Ctrl:subscribeEvent("SelectionChanged",	_MaterialSelectionSelectionChanged)
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _MaterialSelectionDialogShow(
	mode, flags, caption, init_material, selection_accept_callback)

	if not _HelperInit() then
		return
	end
	_Callbacks.SelectionAccept = selection_accept_callback
	
	_MaterialSelectionInitMaterial:set(init_material)
		
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	
	_MaterialSelectionReload()
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.MaterialSelectionDialog.show = _MaterialSelectionDialogShow
