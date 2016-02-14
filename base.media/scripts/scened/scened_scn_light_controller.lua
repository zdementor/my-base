
local _Ctrls =
{
	Root = {},
	TabControl = {},
	Tabs =
	{
		Props =
		{
			Caption = "Properties",
			Root = {},
			ScrollArea = {},
			Checks =
			{
				DynamicCheck = {},
				EnabledCheck = {},
				CastShadowsCheck = {},
				IntensityWaveEnabledCheck = {},
			},
			TypeCombo = {},
			IntensityWaveFuncLabel = {},
			ColorImages =
			{
				DiffuseImage = {},
				AmbientImage = {},
				SpecularImage = {},
			},
			Buttons =
			{
				DiffuseBtn = {},
				AmbientBtn = {},
				SpecularBtn= {},
				IntensityWaveBtn = {},
			},
			Spinners =
			{
				RadiusSpinner = {},
				LightedVolumeCenterOffsetXSpinner = {},
				LightedVolumeCenterOffsetYSpinner = {},
				LightedVolumeCenterOffsetZSpinner = {},
				LightedVolumeSizeChangeXSpinner = {},
				LightedVolumeSizeChangeYSpinner = {},
				LightedVolumeSizeChangeZSpinner = {},
			},
		},
	}
}

local _Resources =
{
	Textures =
	{
		ColorImages =
		{
			DiffuseImage = nil,
			AmbientImage = nil,
			SpecularImage = nil,
		}
	},
}

local _ColorTbl =
{
	DiffuseImage = img.SColor(),
	AmbientImage = img.SColor(),
	SpecularImage = img.SColor(),
}

local _Light = vid.SLight()
local _LightedVolumeCenterOffset = core.vector3df()
local _LightedVolumeSizeChange = core.vector3df()

local function _ScenedLightGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local light_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_LIGHT_SCENE_NODE then
			light_scene_node = tolua.cast(scene_node, "scn::ILightSceneNode")
		end
	end
	return light_scene_node
end

local function _ScenedLightUpdate()
end

local function _ScenedUpdateLightControls()
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(light_scene_node ~= nil)
	if light_scene_node == nil then
		return
	end
	
	_Light:set(light_scene_node:getLightParameters())
	_LightedVolumeCenterOffset:set(light_scene_node:getLightedVolumeCenterOffset())
	_LightedVolumeSizeChange:set(light_scene_node:getLightedVolumeSizeChange())

	_Ctrls.Tabs.Props.ScrollArea.Ctrl:setEnabled(_Light.Enabled)
	
	if _Ctrls.Tabs.Props.Checks.EnabledCheck.Ctrl:isSelected() ~= _Light.Enabled then
		_Ctrls.Tabs.Props.Checks.EnabledCheck.Ctrl:setSelected(_Light.Enabled)
	end

	if _Ctrls.Tabs.Props.Checks.DynamicCheck.Ctrl:isSelected() ~= _Light.Dynamic then
		_Ctrls.Tabs.Props.Checks.DynamicCheck.Ctrl:setSelected(_Light.Dynamic)
	end
	
	if _Ctrls.Tabs.Props.Checks.IntensityWaveEnabledCheck.Ctrl:isSelected() ~= _Light.IntensityWaveEnabled then
		_Ctrls.Tabs.Props.Checks.IntensityWaveEnabledCheck.Ctrl:setSelected(_Light.IntensityWaveEnabled)
	end

	if _Ctrls.Tabs.Props.Checks.CastShadowsCheck.Ctrl:isSelected() ~= _Light.CastShadows then
		_Ctrls.Tabs.Props.Checks.CastShadowsCheck.Ctrl:setSelected(_Light.CastShadows)
	end
	
	_Ctrls.Tabs.Props.IntensityWaveFuncLabel.Ctrl:setText(string.format("%s\n{%.2f, %.2f, %.2f, %.2f}",
		core.getFuncTypeReadableName(_Light.IntensityWave.func),
		_Light.IntensityWave.base, _Light.IntensityWave.amplitude, _Light.IntensityWave.phase, _Light.IntensityWave.frequency))

	_ColorTbl.DiffuseImage:set(_Light.DiffuseColor)
	_ColorTbl.AmbientImage:set(_Light.AmbientColor)
	_ColorTbl.SpecularImage:set(_Light.SpecularColor)
	for key, value in pairs(_Resources.Textures.ColorImages) do
		if _ColorTbl[key] ~= nil then
			Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.ColorImages[key], _ColorTbl[key])
		else
			MyLogger:logInfo(string.format("Incorrect ColorImage in Scene Props (key='%s')", key))
		end
	end
	
	_Ctrls.Tabs.Props.Spinners.RadiusSpinner.Ctrl:setCurrentValue(_Light.Radius)
	
	_Ctrls.Tabs.Props.TypeCombo.Ctrl:setItemSelectState(_Light.Type, true)
	
	_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetXSpinner.Ctrl:setCurrentValue(_LightedVolumeCenterOffset.x)
	_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetYSpinner.Ctrl:setCurrentValue(_LightedVolumeCenterOffset.y)
	_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetZSpinner.Ctrl:setCurrentValue(_LightedVolumeCenterOffset.z)
	
	_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeXSpinner.Ctrl:setCurrentValue(_LightedVolumeSizeChange.x)
	_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeYSpinner.Ctrl:setCurrentValue(_LightedVolumeSizeChange.y)
	_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeZSpinner.Ctrl:setCurrentValue(_LightedVolumeSizeChange.z)
		
end

local function _ScenedLightCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	if name == "Scened.Scene.Light.Props.EnabledCheck" then
		_Light.Enabled = _Ctrls.Tabs.Props.Checks.EnabledCheck.Ctrl:isSelected()
		light_scene_node:setLightParameters(_Light)
	elseif name == "Scened.Scene.Light.Props.DynamicCheck" then
		_Light.Dynamic = _Ctrls.Tabs.Props.Checks.DynamicCheck.Ctrl:isSelected()
		light_scene_node:setLightParameters(_Light)
	elseif name == "Scened.Scene.Light.Props.IntensityWaveEnabledCheck" then
		_Light.IntensityWaveEnabled = _Ctrls.Tabs.Props.Checks.IntensityWaveEnabledCheck.Ctrl:isSelected()
		light_scene_node:setLightParameters(_Light)
	elseif name == "Scened.Scene.Light.Props.CastShadowsCheck" then
		_Light.CastShadows = _Ctrls.Tabs.Props.Checks.CastShadowsCheck.Ctrl:isSelected()
		light_scene_node:setLightParameters(_Light)
	end
	_ScenedUpdateLightControls()
end

local function _ScenedLightSetDiffuseColor(color)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	_Light.DiffuseColor:set(color)
	light_scene_node:setLightParameters(_Light)
	_ScenedUpdateLightControls()
end

local function _ScenedLightSetAmbientColor(color)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	_Light.AmbientColor:set(color)
	light_scene_node:setLightParameters(_Light)
	_ScenedUpdateLightControls()
end

local function _ScenedLightSetSpecularColor(color)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	_Light.SpecularColor:set(color)
	light_scene_node:setLightParameters(_Light)
	_ScenedUpdateLightControls()
end

function _ScenedLightSetIntensityWaveFunc(wave)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	_Light.IntensityWave:set(wave)
	light_scene_node:setLightParameters(_Light)
	_ScenedUpdateLightControls()
end

local function _ScenedLightWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Light.Props.DiffuseBtn" then
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, _ColorTbl.DiffuseImage, _ScenedLightSetDiffuseColor)
	elseif name == "Scened.Scene.Light.Props.AmbientBtn" then
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, _ColorTbl.AmbientImage, _ScenedLightSetAmbientColor)
	elseif name == "Scened.Scene.Light.Props.SpecularBtn" then
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, _ColorTbl.SpecularImage, _ScenedLightSetSpecularColor)
	elseif name == "Scened.Scene.Light.Props.IntensityWaveBtn" then
		Helper.GUI.WaveSetupDialog.show(
			"Intensity function", 0, _Light.IntensityWave, _ScenedLightSetIntensityWaveFunc)
	end
end

local function _ScenedLightValueChanged(args)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local spinner = tolua.cast(we.window, "CEGUI::Spinner")
	if name =="Scened.Scene.Light.Props.RadiusSpinner" then
		_Light.Radius = spinner:getCurrentValue()
		light_scene_node:setLightParameters(_Light)
	elseif name =="Scened.Scene.Light.Props.LightedVolumeCenterOffsetXSpinner"
			or name =="Scened.Scene.Light.Props.LightedVolumeCenterOffsetYSpinner"
			or name =="Scened.Scene.Light.Props.LightedVolumeCenterOffsetZSpinner" then
		_LightedVolumeCenterOffset:set(
			_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetXSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetYSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Tabs.Props.Spinners.LightedVolumeCenterOffsetZSpinner.Ctrl:getCurrentValue())
		light_scene_node:setLightedVolumeCenterOffset(_LightedVolumeCenterOffset)
	elseif name =="Scened.Scene.Light.Props.LightedVolumeSizeChangeXSpinner"
			or name =="Scened.Scene.Light.Props.LightedVolumeSizeChangeYSpinner"
			or name =="Scened.Scene.Light.Props.LightedVolumeSizeChangeZSpinner" then
		_LightedVolumeSizeChange:set(
			_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeXSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeYSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Tabs.Props.Spinners.LightedVolumeSizeChangeZSpinner.Ctrl:getCurrentValue())
		light_scene_node:setLightedVolumeSizeChange(_LightedVolumeSizeChange)
	end
end

function _ScenedLightListSelectionAccepted(args)
	local light_scene_node = _ScenedLightGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local combo = tolua.cast(we.window, "CEGUI::Combobox")
	local list = combo:getDropList()
	local index = list:getItemIndex(list:getFirstSelectedItem())
	if name =="Scened.Scene.Light.Props.TypeCombo" then
		_Light.Type = index
		light_scene_node:setLightParameters(_Light)
	end
end

function _ScenedInitLightControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_light.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Light.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Light.TabControl"), "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for key, value in _Ctrls.Tabs do
		value.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Light."..key..".Root")
		value.Root.Ctrl:setText(value.Caption)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
		value.ScrollArea.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Light."..key..".ScrollArea")
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Props.Root.Ctrl)

	for key, value in _Ctrls.Tabs.Props.Checks do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Light.Props."..key), "CEGUI::Checkbox")
		value.Ctrl:subscribeEvent("CheckStateChanged", _ScenedLightCheckStateChanged)
	end
	
	for key, value in pairs(_Ctrls.Tabs.Props.ColorImages) do
		value.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Light.Props."..key)
		local cegui_tex = nil
		local imageset = nil
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		_Resources.Textures.ColorImages[key] = cegui_tex
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		imageset = CEGUIImgsetMgr:create("Scened.Scene.Light.Props."..key, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.Ctrl:setProperty("Image", "set:Scened.Scene.Light.Props."..key.." image:ColorImage")
	end
	
	Helper.GUI.initButtons("Scened.Scene.Light.Props.",
		_Ctrls.Tabs.Props.Buttons, _ScenedLightWidgetClicked)
	
	for key, value in _Ctrls.Tabs.Props.Spinners do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Light.Props."..key), "CEGUI::Spinner")
		value.Ctrl:subscribeEvent("ValueChanged", _ScenedLightValueChanged)
	end
	
	_Ctrls.Tabs.Props.IntensityWaveFuncLabel.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Light.Props.IntensityWaveFuncLabel")
	
	_Ctrls.Tabs.Props.TypeCombo.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Light.Props.TypeCombo"), "CEGUI::Combobox")

	for i = 0, vid.E_LIGHT_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Tabs.Props.TypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getLightTypeReadableName(i), i, nil)
	end

	_Ctrls.Tabs.Props.TypeCombo.Ctrl:subscribeEvent(
			"ListSelectionAccepted", _ScenedLightListSelectionAccepted)
			
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedLightUpdate,
		updateControls = _ScenedUpdateLightControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end
