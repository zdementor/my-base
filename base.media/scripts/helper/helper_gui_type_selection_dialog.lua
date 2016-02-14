
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn		= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
	},
	TypesCombo = {},
}

local _Callbacks =
{
	SelectionAccept = nil,
}

local function _TypeSelectionDialogClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.TypeSelectionDialog.OKBtn" then
		local item = _Ctrls.TypesCombo.Ctrl:getSelectedItem()
		if item ~= nil then
			local sel_type = tonumber(item:getID())
			_Ctrls.Root.Ctrl:setVisible(false)
			_Callbacks.SelectionAccept(sel_type)
		end
	elseif name == "Helper.TypeSelectionDialog.CancelBtn" or
			name == "Helper.TypeSelectionDialog.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
	end
end

local function _TypeSelectionDialogShow(mode, flags, caption, types_table, selection_accept_callback)
	if not _HelperInit() then
		return
	end
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)

	_Ctrls.TypesCombo.Ctrl:resetList()
	for k, v in pairs(types_table) do
		Helper.GUI.addListboxItem(_Ctrls.TypesCombo.Ctrl, OPTIONS.HelperGUI.SchemeName,
			v.Name, v.Value, nil)
	end
	local item = _Ctrls.TypesCombo.Ctrl:getListboxItemFromIndex(0)
	_Ctrls.TypesCombo.Ctrl:setItemSelectState(item, true)

	_Callbacks.SelectionAccept = selection_accept_callback	

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.TypeSelectionDialog.show = _TypeSelectionDialogShow

function _TypeSelectionDialogInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_type_selection_dialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.TypeSelectionDialog.Frame")
	
	Helper.GUI.initButtons("Helper.TypeSelectionDialog.",
		_Ctrls.Buttons, _TypeSelectionDialogClick)

	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _TypeSelectionDialogClick)
	
	_Ctrls.TypesCombo.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.TypeSelectionDialog.TypesCombo"), "CEGUI::Combobox")
	
	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
	
end
