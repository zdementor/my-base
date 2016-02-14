
local _Ctrls =
{
	Root = {},
	Frame = {},
	VertexShaderEditbox = {},
	PixelShaderEditbox = {},
	Buttons =
	{
		SaveAsButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save-as"			},
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
		CompileButton		= {Ctrl = nil, ImageName = nil	},
		OKButton			= {Ctrl = nil, ImageName = nil	},
		CancelButton		= {Ctrl = nil, ImageName = nil	},		
	},
	Uniforms =
	{
		ScrollArea = {},
		Checkboxes = {},
	},
	VertexShaderVerCombobox = {},
	PixelShaderVerCombobox = {},
	TagCombobox = {},
	LightsCountSpinner = {},
}

local _Callbacks =
{
	Open	= nil,
	Reload	= nil,
	Save	= nil,
	Cancel	= nil,
	Accept	= nil,
}

local _ScenedShaderEditorFonts =
{
	[1] = "Cour-8",
	[2] = "Cour-10",
	[3] = "Cour-12",
}
local _ScenedShaderEditorCurFont = 2

local _ScenedShaderEditorCaption = "Text Editor:"
local _ScenedShaderEditorFileName = ""

local SHADER_EDITOR_STATE_FULLSCREEN		= 0
local SHADER_EDITOR_STATE_WINDOWED		= 1
local SHADER_EDITOR_STATE_BOTTOM_ALIGNED	= 2
local SHADER_EDITOR_STATE_TOP_ALIGNED		= 3
local SHADER_EDITOR_STATE_LEFT_ALIGNED	= 4
local SHADER_EDITOR_STATE_RIGHT_ALIGNED	= 5

local _ScenedShaderEditorState = SHADER_EDITOR_STATE_WINDOWED
local _ScenedShaderEditorFlags = 0

local _ScenedShaderSources = {}

local function _ScenedShaderEditorUpdateControls()

	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl:enable()
	end

	if 	_ScenedShaderEditorCurFont <= 1 then
		_Ctrls.Buttons.DecFontButton.Ctrl:disable()
	end
	if 	_ScenedShaderEditorCurFont >= table.getn(_ScenedShaderEditorFonts) then
		_Ctrls.Buttons.IncFontButton.Ctrl:disable()
	end
	
	if _ScenedShaderEditorFileName == nil then
		--_Ctrls.Buttons.ReloadButton.Ctrl:disable()
	end

	if _ScenedShaderEditorState == SHADER_EDITOR_STATE_FULLSCREEN then
		_Ctrls.Buttons.FullscreenButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{1.0,0},{1.0,0}}")
	elseif _ScenedShaderEditorState == SHADER_EDITOR_STATE_WINDOWED then
		_Ctrls.Buttons.WindowedButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.1,0},{0.05,0},{0.9,0},{0.95,0}}")
	elseif _ScenedShaderEditorState == SHADER_EDITOR_STATE_BOTTOM_ALIGNED then
		_Ctrls.Buttons.BottomAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.5,0},{1.0,0},{1.0,0}}")
	elseif _ScenedShaderEditorState == SHADER_EDITOR_STATE_TOP_ALIGNED then
		_Ctrls.Buttons.TopAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.,0},{0.0,0},{1.0,0},{0.5,0}}")
	elseif _ScenedShaderEditorState == SHADER_EDITOR_STATE_LEFT_ALIGNED then
		_Ctrls.Buttons.LeftAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{0.66,0},{1.0,0}}")
	elseif _ScenedShaderEditorState == SHADER_EDITOR_STATE_RIGHT_ALIGNED then
		_Ctrls.Buttons.RightAlignButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.33,0},{0.0,0},{1.0,0},{1.0,0}}")
	end
	
end

local function _ScenedShaderEditorUpdateControlsAll()
	_ScenedShaderEditorUpdateControls()
	
	for uni_type = vid.EUT_NONE + 1, vid.E_UNIFORM_TYPE_COUNT - 1 do
		local uni = bit.band(_ScenedShaderSources.Uniforms, vid.getUniformFlag(uni_type))
		_Ctrls.Uniforms.Checkboxes[vid.getUniformName(uni_type)].Ctrl:setSelected(uni ~= 0)
	end

	_Ctrls.VertexShaderEditbox.Ctrl:setText(_ScenedShaderSources.Vertex.Source)
	_Ctrls.PixelShaderEditbox.Ctrl:setText(_ScenedShaderSources.Pixel.Source)
	
	_Ctrls.LightsCountSpinner.Ctrl:setMinimumValue(0)
	_Ctrls.LightsCountSpinner.Ctrl:setMaximumValue(ShaderGen.getMaxLightsCount())
	_Ctrls.LightsCountSpinner.Ctrl:setStepSize(1)
	_Ctrls.LightsCountSpinner.Ctrl:setCurrentValue(_ScenedShaderSources.LightsCount)

	local tag_index = 0
	if _ScenedShaderSources.Tag ~= nil then
		for i = 0, ShaderGen.getTagsCount() - 1 do
			if _ScenedShaderSources.Tag == ShaderGen.getTag(i) then
				tag_index = i + 1
				break
			end
		end
	end
	_Ctrls.TagCombobox.Ctrl:setItemSelectState(tag_index, true)

	_Ctrls.VertexShaderVerCombobox.Ctrl:setItemSelectState(_ScenedShaderSources.Vertex.Ver, true)
	_Ctrls.PixelShaderVerCombobox.Ctrl:setItemSelectState(_ScenedShaderSources.Pixel.Ver, true)
	
end

local function _ScenedShaderEditorReadShaderSources(file_name)
	if file_name ~= nil then
		_ScenedShaderSources= nil
		local tag = ShaderGen.getCurrentTag()
		local progInfo = vid.loadGPUProgramInfo(file_name, tag)
		local unforms = 0
		local lights_count = progInfo.LightsCount
		local vertex_file_name = ""
		local vertex_source = ""
		local vertex_ver = 0
		local pixel_file_name = ""
		local pixel_source = ""
		local pixel_ver = 0
		if progInfo.TaggedShadersCount > 0 then
			for i = 0, progInfo.TaggedShadersCount - 1 do
				local shaderInfo = progInfo:getTaggedShaderInfo(i)
				if shaderInfo.Driver == MyDriver:getDriverType() then
					tag = shaderInfo:getTag()
					uniforms = shaderInfo.Uniforms
					vertex_file_name = shaderInfo:getVertexFileName()
					vertex_ver = shaderInfo.VertexVer
					pixel_file_name = shaderInfo:getPixelFileName()
					pixel_ver = shaderInfo.PixelVer
					break
				end
			end
		else
			for i = 0, progInfo.OtherShadersCount - 1 do
				local shaderInfo = progInfo:getOtherShaderInfo(i)
				if shaderInfo.Driver == MyDriver:getDriverType() then
					tag = shaderInfo:getTag()
					uniforms = shaderInfo.Uniforms
					vertex_file_name = shaderInfo:getVertexFileName()
					vertex_ver = shaderInfo.VertexVer
					pixel_file_name = shaderInfo:getPixelFileName()
					pixel_ver = shaderInfo.PixelVer
					break
				end
			end
		end

		if MyFileSys:existFile(vertex_file_name) then
			local file = io.open (vertex_file_name, "r")
			if file ~= nil then
				for line in file:lines() do 
					vertex_source = vertex_source..line.."\n"
				end
				file:close()
			end
		end
		if MyFileSys:existFile(pixel_file_name) then
			local file = io.open (pixel_file_name, "r")
			if file ~= nil then
				for line in file:lines() do 
					pixel_source = pixel_source..line.."\n"
				end
				file:close()
			end
		end
		_ScenedShaderSources = {
			LightsCount = lights_count,
			Uniforms = uniforms,
			Tag = tag,
			Vertex = { Source = vertex_source, Ver = vertex_ver},
			Pixel  = { Source = pixel_source, Ver = pixel_ver},
			}
	end
end

local function _ScenedShaderEditorOpenCallback(full_file_name)
	Helper.GUI.Scened.ShaderEditor.setFileName(full_file_name, true)
end

local function _ScenedShaderEditorSaveCallback(full_file_name)
	local sources = Helper.GUI.Scened.ShaderEditor.getSources()
	if sources ~= nil then
		local progInfo = vid.loadGPUProgramInfo(full_file_name, nil)
		if progInfo == nil then
			progInfo = vid.initIdentityGPUProgramInfo(sources.LightsCount)
		else
			vid.setGPUProgramInfoLightsCount(progInfo, sources.LightsCount)
		end
		local file_path = core.extractFilePath(full_file_name)
		local file_name = core.extractFileName(full_file_name, true)
		file_name = file_name.."-"..vid.getDriverTypeName(MyDriver:getDriverType())
		if sources.Tag ~= nil then
			file_name = file_name.."-"..sources.Tag
		end
		local vertex_file_name = file_name..".vsh"
		local pixel_file_name = file_name..".psh"
		vid.appendGPUProgramInfo(progInfo,
			sources.Uniforms, MyDriver:getDriverType(),
			sources.Tag,
			sources.Vertex.Ver, vertex_file_name,
			sources.Pixel.Ver, pixel_file_name)
		vid.writeGPUProgramInfo(full_file_name, progInfo)
		local file = nil
		file = io.open (file_path..vertex_file_name, "w")
		if file ~= nil then
			file:write(sources.Vertex.Source)
			file:close()
		end
		file = io.open (file_path..pixel_file_name, "w")
		if file ~= nil then
			file:write(sources.Pixel.Source)
			file:close()
		end
	end
	Helper.GUI.Scened.ShaderEditor.setFileName(full_file_name, false)
end

local function _ScenedShaderEditorReloadCallback()
end

local function _ScenedShaderEditorAcceptCallback()
end

local function _ScenedShaderEditorCancelCallback()
end

local function _ScenedShaderEditorClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ShaderEditor.Frame" or name == "Helper.ShaderEditor.CancelButton" then
		_Ctrls.Root.Ctrl:setVisible(false)
		if _Callbacks.Cancel ~= nil then
			_Callbacks.Cancel(_ScenedShaderEditorCancelCallback)
		end
	elseif name == "Helper.ShaderEditor.OKButton" then
		_Ctrls.Root.Ctrl:setVisible(false)
		if _Callbacks.Accept ~= nil then
			_Callbacks.Accept(_ScenedShaderEditorAcceptCallback)
		end
	elseif name == "Helper.ShaderEditor.DecFontButton" then
		if _ScenedShaderEditorCurFont > 1 then
			_ScenedShaderEditorCurFont = _ScenedShaderEditorCurFont - 1
			_Ctrls.PixelShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
			_Ctrls.VertexShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
			
			_Ctrls.PixelShaderEditbox.Ctrl:setText(_Ctrls.PixelShaderEditbox.Ctrl:getText())
			_Ctrls.VertexShaderEditbox.Ctrl:setText(_Ctrls.VertexShaderEditbox.Ctrl:getText())
		end
	elseif name == "Helper.ShaderEditor.IncFontButton" then		
		if _ScenedShaderEditorCurFont < table.getn(_ScenedShaderEditorFonts) then
			_ScenedShaderEditorCurFont = _ScenedShaderEditorCurFont + 1
			_Ctrls.PixelShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
			_Ctrls.VertexShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
			
			_Ctrls.PixelShaderEditbox.Ctrl:setText(_Ctrls.PixelShaderEditbox.Ctrl:getText())
			_Ctrls.VertexShaderEditbox.Ctrl:setText(_Ctrls.VertexShaderEditbox.Ctrl:getText())
		end
	elseif name == "Helper.ShaderEditor.SaveAsButton" then
		if _Callbacks.Save ~= nil then
			_ScenedShaderSources.Vertex.Source = _Ctrls.VertexShaderEditbox.Ctrl:getText()
			_ScenedShaderSources.Pixel.Source = _Ctrls.PixelShaderEditbox.Ctrl:getText()
			_Callbacks.Save(_ScenedShaderEditorSaveCallback)
		end
	elseif name == "Helper.ShaderEditor.OpenButton" then
		if _Callbacks.Open ~= nil then
			_Callbacks.Open(_ScenedShaderEditorOpenCallback)
		end
	elseif name == "Helper.ShaderEditor.ReloadButton" then
		if _Callbacks.Reload ~= nil then
			_Callbacks.Reload(_ScenedShaderEditorReloadCallback)
		end
	elseif name == "Helper.ShaderEditor.FullscreenButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_FULLSCREEN
	elseif name == "Helper.ShaderEditor.WindowedButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_WINDOWED
	elseif name == "Helper.ShaderEditor.BottomAlignButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_BOTTOM_ALIGNED
	elseif name == "Helper.ShaderEditor.TopAlignButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_TOP_ALIGNED
	elseif name == "Helper.ShaderEditor.LeftAlignButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_LEFT_ALIGNED
	elseif name == "Helper.ShaderEditor.RightAlignButton" then
		_ScenedShaderEditorState = SHADER_EDITOR_STATE_RIGHT_ALIGNED
	elseif name == "Helper.ShaderEditor.CompileButton" then
		MyLogger:logInfo("Compiling GPU program...")
		local v_src = _Ctrls.VertexShaderEditbox.Ctrl:getText()
		local p_src = _Ctrls.PixelShaderEditbox.Ctrl:getText()
		if not MyDriver:compileGPUSources(
				_ScenedShaderSources.Uniforms, _ScenedShaderSources.LightsCount,
				_ScenedShaderSources.Vertex.Ver, v_src,
				_ScenedShaderSources.Pixel.Ver, p_src) then
			MyLogger:logInfo(" GPU program compiled with errors")
		else
			_ScenedShaderSources.Vertex.Source = v_src
			_ScenedShaderSources.Pixel.Source = p_src
			MyLogger:logInfo(" GPU program compiled")
		end
	end
	_ScenedShaderEditorUpdateControls()
end

local function _ScenedShaderEditorSelectionAccepted(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ShaderEditor.TagCombobox" then
		local index = _Ctrls.TagCombobox.Ctrl:getItemIndex(
			_Ctrls.TagCombobox.Ctrl:getSelectedItem())
		local tag = ""
		if index > 0 then
			tag = ShaderGen.getTag(index - 1)
		end
		if _ScenedShaderSources ~= nil then
			_ScenedShaderSources.Tag = tag
		end
	elseif name == "Helper.ShaderEditor.VertexShaderVerCombobox" then
		local index = _Ctrls.VertexShaderVerCombobox.Ctrl:getItemIndex(
			_Ctrls.VertexShaderVerCombobox.Ctrl:getSelectedItem())
		_ScenedShaderSources.Vertex.Ver = index
	elseif name == "Helper.ShaderEditor.PixelShaderVerCombobox" then
		local index = _Ctrls.PixelShaderVerCombobox.Ctrl:getItemIndex(
			_Ctrls.PixelShaderVerCombobox.Ctrl:getSelectedItem())
		_ScenedShaderSources.Pixel.Ver = index
	end
end

local function _ScenedShaderEditorValueChanged(args)
	local pass = _ScenedMaterialGetSelectedPass()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ShaderEditor.LightsCountSpinner" then
		local lights = _Ctrls.LightsCountSpinner.Ctrl:getCurrentValue()
		_ScenedShaderSources.LightsCount = lights
	end
end

function _ScenedShaderEditorInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_scened_shadereditor.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)

	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.ShaderEditor.Frame")
	
	_Ctrls.Uniforms.ScrollArea.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow(
			"Helper.ShaderEditor.Uniforms.ScrollArea"), "CEGUI::ScrollablePane")
	local content = tolua.cast(_Ctrls.Uniforms.ScrollArea.Ctrl:getContentPane(), "CEGUI::Window")
	local uni_font = _Ctrls.Uniforms.ScrollArea.Ctrl:getProperty("Font")
	local uni_y = 5
	local uni_h = 18
	for uni_type = vid.EUT_NONE + 1, vid.E_UNIFORM_TYPE_COUNT - 1 do
		local checkbox = tolua.cast(
			CEGUIWinMgr:createWindow(OPTIONS.ScenedGUI.SchemeName.."/Checkbox",
				"Helper.ShaderEditor.Uniforms."..vid.getUniformName(uni_type)),
			"CEGUI::Checkbox")
		content:addChildWindow(checkbox)
		checkbox:setVisible(true)
		checkbox:setText(vid.getUniformReadableName(uni_type))
		checkbox:setProperty("Font", uni_font)
		checkbox:setProperty("UnifiedAreaRect",
			string.format("{{0.0,0},{0.0,%d},{1.0,0},{0.0,%d}}", uni_y, uni_y + uni_h))
		uni_y = uni_y + uni_h
		_Ctrls.Uniforms.Checkboxes[vid.getUniformName(uni_type)] = {}
		_Ctrls.Uniforms.Checkboxes[vid.getUniformName(uni_type)].Ctrl = checkbox
	end
	-- to correct vertical scrollbar calculation
	_Ctrls.Uniforms.ScrollArea.Ctrl:setShowVertScrollbar(false)
	_Ctrls.Uniforms.ScrollArea.Ctrl:setShowVertScrollbar(true)

	_Ctrls.VertexShaderEditbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.VertexShaderEditbox"), "CEGUI::MultiLineEditbox")
	_Ctrls.PixelShaderEditbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.PixelShaderEditbox"), "CEGUI::MultiLineEditbox")
	
	Helper.GUI.initButtons("Helper.ShaderEditor.", _Ctrls.Buttons, _ScenedShaderEditorClick)
	
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _ScenedShaderEditorClick)

	_Ctrls.VertexShaderVerCombobox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.VertexShaderVerCombobox"), "CEGUI::Combobox")
	_Ctrls.VertexShaderVerCombobox.Ctrl:subscribeEvent("ListSelectionAccepted",
		_ScenedShaderEditorSelectionAccepted)
		
	_Ctrls.PixelShaderVerCombobox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.PixelShaderVerCombobox"), "CEGUI::Combobox")
	_Ctrls.PixelShaderVerCombobox.Ctrl:subscribeEvent("ListSelectionAccepted",
		_ScenedShaderEditorSelectionAccepted)
	
	_Ctrls.TagCombobox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.TagCombobox"), "CEGUI::Combobox")
	_Ctrls.TagCombobox.Ctrl:subscribeEvent("ListSelectionAccepted",
		_ScenedShaderEditorSelectionAccepted)

	_Ctrls.TagCombobox.Ctrl:resetList()
	Helper.GUI.addListboxItem(_Ctrls.TagCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName,
		"None", 0, nil)
	for i = 0, ShaderGen.getTagsCount() - 1 do
		Helper.GUI.addListboxItem(_Ctrls.TagCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName,
			ShaderGen.getTag(i), i + 1, nil)
	end
	_Ctrls.TagCombobox.Ctrl:setItemSelectState(0, true)
	
	_Ctrls.VertexShaderVerCombobox.Ctrl:resetList()
	for i = 0, vid.E_VERTEX_SHADER_VERSION_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.VertexShaderVerCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName,
			vid.getVertexShaderVersionReadableName(i), i, nil)
	end

	_Ctrls.PixelShaderVerCombobox.Ctrl:resetList()
	for i = 0, vid.E_PIXEL_SHADER_VERSION_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.PixelShaderVerCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName,
			vid.getPixelShaderVersionReadableName(i), i, nil)
	end
	
	_Ctrls.LightsCountSpinner.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ShaderEditor.LightsCountSpinner"), "CEGUI::Spinner")
	_Ctrls.LightsCountSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedShaderEditorValueChanged)
	
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _ScenedShaderEditorShow(mode, flags, caption, file_name, sources,
	open_callback, reload_callback, save_callback, accept_callback, cancel_callback)
	
	if not _HelperInit() then
		return false
	end
	
	_ScenedShaderEditorCaption = caption

	_ScenedShaderEditorFlags = flags

	_Callbacks.Open		= open_callback
	_Callbacks.Reload	= reload_callback
	_Callbacks.Save		= save_callback
	_Callbacks.Accept	= accept_callback
	_Callbacks.Cancel	= cancel_callback
	
	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
	
	_ScenedShaderSources= nil
	if sources ~= nil then
		_ScenedShaderSources = sources
		file_name = nil
	end
	
	_Ctrls.PixelShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
	_Ctrls.VertexShaderEditbox.Ctrl:setFont(_ScenedShaderEditorFonts[_ScenedShaderEditorCurFont])
	
	Helper.GUI.Scened.ShaderEditor.setFileName(file_name, true)
	
	return true
end
Helper.GUI.Scened.ShaderEditor.show = _ScenedShaderEditorShow

local function _ScenedShaderEditorSetFileName(file_name, load_text)
	if not _HelperInit() then
		return false
	end
	
	if file_name ~= nil then
		_ScenedShaderEditorFileName = file_name
		_Ctrls.Frame.Ctrl:setText(
			_ScenedShaderEditorCaption..": "..core.extractFileName(file_name))
		if load_text then
			_ScenedShaderEditorReadShaderSources(file_name)
		end
	else
		_ScenedShaderEditorFileName = nil
		_Ctrls.Frame.Ctrl:setText(_ScenedShaderEditorCaption..": Noname")
	end
	_ScenedShaderEditorUpdateControlsAll()
	return true
end
Helper.GUI.Scened.ShaderEditor.setFileName = _ScenedShaderEditorSetFileName

local function _ScenedShaderEditorGetFileName(file_name, load_text)
	if not _HelperInit() then
		return nil
	end
	return _ScenedShaderEditorFileName
end
Helper.GUI.Scened.ShaderEditor.getFileName = _ScenedShaderEditorGetFileName

local function _ScenedShaderEditorSetSources(sources)
	if not _HelperInit() then
		return false
	end
	_ScenedShaderSources = sources
	Helper.GUI.Scened.ShaderEditor.setFileName(nil)
	return true
end
Helper.GUI.Scened.ShaderEditor.setSources = _ScenedShaderEditorSetSources

local function _ScenedShaderEditorGetSources(sources)
	if not _HelperInit() then
		return nil
	end
	return _ScenedShaderSources
end
Helper.GUI.Scened.ShaderEditor.getSources = _ScenedShaderEditorGetSources
