
local _Ctrls =
{
	Root = {},
	Frame = {},
	TypeCombo = {},

	Editboxes =
	{
		BaseEditbox	= {},
		AmplEditbox	= {},
		PhaseEditbox= {},
		FreqEditbox	= {},
	},
	Scrolls =
	{
		BaseScroll	= {},
		AmplScroll	= {},
		PhaseScroll	= {},
		FreqScroll	= {},
	},
	Buttons =
	{
		OKBtn		= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
	},
}

local _Resources =
{
}

local _Callbacks =
{
	SetupAccept = nil,
}

local _WaveSetupDialogInitWave = core.wave(core.EFT_SIN, 0.0, 1.0, 0.5, 1.0)

local function _WaveSetupDialogSetWave(func, base, ampl, phase, freq)

	base = Helper.getClampedValue(base, -8.0, 8.0)
	ampl = Helper.getClampedValue(ampl, -8.0, 8.0)
	phase = Helper.getClampedValue(phase, -8.0, 8.0)
	freq = Helper.getClampedValue(freq, -8.0, 8.0)

	_Ctrls.TypeCombo.Ctrl:setItemSelectState(func, true)
	
	_Ctrls.Editboxes.BaseEditbox.Ctrl:setText(string.format("%.3f", base))
	_Ctrls.Editboxes.AmplEditbox.Ctrl:setText(string.format("%.3f", ampl))
	_Ctrls.Editboxes.PhaseEditbox.Ctrl:setText(string.format("%.3f", phase))
	_Ctrls.Editboxes.FreqEditbox.Ctrl:setText(string.format("%.3f", freq))
	
	_Ctrls.Scrolls.BaseScroll.Ctrl:setScrollPosition((base + 8) / 16)
	_Ctrls.Scrolls.AmplScroll.Ctrl:setScrollPosition((ampl + 8) / 16)
	_Ctrls.Scrolls.PhaseScroll.Ctrl:setScrollPosition((phase + 8) /16)
	_Ctrls.Scrolls.FreqScroll.Ctrl:setScrollPosition((freq + 8) / 16)
end

local function _WaveSetupDialogSetupAccept()
	if _Callbacks.SetupAccept == nil then
		return
	end
	local list = _Ctrls.TypeCombo.Ctrl:getDropList()
	local func = list:getItemIndex(list:getFirstSelectedItem())
	local base = tonumber(_Ctrls.Editboxes.BaseEditbox.Ctrl:getText())
	local ampl = tonumber(_Ctrls.Editboxes.AmplEditbox.Ctrl:getText())
	local phase = tonumber(_Ctrls.Editboxes.PhaseEditbox.Ctrl:getText())
	local freq = tonumber(_Ctrls.Editboxes.FreqEditbox.Ctrl:getText())
	_Callbacks.SetupAccept(core.wave(func, base, ampl, phase, freq))
end

local function _WaveSetupDialogClickHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.WaveSetupDialog.OKBtn" then
		_Ctrls.Root.Ctrl:setVisible(false)
		_WaveSetupDialogSetupAccept()
	elseif name == "Helper.WaveSetupDialog.CancelBtn" or
			name == "Helper.WaveSetupDialog.Frame" then
		_Ctrls.Root.Ctrl:setVisible(false)
		_Callbacks.SetupAccept(_WaveSetupDialogInitWave)
	end
end

local function _WaveSetupDialogEditboxHandler(args)
	local list = _Ctrls.TypeCombo.Ctrl:getDropList()
	local func = list:getItemIndex(list:getFirstSelectedItem())
	local base = tonumber(_Ctrls.Editboxes.BaseEditbox.Ctrl:getText())
	local ampl = tonumber(_Ctrls.Editboxes.AmplEditbox.Ctrl:getText())
	local phase = tonumber(_Ctrls.Editboxes.PhaseEditbox.Ctrl:getText())
	local freq = tonumber(_Ctrls.Editboxes.FreqEditbox.Ctrl:getText())
	_WaveSetupDialogSetWave(func, base, ampl, phase, freq)
	_WaveSetupDialogSetupAccept()
end

local function _WaveSetupDialogScrollHandler(args)
	local list = _Ctrls.TypeCombo.Ctrl:getDropList()
	local func = list:getItemIndex(list:getFirstSelectedItem())
	local base = _Ctrls.Scrolls.BaseScroll.Ctrl:getScrollPosition() * 16 - 8
	local ampl = _Ctrls.Scrolls.AmplScroll.Ctrl:getScrollPosition() * 16 - 8
	local phase = _Ctrls.Scrolls.PhaseScroll.Ctrl:getScrollPosition() * 16 -8
	local freq = _Ctrls.Scrolls.FreqScroll.Ctrl:getScrollPosition() * 16 - 8
	_WaveSetupDialogSetWave(func, base, ampl, phase, freq)
	_WaveSetupDialogSetupAccept()
end

local function _WaveSetupDialogComboHandler(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.WaveSetupDialog.TypeCombo" then
		local list = _Ctrls.TypeCombo.Ctrl:getDropList()
		local func = list:getItemIndex(list:getFirstSelectedItem())
		local base = tonumber(_Ctrls.Editboxes.BaseEditbox.Ctrl:getText())
		local ampl = tonumber(_Ctrls.Editboxes.AmplEditbox.Ctrl:getText())
		local phase = tonumber(_Ctrls.Editboxes.PhaseEditbox.Ctrl:getText())
		local freq = tonumber(_Ctrls.Editboxes.FreqEditbox.Ctrl:getText())
		_WaveSetupDialogSetWave(func, base, ampl, phase, freq)
		_WaveSetupDialogSetupAccept()
	end
end

function _WaveSetupDialogInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_wave_setup_dialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)

	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.WaveSetupDialog.Frame")

	_Ctrls.TypeCombo.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.WaveSetupDialog.TypeCombo"), "CEGUI::Combobox")
	_Ctrls.TypeCombo.Ctrl:subscribeEvent("ListSelectionAccepted", _WaveSetupDialogComboHandler)
	for i = 0, core.E_FUNC_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.TypeCombo.Ctrl,
			OPTIONS.HelperGUI.SchemeName, core.getFuncTypeReadableName(i), i, nil)
	end
	
	Helper.GUI.initButtons("Helper.WaveSetupDialog.",
		_Ctrls.Buttons, _WaveSetupDialogClickHandler)
	for key, value in pairs(_Ctrls.Scrolls) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Helper.WaveSetupDialog."..key), "CEGUI::Scrollbar")
		value.Ctrl:subscribeEvent("ScrollPosChanged", _WaveSetupDialogScrollHandler)
	end
	for key, value in pairs(_Ctrls.Editboxes) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Helper.WaveSetupDialog."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _WaveSetupDialogEditboxHandler)
	end
	
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _WaveSetupDialogClickHandler)

	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)

end

local function _WaveSetupDialogShow(caption, flags, init_wave, setup_accept_callback)
	if not _HelperInit() then
		return
	end
	_Callbacks.SetupAccept = setup_accept_callback
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	_WaveSetupDialogInitWave:set(init_wave)
	_WaveSetupDialogSetWave(init_wave.func,
		init_wave.base, init_wave.amplitude, init_wave.phase, init_wave.frequency)
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
end
Helper.GUI.WaveSetupDialog.show = _WaveSetupDialogShow
