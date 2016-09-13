
local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
		ConvertBtn	= {Ctrl = nil, ImageName = nil	},
		CancelBtn	= {Ctrl = nil, ImageName = nil	},
	},
	FileNameText = {},
	AmplitudeScroll = {},
	AmplitudeLabel = {},
	CheckXSwap = {},
	CheckYSwap = {},
	CheckHeightmapInAlpha = {},
}

local function _ScenedNMapConvOpenAccepted(full_file_name)
	if MyFileSys:existFile(full_file_name) then
		MyCEGUI.setWindowTextUtf8(
			_Ctrls.FileNameText.Ctrl, core.AnsiToUtf8(full_file_name))
		_Ctrls.Buttons.ConvertBtn.Ctrl:setEnabled(true)
	else
		_Ctrls.FileNameText.Ctrl:setText("")
		_Ctrls.Buttons.ConvertBtn.Ctrl:setEnabled(false)
	end
end

local function _ScenedNMapConvGetAmplitude()
	local amplitude = _Ctrls.AmplitudeScroll.Ctrl:getScrollPosition() * 9.0
	amplitude = Helper.getClampedValue(amplitude, 0.0, 9.0)
	return math.floor(amplitude)
end

local function _ScenedNMapConvSaveAccepted(nmap_full_file_name)
	_Ctrls.Root.Ctrl:setVisible(false)
	local hmap_full_file_name = core.Utf8ToAnsi(
		_Ctrls.FileNameText.Ctrl:getText())
	local amplitude = _ScenedNMapConvGetAmplitude()
	local swapX = _Ctrls.CheckXSwap.Ctrl:isSelected()
	local swapY = _Ctrls.CheckYSwap.Ctrl:isSelected()
	local hmapInAlpha = _Ctrls.CheckHeightmapInAlpha.Ctrl:isSelected()
	Helper.convertToNormalMap(
		hmap_full_file_name, nmap_full_file_name, amplitude, swapX, swapY, hmapInAlpha)
end

local function _ScenedNMapConvScrollHandler(args)
	local ampl = _ScenedNMapConvGetAmplitude()
	_Ctrls.AmplitudeLabel.Ctrl:setText(string.format("Amplitude: %d", ampl))
end

local function _ScenedNMapConvStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.NormalMapConverter.CheckXSwap" then
	elseif name == "Scened.NormalMapConverter.CheckYSwap" then
	elseif name == "Scened.NormalMapConverter.CheckHeightmapInAlpha" then
	end
end

local function _ScenedNMapConvWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.NormalMapConverter.OpenBtn" then
		local init_fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		local init_fname = nil
		local filename_utf8 = _Ctrls.FileNameText.Ctrl:getText()
		if string.len(filename_utf8) > 0 then
			local file_name_ansi = core.Utf8ToAnsi(filename_utf8)
			if MyFileSys:existFile(file_name_ansi) then
				init_fpath = core.extractFilePath(file_name_ansi)
				init_fname = core.extractFileName(file_name_ansi)
			end
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Choose image to convert to Normal Map...",
			init_fpath, init_fname,
			SCENED_IMAGE_OPEN_FILE_FILTER, _ScenedNMapConvOpenAccepted)
	elseif name == "Scened.NormalMapConverter.ConvertBtn" then
		local filename_utf8 = _Ctrls.FileNameText.Ctrl:getText()
		if string.len(filename_utf8) > 0 then
			local file_name_ansi = core.Utf8ToAnsi(filename_utf8)
			local fpath = core.extractFilePath(file_name_ansi)
			local fname = core.extractFileName(file_name_ansi, true).."_nmap"..".tga"
			Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save Normal Map to...",
				fpath, fname, {
					[1]		= { Descr = "TGA images", Value = "tga" },
					["tga"]	= { Descr = "TGA images", Value = "tga" },},
				_ScenedNMapConvSaveAccepted)
		end
	elseif name == "Scened.NormalMapConverter.CancelBtn" or
			name == "Scened.NormalMapConverter.Frame"then
		_Ctrls.Root.Ctrl:setVisible(false)
	end
end

function _ScenedNMapConvInit()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_scened_normalmap_converter.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Scened.NormalMapConverter.Frame")
	
	_Ctrls.Frame.Ctrl:subscribeEvent("Clicked", _ScenedNMapConvWidgetClicked)
	
	Helper.GUI.initButtons("Scened.NormalMapConverter.",
		_Ctrls.Buttons, _ScenedNMapConvWidgetClicked)
		
	_Ctrls.FileNameText.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.NormalMapConverter.FileNameText")

	_Ctrls.AmplitudeLabel.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.NormalMapConverter.AmplitudeLabel")

	_Ctrls.AmplitudeScroll.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Scened.NormalMapConverter.AmplitudeScroll"), "CEGUI::Scrollbar")
	_Ctrls.AmplitudeScroll.Ctrl:subscribeEvent("ScrollPosChanged", _ScenedNMapConvScrollHandler)
	_Ctrls.AmplitudeScroll.Ctrl:setScrollPosition(0.25)

	_Ctrls.CheckXSwap.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NormalMapConverter.CheckXSwap"), "CEGUI::Checkbox")
	_Ctrls.CheckXSwap.Ctrl:subscribeEvent("CheckStateChanged", _ScenedNMapConvStateChanged)

	_Ctrls.CheckYSwap.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NormalMapConverter.CheckYSwap"), "CEGUI::Checkbox")
	_Ctrls.CheckYSwap.Ctrl:subscribeEvent("CheckStateChanged", _ScenedNMapConvStateChanged)

	_Ctrls.CheckHeightmapInAlpha.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.NormalMapConverter.CheckHeightmapInAlpha"), "CEGUI::Checkbox")
	_Ctrls.CheckHeightmapInAlpha.Ctrl:subscribeEvent("CheckStateChanged", _ScenedNMapConvStateChanged)
	_Ctrls.CheckHeightmapInAlpha.Ctrl:setSelected(true)

	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _ScenedNMapConvShow()

	if not _HelperInit() then
		return false
	end

	_Ctrls.FileNameText.Ctrl:setText("")
	_Ctrls.Buttons.ConvertBtn.Ctrl:setEnabled(false)	
		
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
	
	return true
end
Helper.GUI.Scened.NormalMapConverterDialog.show = _ScenedNMapConvShow
