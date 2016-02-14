
local _Ctrls =
{
	Root = {},
	Frame = {},
	Editbox = {},
	Buttons =
	{
		SaveButton			= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save"				},
		OpenButton			= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"				},
		ReloadButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh"				},
		IncFontButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:apps.preferences-desktop-font-inc"	},
		DecFontButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:apps.preferences-desktop-font-dec"	},
		FullscreenButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-fullscreen"			},
		WindowedButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-windowed"				},
		BottomAlignButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-bottom-aligned"		},
		TopAlignButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-top-aligned"			},
		LeftAlignButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-left-aligned"			},
		RightAlignButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-right-aligned"			},
	},
}

local _Callbacks =
{
	Open	= nil,
	Reload	= nil,
	Save	= nil,
	Close	= nil,
}

local _TextEditorFonts =
{
	[1] = "Cour-8",
	[2] = "Cour-10",
	[3] = "Cour-12",
}
local _TextEditorCurFont = 1

local _TextEditorCaption = "Text Editor:"
local _TextEditorFileName = ""

local TEXT_EDITOR_STATE_FULLSCREEN		= 0
local TEXT_EDITOR_STATE_WINDOWED		= 1
local TEXT_EDITOR_STATE_BOTTOM_ALIGNED	= 2
local TEXT_EDITOR_STATE_TOP_ALIGNED		= 3
local TEXT_EDITOR_STATE_LEFT_ALIGNED	= 4
local TEXT_EDITOR_STATE_RIGHT_ALIGNED	= 5

local _TextEditorState = TEXT_EDITOR_STATE_WINDOWED
local _TextEditorFlags = 0

local function _TextEditorUpdateControls()

	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl:enable()
	end
	_Ctrls.Buttons.SaveButton.Ctrl:setEnabled(bit.band(_TextEditorFlags, TEXT_EDITOR_FLAGS.SAVE) ~= 0)
	_Ctrls.Buttons.OpenButton.Ctrl:setEnabled(bit.band(_TextEditorFlags, TEXT_EDITOR_FLAGS.OPEN) ~= 0)
	_Ctrls.Buttons.ReloadButton.Ctrl:setEnabled(bit.band(_TextEditorFlags, TEXT_EDITOR_FLAGS.RELOAD) ~= 0)
	if 	_TextEditorCurFont <= 1 then
		_Ctrls.Buttons.DecFontButton.Ctrl:disable()
	end
	if 	_TextEditorCurFont >= table.getn(_TextEditorFonts) then
		_Ctrls.Buttons.IncFontButton.Ctrl:disable()
	end
	
	if _TextEditorState == TEXT_EDITOR_STATE_FULLSCREEN then
		_Ctrls.Buttons.FullscreenButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{1.0,0},{1.0,0}}")
	elseif _TextEditorState == TEXT_EDITOR_STATE_WINDOWED then
		_Ctrls.Buttons.WindowedButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.1,0},{0.2,0},{0.9,0},{0.8,0}}")
	elseif _TextEditorState == TEXT_EDITOR_STATE_BOTTOM_ALIGNED then
		_Ctrls.Buttons.BottomAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.5,0},{1.0,0},{1.0,0}}")
	elseif _TextEditorState == TEXT_EDITOR_STATE_TOP_ALIGNED then
		_Ctrls.Buttons.TopAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.,0},{0.0,0},{1.0,0},{0.5,0}}")
	elseif _TextEditorState == TEXT_EDITOR_STATE_LEFT_ALIGNED then
		_Ctrls.Buttons.LeftAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{0.66,0},{1.0,0}}")
	elseif _TextEditorState == TEXT_EDITOR_STATE_RIGHT_ALIGNED then
		_Ctrls.Buttons.RightAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.33,0},{0.0,0},{1.0,0},{1.0,0}}")
	end
	
end

local function _TextEditorClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.TextEditor.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
		_Callbacks.Close()
	elseif name == "Helper.TextEditor.DecFontButton" then
		if _TextEditorCurFont > 1 then
			_TextEditorCurFont = _TextEditorCurFont - 1
			_Ctrls.Editbox.Ctrl:setFont(_TextEditorFonts[_TextEditorCurFont])
			_Ctrls.Editbox.Ctrl:setText(_Ctrls.Editbox.Ctrl:getText())
		end
	elseif name == "Helper.TextEditor.IncFontButton" then		
		if _TextEditorCurFont < table.getn(_TextEditorFonts) then
			_TextEditorCurFont = _TextEditorCurFont + 1
			_Ctrls.Editbox.Ctrl:setFont(_TextEditorFonts[_TextEditorCurFont])
			_Ctrls.Editbox.Ctrl:setText(_Ctrls.Editbox.Ctrl:getText())
		end
	elseif name == "Helper.TextEditor.SaveButton" then
		if _Callbacks.Save ~= nil then
			_Callbacks.Save()
		end
	elseif name == "Helper.TextEditor.OpenButton" then
		if _Callbacks.Open ~= nil then
			_Callbacks.Open()
		end
	elseif name == "Helper.TextEditor.ReloadButton" then
		if _Callbacks.Reload ~= nil then
			_Callbacks.Reload()
		end
	elseif name == "Helper.TextEditor.FullscreenButton" then
		_TextEditorState = TEXT_EDITOR_STATE_FULLSCREEN
	elseif name == "Helper.TextEditor.WindowedButton" then
		_TextEditorState = TEXT_EDITOR_STATE_WINDOWED
	elseif name == "Helper.TextEditor.BottomAlignButton" then
		_TextEditorState = TEXT_EDITOR_STATE_BOTTOM_ALIGNED
	elseif name == "Helper.TextEditor.TopAlignButton" then
		_TextEditorState = TEXT_EDITOR_STATE_TOP_ALIGNED
	elseif name == "Helper.TextEditor.LeftAlignButton" then
		_TextEditorState = TEXT_EDITOR_STATE_LEFT_ALIGNED
	elseif name == "Helper.TextEditor.RightAlignButton" then
		_TextEditorState = TEXT_EDITOR_STATE_RIGHT_ALIGNED
	end
	_TextEditorUpdateControls()
end

function _TextEditorInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_texteditor.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)

	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.TextEditor.Frame")
	_Ctrls.Editbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.TextEditor.Editbox"), "CEGUI::MultiLineEditbox")
	
	Helper.GUI.initButtons("Helper.TextEditor.", _Ctrls.Buttons, _TextEditorClick)
	
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _TextEditorClick)
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _TextEditorShow(mode, flags, caption, file_name,
	file_open_callback, file_reload_callback, file_save_callback, close_callback)
	
	if not _HelperInit() then
		return false
	end
	
	_TextEditorCaption = caption

	_TextEditorFlags = flags

	_Callbacks.Open		= file_open_callback
	_Callbacks.Reload	= file_reload_callback
	_Callbacks.Save		= file_save_callback
	_Callbacks.Close	= close_callback
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
	
	Helper.GUI.TextEditor.setFileName(file_name, false)
	
	return true
end
Helper.GUI.TextEditor.show = _TextEditorShow

local function _TextEditorSetFileName(file_name, load_text)
	if not _HelperInit() then
		return false
	end
	_TextEditorFileName = file_name
	_Ctrls.Frame.Ctrl:setText(
		_TextEditorCaption..": "..core.extractFileName(file_name))
	if load_text then
		local file = io.open (file_name, "r")
		local text = ""
		if file ~= nil then
			text = file:read("*a")
			file:close()
		end
		Helper.GUI.TextEditor.setText(text)
	end
	_TextEditorUpdateControls()
	return true
end
Helper.GUI.TextEditor.setFileName = _TextEditorSetFileName

local function _TextEditorSetText(text)
	_Ctrls.Editbox.Ctrl:setText(text)
end
Helper.GUI.TextEditor.setText = _TextEditorSetText

local function _TextEditorGetText()
	return _Ctrls.Editbox.Ctrl:getText()
end
Helper.GUI.TextEditor.getText = _TextEditorGetText
