
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OKBtn		= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
	},
	Scrolls =
	{
		RedScroll = {},
		GreenScroll = {},
		BlueScroll = {},
		AlphaScroll = {},
	},
	Spinners =
	{
		RedSpinner = {},
		GreenSpinner = {},
		BlueSpinner = {},
		AlphaSpinner = {},
	},
	ColorImage = {},
}

local _Resources =
{
	Texture = nil,
}

local _Callbacks =
{
	SelectionAccept = nil,
}

local _ColorSelectionDialogInitColor = img.SColor(255,0,0,0)

local _TmpColor = img.SColor()

local function _ColorSelectionDialogClickHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ColorSelectionDialog.OKBtn" then
		local r = _Ctrls.Spinners.RedSpinner.Ctrl:getCurrentValue()
		local g = _Ctrls.Spinners.GreenSpinner.Ctrl:getCurrentValue()
		local b = _Ctrls.Spinners.BlueSpinner.Ctrl:getCurrentValue()
		local a = _Ctrls.Spinners.AlphaSpinner.Ctrl:getCurrentValue()
		r = Helper.getClampedValue(r, 0, 255)
		g = Helper.getClampedValue(g, 0, 255)
		b = Helper.getClampedValue(b, 0, 255)
		a = Helper.getClampedValue(a, 0, 255)
		_Ctrls.Root.Ctrl:setVisible(false)
		_TmpColor:set(a, r, g, b)
		_Callbacks.SelectionAccept(_TmpColor)
	elseif name == "Helper.ColorSelectionDialog.CancelBtn" or
			name == "Helper.ColorSelectionDialog.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
		_Callbacks.SelectionAccept(_ColorSelectionDialogInitColor)
	end
end

local function _ColorSelectionDialogSetColor(col)
	local r = col.red
	local g = col.green
	local b = col.blue
	local a = col.alpha
	_Ctrls.Scrolls.RedScroll.Ctrl:setScrollPosition(r / 255)
	_Ctrls.Scrolls.GreenScroll.Ctrl:setScrollPosition(g / 255)
	_Ctrls.Scrolls.BlueScroll.Ctrl:setScrollPosition(b / 255)
	_Ctrls.Scrolls.AlphaScroll.Ctrl:setScrollPosition(a / 255)
	_Ctrls.Spinners.RedSpinner.Ctrl:setCurrentValue(r)
	_Ctrls.Spinners.GreenSpinner.Ctrl:setCurrentValue(g)
	_Ctrls.Spinners.BlueSpinner.Ctrl:setCurrentValue(b)
	_Ctrls.Spinners.AlphaSpinner.Ctrl:setCurrentValue(a)
	
	if r == 0 then
		_Ctrls.Spinners.RedSpinner.Ctrl:setText("0")
	end
	if g == 0 then
		_Ctrls.Spinners.GreenSpinner.Ctrl:setText("0")
	end
	if b == 0 then
		_Ctrls.Spinners.BlueSpinner.Ctrl:setText("0")
	end
	if a == 0 then
		_Ctrls.Spinners.AlphaSpinner.Ctrl:setText("0")
	end
	
	Helper.GUI.fillCEGUITextureWithColor(_Resources.Texture, col)
end

local function _ColorSelectionDialogScrollHandler(args)
	local r = _Ctrls.Scrolls.RedScroll.Ctrl:getScrollPosition() * 255.0 + 0.5
	local g = _Ctrls.Scrolls.GreenScroll.Ctrl:getScrollPosition() * 255.0 + 0.5
	local b = _Ctrls.Scrolls.BlueScroll.Ctrl:getScrollPosition() * 255.0 + 0.5
	local a = _Ctrls.Scrolls.AlphaScroll.Ctrl:getScrollPosition() * 255.0 + 0.5
	r = Helper.getClampedValue(r, 0, 255)
	g = Helper.getClampedValue(g, 0, 255)
	b = Helper.getClampedValue(b, 0, 255)
	a = Helper.getClampedValue(a, 0, 255)
	_TmpColor:set(a, r, g, b)
	_ColorSelectionDialogSetColor(_TmpColor)
	if _Callbacks.SelectionAccept ~= nil then
		_TmpColor:set(a, r, g, b)
		_Callbacks.SelectionAccept(_TmpColor)
	end
end

local function _ColorSelectionDialogSpinnerHandler(args)
	local r = _Ctrls.Spinners.RedSpinner.Ctrl:getCurrentValue()
	local g = _Ctrls.Spinners.GreenSpinner.Ctrl:getCurrentValue()
	local b = _Ctrls.Spinners.BlueSpinner.Ctrl:getCurrentValue()
	local a = _Ctrls.Spinners.AlphaSpinner.Ctrl:getCurrentValue()
	r = Helper.getClampedValue(r, 0, 255)
	g = Helper.getClampedValue(g, 0, 255)
	b = Helper.getClampedValue(b, 0, 255)
	a = Helper.getClampedValue(a, 0, 255)
	_TmpColor:set(a, r, g, b)
	_ColorSelectionDialogSetColor(_TmpColor)
	if _Callbacks.SelectionAccept ~= nil then
		_Callbacks.SelectionAccept(_TmpColor)
	end
end

function _ColorSelectionDialogInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_color_selection_dialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.ColorSelectionDialog.Frame")
	
	Helper.GUI.initButtons("Helper.ColorSelectionDialog.",
		_Ctrls.Buttons, _ColorSelectionDialogClickHandler)
	for key, value in pairs(_Ctrls.Scrolls) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Helper.ColorSelectionDialog."..key), "CEGUI::Scrollbar")
		value.Ctrl:subscribeEvent("ScrollPosChanged", _ColorSelectionDialogScrollHandler)
	end
	for key, value in pairs(_Ctrls.Spinners) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Helper.ColorSelectionDialog."..key), "CEGUI::Spinner")
		value.Ctrl:subscribeEvent("ValueChanged", _ColorSelectionDialogSpinnerHandler)
	end		
	
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _ColorSelectionDialogClickHandler)
	
	_Ctrls.ColorImage.Ctrl = CEGUIWinMgr:getWindow("Helper.ColorSelectionDialog.ColorImage")
	
	local cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	_Resources.Texture = cegui_tex

	_TmpColor:set(255, 0, 0, 0)
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, _TmpColor)	

	local imageset = CEGUIImgsetMgr:create("Helper.ColorSelectionDialog", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Ctrls.ColorImage.Ctrl:setProperty(
		"Image", "set:Helper.ColorSelectionDialog image:ColorImage")
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
	
end

local function _ColorSelectionDialogShow(caption, flags, init_color, selection_accept_callback)
	if not _HelperInit() then
		return
	end
	_Callbacks.SelectionAccept = selection_accept_callback
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	_ColorSelectionDialogInitColor.color = init_color.color
	_ColorSelectionDialogSetColor(_ColorSelectionDialogInitColor)

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.ColorSelectionDialog.show = _ColorSelectionDialogShow
