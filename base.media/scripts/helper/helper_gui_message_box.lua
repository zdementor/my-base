
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn = {Ctrl = nil, ImageName = nil	},
	},
	MessageText = {},
}

local _Callbacks =
{
}

local function _MessageBoxClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.MessageBox.OKBtn" or
			name == "Helper.MessageBox.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
	end
end

local function _MessageBoxShow(mode, flags, caption, msg_text)
	if not _HelperInit() then
		return
	end
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	_Ctrls.MessageText.Ctrl:setText(msg_text)

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.MessageBox.show = _MessageBoxShow

function _MessageBoxInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_message_box.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.MessageBox.Frame")
	
	Helper.GUI.initButtons("Helper.MessageBox.",
		_Ctrls.Buttons, _MessageBoxClick)
	
	_Ctrls.MessageText.Ctrl = CEGUIWinMgr:getWindow("Helper.MessageBox.MessageText")
	
	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
	
end
