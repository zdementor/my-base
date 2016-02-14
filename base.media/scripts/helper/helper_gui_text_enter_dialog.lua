
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn		= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
	},
	TextEditbox = {},
}

local _Callbacks =
{
	ValidateText = nil,
	AcceptText = nil,
}

local _InitText = ""

local function _TextEnterDialogClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.TextEnterDialog.OKBtn" then
		local text = _Ctrls.TextEditbox.Ctrl:getText()
		if _Callbacks.ValidateText ~= nil then
			text = _Callbacks.ValidateText(text)
		end
		if text ~= nil and string.len(text) > 0 then
			_Ctrls.Root.Ctrl:setVisible(false)
			_Callbacks.AcceptText(text)
		end
	elseif name == "Helper.TextEnterDialog.CancelBtn" or
			name == "Helper.TextEnterDialog.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
	end
end

local function _TextEnterDialogShow(mode, flags, caption, init_text, accept_callback, validate_callback)
	if not _HelperInit() then
		return
	end
	
	_InitText = init_text
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	_Ctrls.TextEditbox.Ctrl:setText(_InitText)

	_Callbacks.ValidateText = validate_callback	
	_Callbacks.AcceptText = accept_callback

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.TextEnterDialog.show = _TextEnterDialogShow

function _TextEnterDialogInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_text_enter_dialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.TextEnterDialog.Frame")
	
	Helper.GUI.initButtons("Helper.TextEnterDialog.",
		_Ctrls.Buttons, _TextEnterDialogClick)

	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _TextEnterDialogClick)
	
	_Ctrls.TextEditbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.TextEnterDialog.TextEditbox"), "CEGUI::MultiLineEditbox")
	
	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
	
end
