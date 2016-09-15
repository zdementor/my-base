
local _Ctrls =
{
	ParentTabControl = {},
	Root = {},
	Buttons =
	{
		PassAddBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"					},
		PassRemBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"				},
		OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"				},
		SaveBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save-as"			},
		ChooseBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-choose-from-list"	},
		AddBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-add-to-list"		},
	},
	PassesListbox = {},
	TabControl = {},
	Props =
	{
		Root = {},
		LightingCheck = {},
		ColorImages =
		{
			DiffuseImage = {},
			AmbientImage = {},
			SpecularImage = {},
			EmissiveImage = {},
		},
		Buttons =
		{
			DiffuseBtn = {},
			AmbientBtn = {},
			SpecularBtn = {},
			EmissiveBtn = {},
			ColorGenBtn = {},
			AlphaGenBtn = {},
		},
		ShininessSpinner = {},
		Flags =
		{
			Flag0Checkbox = {},
			Flag1Checkbox = {},
			Flag2Checkbox = {},
			Flag3Checkbox = {},
			Flag4Checkbox = {},
			Flag5Checkbox = {},
			Flag6Checkbox = {},
		},
		Combos =
		{		
			EqBlendCombo = {},
			SourceBlendCombo = {},
			DestBlendCombo = {},
			ColorGenCombo = {},
			AlphaGenCombo = {},
			DepthTestCombo = {},
			AlphaTestCombo = {},
		},		
	},
	Layers =
	{
		Root = {},
		TabControl = {},
		Listbox = {},
		Buttons =
		{
			AddBtn 		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"		},
			RemBtn 		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"	},
			MoveUpBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-up"			},
			MoveDownBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-down"		},
		},
		Tex =
		{
			Root = {},
			TypeCombo = {},
			Image = {},
			NameEditbox = {},
			Buttons =
			{
				OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
				ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
			},
			RGBScaleSpinner = {},
			FilterCheck = {},
		},
		TCoord =
		{
			Root = {},
			ChannelSpinner = {},
			AddrCombo = {},
			GenCombo = {},
			Gen =
			{
				Root = {},
				Editboxes =
				{
					ProjWidth = {},
					ProjHeight = {},
					ProjPosX = {},
					ProjPosY = {},
					ProjPosZ = {},
					ProjDirX = {},
					ProjDirY = {},
					ProjDirZ = {},
					ProjUpX = {},
					ProjUpY = {},
					ProjUpZ = {},
				},
			},
		},
		TCMod =
		{
			Root = {},
			Listbox = {},
			Buttons =
			{
				AddBtn 		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"	},
				RemBtn 		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"},
				MoveUpBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-up"		},
				MoveDownBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-down"	},
			},			
			ClientAreaRoot = {},
			TypeCombo = {},
			ClientArea = {},
			Clients =
			{
				Transform =
				{
					Root = {},
					Editboxes =
					{
						Matrix00Edit = {},
						Matrix01Edit = {},
						Matrix10Edit = {},
						Matrix11Edit = {},
						Translate0Edit = {},
						Translate1Edit = {},
					},
				},
				Turbulent =
				{
					Root = {},
					Label = {},
					Btn = {},
				},
				Scale =
				{
					Root = {},
					Editboxes =
					{
						Scale0Edit = {},
						Scale1Edit = {},
					}
				},
				Scroll =
				{
					Root = {},
					Editboxes =
					{
						Scroll0Edit = {},
						Scroll1Edit = {},
					}
				},
				Rotate =
				{
					Root = {},
					Editboxes =
					{
						RotateSpeedEdit = {},
					}
				},
			},
		},
	},
	Shader =
	{
		Root = {},
		Buttons =
		{
			OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
			EditBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-text"		},
			ReloadBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh"	},
			ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
		},
		FileNameEditbox = {},
	},
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
			EmissiveImage = nil,
			TextureImage = nil,
		}
	},
	Imagesets =
	{
		TextureImage = nil,
	},
}

function _ScenedInitMaterialControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_material.layout", "", "CommonLayouts")	
	
	_Ctrls.ParentTabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.CommonTabControl"), "CEGUI::TabControl")
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Root")
	_Ctrls.Root.Ctrl:setVisible(false)

	Helper.GUI.initButtons("Scened.Material.",
		_Ctrls.Buttons, _ScenedMaterialWidgetClicked)

	_Ctrls.PassesListbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.PassesListbox"), "CEGUI::Listbox")
	_Ctrls.PassesListbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedMaterialItemSelectionChanged)
		
	_Ctrls.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.TabControl"), "CEGUI::TabControl")

	_Ctrls.Props.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Props.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Props.Tab"):addChildWindow(
		_Ctrls.Props.Root.Ctrl)
	_Ctrls.Props.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
		
	_Ctrls.Layers.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.Tab"):addChildWindow(
		_Ctrls.Layers.Root.Ctrl)
	_Ctrls.Layers.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	_Ctrls.Shader.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Shader.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Shader.Tab"):addChildWindow(
		_Ctrls.Shader.Root.Ctrl)
	_Ctrls.Shader.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	if not MyDevice:getDeviceFlagValue(dev.EDCF_USE_SHADERS) then
		_Ctrls.TabControl.Ctrl:removeTab("Scened.Material.Pass.Shader.Tab")
	end

	_Ctrls.Layers.TabControl.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TabControl"), "CEGUI::TabControl")

	_Ctrls.Layers.TCoord.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCoord.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCoord.Tab"):addChildWindow(
		_Ctrls.Layers.TCoord.Ctrl)
	_Ctrls.Layers.TCoord.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	for key, value in pairs(_Ctrls.Props.Flags) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Props."..key), "CEGUI::Checkbox")
		value.Ctrl:subscribeEvent(
			"CheckStateChanged", _ScenedMaterialCheckStateChanged)
	end
	
	for i = 0, vid.E_MATERIAL_FLAG_COUNT - 1 do
		local checkbox = _Ctrls.Props.Flags[string.format("Flag%dCheckbox", i)].Ctrl
		checkbox:setText(vid.getMaterialFlagReadableName(i))
		checkbox:setTooltipText(checkbox:getText())
	end
	
	_Ctrls.Props.LightingCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Props.LightingCheck"), "CEGUI::Checkbox")
	_Ctrls.Props.LightingCheck.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedMaterialCheckStateChanged)

	for key, value in pairs(_Ctrls.Props.ColorImages) do
		value.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Props."..key)
		local cegui_tex = nil
		local imageset = nil
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		_Resources.Textures.ColorImages[key] = cegui_tex
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		imageset = CEGUIImgsetMgr:create("Scened.Material.Pass.Props."..key, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.Ctrl:setProperty("Image", "set:Scened.Material.Pass.Props."..key.." image:ColorImage")
	end
	
	Helper.GUI.initButtons("Scened.Material.Pass.Props.",
		_Ctrls.Props.Buttons, _ScenedMaterialWidgetClicked)

	_Ctrls.Props.ShininessSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Props.ShininessSpinner"), "CEGUI::Spinner")
	_Ctrls.Props.ShininessSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedMaterialValueChanged)

	for key, value in pairs(_Ctrls.Props.Combos) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Props."..key), "CEGUI::Combobox")
		value.Ctrl:subscribeEvent(
			"ListSelectionAccepted", _ScenedMaterialListSelectionAccepted)
	end
	
	Helper.GUI.initButtons("Scened.Material.Pass.Layers.",
		_Ctrls.Layers.Buttons, _ScenedMaterialWidgetClicked)

	_Ctrls.Layers.Listbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.Listbox"), "CEGUI::Listbox")
	_Ctrls.Layers.Listbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedMaterialItemSelectionChanged)

	for i = 0, vid.E_BLEND_EQUATION_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.EqBlendCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getBlendEquationReadableName(i), i, nil)
	end
	for i = 0, vid.E_SRC_BLEND_FUNCTION_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.SourceBlendCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getSrcBlendFunctionReadableName(i), i, nil)
	end
	for i = 0, vid.E_DST_BLEND_FUNCTION_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.DestBlendCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getDstBlendFunctionReadableName(i), i, nil)
	end
	
	for i = 0, vid.E_COLOR_GEN_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.ColorGenCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getColorGenTypeReadableName(i), i, nil)
	end
	for i = 0, vid.E_ALPHA_GEN_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.AlphaGenCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getAlphaGenTypeReadableName(i), i, nil)
	end

	for i = 0, vid.E_COMPARE_TEST_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.DepthTestCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getCompareTestReadableName(i), i, nil)
	end	
	for i = 0, vid.E_ALPHA_TEST_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Props.Combos.AlphaTestCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getAlphaTestReadableName(i), i, nil)
	end
	
	-- Tex
	
	_Ctrls.Layers.Tex.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.Tex.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.Tex.Tab"):addChildWindow(
		_Ctrls.Layers.Tex.Root.Ctrl)
	_Ctrls.Layers.Tex.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	Helper.GUI.initButtons("Scened.Material.Pass.Layers.Tex.",
		_Ctrls.Layers.Tex.Buttons, _ScenedMaterialWidgetClicked)
	
	_Ctrls.Layers.Tex.Image.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.Tex.Image")
	
	local cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.TextureImage = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.Material.Pass.Layers.TextureImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.TextureImage = imageset
	_Ctrls.Layers.Tex.Image.Ctrl:setProperty("Image", "set:"..imageset:getName().." image:ColorImage")
	
	_Ctrls.Layers.Tex.NameEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.Tex.NameEditbox"), "CEGUI::Editbox")

	_Ctrls.Layers.Tex.TypeCombo.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.Tex.TypeCombo"), "CEGUI::Combobox")
	_Ctrls.Layers.Tex.TypeCombo.Ctrl:subscribeEvent(
		"ListSelectionAccepted", _ScenedMaterialListSelectionAccepted)
	for i = 0, vid.E_TEXTURE_LAYER_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Layers.Tex.TypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getTextureLayerTypeReadableName(i), i, nil)
	end
	
	_Ctrls.Layers.Tex.RGBScaleSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.Tex.RGBScaleSpinner"), "CEGUI::Spinner")
	_Ctrls.Layers.Tex.RGBScaleSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedMaterialValueChanged)
		
	_Ctrls.Layers.Tex.FilterCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.Tex.FilterCheck"), "CEGUI::Checkbox")
	_Ctrls.Layers.Tex.FilterCheck.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedMaterialCheckStateChanged)

	-- TCoord
	
	_Ctrls.Layers.TCoord.ChannelSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCoord.ChannelSpinner"), "CEGUI::Spinner")
	_Ctrls.Layers.TCoord.ChannelSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedMaterialValueChanged)
	
	_Ctrls.Layers.TCoord.AddrCombo.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCoord.AddrCombo"), "CEGUI::Combobox")
	_Ctrls.Layers.TCoord.AddrCombo.Ctrl:subscribeEvent(
		"ListSelectionAccepted", _ScenedMaterialListSelectionAccepted)
	for i = 0, vid.E_TEXCOORD_ADDRESS_MODE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Layers.TCoord.AddrCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getTexCoordAddrModeReadableName(i), i, nil)
	end
	
	_Ctrls.Layers.TCoord.GenCombo.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCoord.GenCombo"), "CEGUI::Combobox")
	_Ctrls.Layers.TCoord.GenCombo.Ctrl:subscribeEvent(
		"ListSelectionAccepted", _ScenedMaterialListSelectionAccepted)
	for i = 0, vid.E_TEXCOORD_GEN_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Layers.TCoord.GenCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getTexCoordGenTypeReadableName(i), i, nil)
	end
	
	for key, value in pairs(_Ctrls.Layers.TCoord.Gen.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCoord.Gen."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedMaterialTextAccepted)
	end
	
	_Ctrls.Layers.TCoord.Gen.Root.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCoord.Gen.Root")
	
	-- TCMod
	
	_Ctrls.Layers.TCMod.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCMod.Root")
	CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCMod.Tab"):addChildWindow(
		_Ctrls.Layers.TCMod.Root.Ctrl)
	_Ctrls.Layers.TCMod.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")

	_Ctrls.Layers.TCMod.ClientAreaRoot.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCMod.ClientAreaRoot")
	
	Helper.GUI.initButtons("Scened.Material.Pass.Layers.TCMod.",
		_Ctrls.Layers.TCMod.Buttons, _ScenedMaterialWidgetClicked)
	
	_Ctrls.Layers.TCMod.Listbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCMod.Listbox"), "CEGUI::Listbox")
	_Ctrls.Layers.TCMod.Listbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedMaterialItemSelectionChanged)

	_Ctrls.Layers.TCMod.TypeCombo.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCMod.TypeCombo"), "CEGUI::Combobox")
	_Ctrls.Layers.TCMod.TypeCombo.Ctrl:subscribeEvent(
		"ListSelectionAccepted", _ScenedMaterialListSelectionAccepted)
	for i = 0, vid.E_TEXCOORD_MOD_TYPE_COUNT - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Layers.TCMod.TypeCombo.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			vid.getTexCoordModTypeReadableName(i), i, nil)
	end
		
	_Ctrls.Layers.TCMod.ClientArea.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCMod.ClientArea")
	for key, value in pairs(_Ctrls.Layers.TCMod.Clients) do
		value.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Material.Pass.Layers.TCMod."..key..".Root")
		_Ctrls.Layers.TCMod.ClientArea.Ctrl:addChildWindow(value.Root.Ctrl)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
		value.Root.Ctrl:setVisible(false)
	end
	
	for key, value in pairs(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCMod.Transform."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedMaterialTextAccepted)
	end

	_Ctrls.Layers.TCMod.Clients.Turbulent.Label.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCMod.Turbulent.Label")
	
	_Ctrls.Layers.TCMod.Clients.Turbulent.Btn.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Layers.TCMod.Turbulent.Btn"), "CEGUI::ButtonBase")
	_Ctrls.Layers.TCMod.Clients.Turbulent.Btn.Ctrl:subscribeEvent(
		"Clicked", _ScenedMaterialWidgetClicked)
	
	for key, value in pairs(_Ctrls.Layers.TCMod.Clients.Scale.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCMod.Scale."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedMaterialTextAccepted)
	end
		for key, value in pairs(_Ctrls.Layers.TCMod.Clients.Scroll.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCMod.Scroll."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedMaterialTextAccepted)
	end
		for key, value in pairs(_Ctrls.Layers.TCMod.Clients.Rotate.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Material.Pass.Layers.TCMod.Rotate."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedMaterialTextAccepted)
	end
	
	-- Shader

	Helper.GUI.initButtons("Scened.Material.Pass.Shader.",
		_Ctrls.Shader.Buttons, _ScenedMaterialWidgetClicked)

	_Ctrls.Shader.FileNameEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Material.Pass.Shader.FileNameEditbox"), "CEGUI::MultiLineEditbox")
		
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedMaterialUpdate,
		updateControls = _ScenedUpdateMaterialControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end

function _ScenedMaterialGetSelectedPass()
	local material = Helper.getSelectedMaterial()
	local item = _Ctrls.PassesListbox.Ctrl:getFirstSelectedItem()
	if material == nil or item == nil then
		return nil
	end
	local pass_idx = _Ctrls.PassesListbox.Ctrl:getItemIndex(item)	
	local pass = material:getPass(pass_idx)
	return pass
end

 function _ScenedMaterialGetSelectedLayer()
	local pass = _ScenedMaterialGetSelectedPass()
	if pass == nil then
		return nil
	end
	local item = _Ctrls.Layers.Listbox.Ctrl:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	local layer_idx = item:getID()
	local units_max = MyDriver:getMaximalTextureUnitsAmount()
	if layer_idx < 0 or layer_idx >= units_max then
		return nil
	end
	return pass.Layers[layer_idx]
end

function _ScenedMaterialGetSelectedLayerTCMod()
	local layer = _ScenedMaterialGetSelectedLayer()
	if layer == nil then
		return nil
	end
	local item = _Ctrls.Layers.TCMod.Listbox.Ctrl:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	local tcmod_idx = item:getID()
	if tcmod_idx < 0 or tcmod_idx >= layer:getTexCoordModifiersCount() then
		return nil
	end
	return layer:getTexCoordModifier(tcmod_idx)
end

function _ScenedMaterialGetSelectedLayerTCModIndex()
	local item = _Ctrls.Layers.TCMod.Listbox.Ctrl:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	return item:getID()
end

function _ScenedMaterialUpdateLayerTCModControls()

	local layer = _ScenedMaterialGetSelectedLayer()
	local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
	
	_Ctrls.Layers.TCMod.Buttons.RemBtn.Ctrl:setEnabled(false)
	_Ctrls.Layers.TCMod.Buttons.MoveUpBtn.Ctrl:setEnabled(false)
	_Ctrls.Layers.TCMod.Buttons.MoveDownBtn.Ctrl:setEnabled(false)
	
	if layer == nil or tcmod == nil then
		_Ctrls.Layers.TCMod.ClientAreaRoot.Ctrl:setVisible(false)
		return
	else
		_Ctrls.Layers.TCMod.ClientAreaRoot.Ctrl:setVisible(true)
	end
	_Ctrls.Layers.TCMod.TypeCombo.Ctrl:setItemSelectState(tcmod:getType(), true)
	
	for key, value in pairs(_Ctrls.Layers.TCMod.Clients) do
		value.Root.Ctrl:setVisible(false)
	end
	
	local tcmodindex = _ScenedMaterialGetSelectedLayerTCModIndex()
	local tcmodcnt = layer:getTexCoordModifiersCount()
	
	_Ctrls.Layers.TCMod.Buttons.RemBtn.Ctrl:setEnabled(true)
	_Ctrls.Layers.TCMod.Buttons.MoveUpBtn.Ctrl:setEnabled(tcmodindex > 0)
	_Ctrls.Layers.TCMod.Buttons.MoveDownBtn.Ctrl:setEnabled(tcmodindex < (tcmodcnt - 1))

	if tcmod:getType() == vid.ETCMT_TRANSFORM then
		_Ctrls.Layers.TCMod.Clients.Transform.Root.Ctrl:setVisible(true)
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix00Edit.Ctrl:setText(string.format("%.4f", tcmod:getMatrix00()))
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix01Edit.Ctrl:setText(string.format("%.4f", tcmod:getMatrix01()))
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix10Edit.Ctrl:setText(string.format("%.4f", tcmod:getMatrix10()))
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix11Edit.Ctrl:setText(string.format("%.4f", tcmod:getMatrix11()))
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Translate0Edit.Ctrl:setText(string.format("%.4f", tcmod:getTranslate0()))
		_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Translate1Edit.Ctrl:setText(string.format("%.4f", tcmod:getTranslate1()))
	elseif tcmod:getType() == vid.ETCMT_TURBULENT then
		_Ctrls.Layers.TCMod.Clients.Turbulent.Root.Ctrl:setVisible(true)
		_Ctrls.Layers.TCMod.Clients.Turbulent.Label.Ctrl:setText(string.format(
			"%s {\nbase %.3f,\nampl. %.3f,\nphase %.3f,\nfreq. %.3f}",
			core.getFuncTypeReadableName(tcmod:getTurbulent().func),
			tcmod:getTurbulent().base, tcmod:getTurbulent().amplitude, tcmod:getTurbulent().phase, tcmod:getTurbulent().frequency))
	elseif tcmod:getType() == vid.ETCMT_SCROLL then
		_Ctrls.Layers.TCMod.Clients.Scroll.Root.Ctrl:setVisible(true)
		_Ctrls.Layers.TCMod.Clients.Scroll.Editboxes.Scroll0Edit.Ctrl:setText(string.format("%.4f", tcmod:getScroll0()))
		_Ctrls.Layers.TCMod.Clients.Scroll.Editboxes.Scroll1Edit.Ctrl:setText(string.format("%.4f", tcmod:getScroll1()))
	elseif tcmod:getType() == vid.ETCMT_SCALE then
		_Ctrls.Layers.TCMod.Clients.Scale.Root.Ctrl:setVisible(true)
		_Ctrls.Layers.TCMod.Clients.Scale.Editboxes.Scale0Edit.Ctrl:setText(string.format("%.4f", tcmod:getScale0()))
		_Ctrls.Layers.TCMod.Clients.Scale.Editboxes.Scale1Edit.Ctrl:setText(string.format("%.4f", tcmod:getScale1()))
	elseif tcmod:getType() == vid.ETCMT_ROTATE then
		_Ctrls.Layers.TCMod.Clients.Rotate.Root.Ctrl:setVisible(true)
		_Ctrls.Layers.TCMod.Clients.Rotate.Editboxes.RotateSpeedEdit.Ctrl:setText(string.format("%.4f", tcmod:getRotateSpeed()))
	end

end

function _ScenedMaterialUpdateLayerControls()

	local layer = _ScenedMaterialGetSelectedLayer()
	local pass = _ScenedMaterialGetSelectedPass()
	
	_Ctrls.Layers.Buttons.RemBtn.Ctrl:setEnabled(false)
	_Ctrls.Layers.Buttons.MoveUpBtn.Ctrl:setEnabled(false)
	_Ctrls.Layers.Buttons.MoveDownBtn.Ctrl:setEnabled(false)	
	
	if pass == nil or layer == nil then
		_Ctrls.Layers.TabControl.Ctrl:setVisible(false)
		return
	else
		_Ctrls.Layers.TabControl.Ctrl:setVisible(true)
	end
	
	_Ctrls.Layers.Buttons.RemBtn.Ctrl:setEnabled(true)
	
	local layer_index = Helper.getPassLayerIndex(pass, layer)
	local units_max = MyDriver:getMaximalTextureUnitsAmount()
	
	_Ctrls.Layers.Buttons.MoveUpBtn.Ctrl:setEnabled(layer_index > 0)
	_Ctrls.Layers.Buttons.MoveDownBtn.Ctrl:setEnabled(layer_index < (units_max - 1))	
	
	local imageset = _Resources.Imagesets.TextureImage
	local cegui_tex = _Resources.Textures.TextureImage
	local my_tex = layer:getTexture()

	if my_tex ~= nil then
		local fullfname = MyDriver:findTextureName(my_tex)
		MyCEGUI.setTexture(cegui_tex, my_tex, false)
		imageset:undefineAllImages()
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		local relfname = Helper.getRelFileName(fullfname, MyResMgr:getMediaDirFull(res.EMT_TEXTURES))
		_Ctrls.Layers.Tex.NameEditbox.Ctrl:setText(relfname)
		_Ctrls.Layers.Tex.Image.Ctrl:setProperty(
			"Image", "set:"..imageset:getName().." image:ColorImage")
	else
		_Ctrls.Layers.Tex.NameEditbox.Ctrl:setText("")
		_Ctrls.Layers.Tex.Image.Ctrl:setProperty("Image", "")
	end
	
	_Ctrls.Layers.Tex.TypeCombo.Ctrl:setItemSelectState(layer:getType(), true)
	_Ctrls.Layers.Tex.RGBScaleSpinner.Ctrl:setCurrentValue(layer:getTexRGBScale())
	_Ctrls.Layers.Tex.FilterCheck.Ctrl:setSelected(layer:isFilterEnabled())
	
	_Ctrls.Layers.TCoord.ChannelSpinner.Ctrl:setCurrentValue(layer:getTexCoordChannel())
	_Ctrls.Layers.TCoord.AddrCombo.Ctrl:setItemSelectState(layer:getTexCoordAddr(), true)
	_Ctrls.Layers.TCoord.GenCombo.Ctrl:setItemSelectState(layer:getTexCoordGen(), true)
	
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjWidth.Ctrl:setText(string.format("%.4f", layer:getProjectionWidth()))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjHeight.Ctrl:setText(string.format("%.4f", layer:getProjectionHeight()))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosX.Ctrl:setText(string.format("%.4f", layer:getProjectionPos().X))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosY.Ctrl:setText(string.format("%.4f", layer:getProjectionPos().Y))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosZ.Ctrl:setText(string.format("%.4f", layer:getProjectionPos().Z))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirX.Ctrl:setText(string.format("%.4f", layer:getProjectionDir().X))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirY.Ctrl:setText(string.format("%.4f", layer:getProjectionDir().Y))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirZ.Ctrl:setText(string.format("%.4f", layer:getProjectionDir().Z))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpX.Ctrl:setText(string.format("%.4f", layer:getProjectionUp().X))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpY.Ctrl:setText(string.format("%.4f", layer:getProjectionUp().Y))
	_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpZ.Ctrl:setText(string.format("%.4f", layer:getProjectionUp().Z))	
	
	_Ctrls.Layers.TCoord.Gen.Root.Ctrl:setVisible(layer:getTexCoordGen() == vid.ETCGT_PROJECTED_MAPPING)
	
	_Ctrls.Layers.TCMod.Listbox.Ctrl:resetList()
	local tcmodcnt = layer:getTexCoordModifiersCount()
	for i = 0, tcmodcnt - 1 do
		Helper.GUI.addListboxItem(_Ctrls.Layers.TCMod.Listbox.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			"Mod "..i, i, nil)
	end
	
	if _Ctrls.Layers.TCMod.Listbox.Ctrl:getItemCount() > 0 then
		_Ctrls.Layers.TCMod.Listbox.Ctrl:setItemSelectState(0, true)
	else
		_ScenedMaterialUpdateLayerTCModControls()
	end
	
end

function _ScenedMaterialUpdatePassControls()

	local pass = _ScenedMaterialGetSelectedPass()
	
	_Ctrls.Buttons.PassRemBtn.Ctrl:setEnabled(false)
	
	if pass == nil then
		_Ctrls.TabControl.Ctrl:setVisible(false)
		return
	else
		_Ctrls.TabControl.Ctrl:setVisible(true)
	end
	_Ctrls.Props.LightingCheck.Ctrl:setSelected(pass:isLighting())
	
	_Ctrls.Buttons.PassRemBtn.Ctrl:setEnabled(true)

	local color_tbl =
	{
		["DiffuseImage"] = pass:getDiffuseColor(),
		["AmbientImage"] = pass:getAmbientColor(),
		["SpecularImage"] = pass:getSpecularColor(),
		["EmissiveImage"] = pass:getEmissiveColor(),
	}
	for key, value in pairs(_Resources.Textures.ColorImages) do
		if color_tbl[key] ~= nil then
			Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.ColorImages[key], color_tbl[key])
		else
			LOG_INFO(string.format("Incorrect ColorImage in Material Editor (key='%s')", key))
		end
	end
	_Ctrls.Props.ShininessSpinner.Ctrl:setCurrentValue(pass:getShininess())

	_Ctrls.Props.Combos.EqBlendCombo.Ctrl:setItemSelectState(pass:getBlendEquation(), true)
	_Ctrls.Props.Combos.SourceBlendCombo.Ctrl:setItemSelectState(pass:getSrcBlendFunc(), true)
	_Ctrls.Props.Combos.DestBlendCombo.Ctrl:setItemSelectState(pass:getDstBlendFunc(), true)
	_Ctrls.Props.Combos.ColorGenCombo.Ctrl:setItemSelectState(pass:getColorGen(), true)
	_Ctrls.Props.Combos.AlphaGenCombo.Ctrl:setItemSelectState(pass:getAlphaGen(), true)
	_Ctrls.Props.Combos.DepthTestCombo.Ctrl:setItemSelectState(pass:getDepthTest(), true)
	_Ctrls.Props.Combos.AlphaTestCombo.Ctrl:setItemSelectState(pass:getAlphaTest(), true)
	
	_Ctrls.Props.Buttons.ColorGenBtn.Ctrl:setVisible(pass:getColorGen() == vid.ECGT_FUNCTION)
	_Ctrls.Props.Buttons.AlphaGenBtn.Ctrl:setVisible(pass:getAlphaGen() == vid.ECGT_FUNCTION)
	
	for i = 0, vid.E_MATERIAL_FLAG_COUNT - 1 do
		local checkbox = _Ctrls.Props.Flags[string.format("Flag%dCheckbox", i)].Ctrl
		checkbox:setSelected(pass:getFlag(i))
	end

	_Ctrls.Layers.Listbox.Ctrl:resetList()
	local units_max = MyDriver:getMaximalTextureUnitsAmount()
	local cnt = 0
	for i = 0, units_max - 1 do
		local layer = pass.Layers[i]
		local texture = layer:getTexture()
		if texture ~= nil then
			Helper.GUI.addListboxItem(_Ctrls.Layers.Listbox.Ctrl, OPTIONS.ScenedGUI.SchemeName,
				"Layer "..i, cnt, nil)
		end
		cnt = cnt + 1
	end
	
	local gpu_prog = pass:getGPUProgram()
	if gpu_prog ~= nil then
		local full_file_name = MyDriver:findGPUProgramFullFileName(gpu_prog)
		local rel_fname = Helper.getRelFileName(
			full_file_name, MyResMgr:getMediaDirFull(res.EMT_MATERIALS))
		_Ctrls.Shader.FileNameEditbox.Ctrl:setText(rel_fname)
		_Ctrls.Shader.Buttons.ReloadBtn.Ctrl:setEnabled(true)
		_Ctrls.Shader.Buttons.EditBtn.Ctrl:setEnabled(true)
		_Ctrls.Shader.Buttons.ClearBtn.Ctrl:setEnabled(true)
	else
		_Ctrls.Shader.FileNameEditbox.Ctrl:setText("Auto generated...")
		_Ctrls.Shader.Buttons.ReloadBtn.Ctrl:setEnabled(false)
		_Ctrls.Shader.Buttons.EditBtn.Ctrl:setEnabled(false)
		_Ctrls.Shader.Buttons.ClearBtn.Ctrl:setEnabled(false)
	end
	_Ctrls.Shader.Buttons.EditBtn.Ctrl:setEnabled(true)
	_Ctrls.Shader.FileNameEditbox.Ctrl:setEnabled(false)

	if _Ctrls.Layers.Listbox.Ctrl:getItemCount() > 0 then
		_Ctrls.Layers.Listbox.Ctrl:setItemSelectState(0, true)
		_Ctrls.Layers.TabControl.Ctrl:setSelectedTabAtIndex(0)
	else
		_ScenedMaterialUpdateLayerControls()
	end

end

function _ScenedMaterialUpdate()
	if not _Ctrls.Root.Ctrl:isVisible() then
		return
	end
end

function _ScenedUpdateMaterialControls()
	local vis = true
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local scene_node = nil
	if sel_nodes == 1 then
		scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getMaterialsCount() ~= 1 then
			vis = false
		elseif scene_node:getSceneNodeType() == scn.ESNT_ANIMATED_MESH_SCENE_NODE then
			local amesh_scene_node = tolua.cast(scene_node, "scn::IAnimatedMeshSceneNode")
			local amesh = amesh_scene_node:getAnimatedMesh()
			if amesh ~= nil then
				if amesh:getAnimatedMeshType() == scn.EMT_CAL3D then
					vis = false
				end
			end
		end
	else
		vis = false
	end
	if not vis then
		_Ctrls.Root.Ctrl:setVisible(false)
		_Ctrls.ParentTabControl.Ctrl:removeTab(_Ctrls.Root.Ctrl:getName())
		return
	end
	if _Ctrls.ParentTabControl.Ctrl:getTabCount() < 3 then
		_Ctrls.ParentTabControl.Ctrl:addTab(_Ctrls.Root.Ctrl)
		_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
		_Ctrls.TabControl.Ctrl:setSelectedTabAtIndex(0)
	end
	_Ctrls.PassesListbox.Ctrl:resetList()
	local material = scene_node:getMaterial(0)
	local pass_cnt = material:getPassesCount()
	for i = 0, pass_cnt - 1 do
		local pass = material:getPass(i)
		Helper.GUI.addListboxItem(_Ctrls.PassesListbox.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			"Pass "..i, i, nil)
	end
	if _Ctrls.PassesListbox.Ctrl:getItemCount() > 0 then
		_Ctrls.PassesListbox.Ctrl:setItemSelectState(0, true)
	else
		_ScenedMaterialUpdatePassControls()
	end
	_Ctrls.TabControl.Ctrl:setSelectedTabAtIndex(0)
end

function _ScenedMaterialSetDiffuseColor(color)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setDiffuseColor(color)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialSetAmbientColor(color)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setAmbientColor(color)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialSetSpecularColor(color)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setSpecularColor(color)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialSetEmissiveColor(color)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setEmissiveColor(color)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialSetColorFunc(wave)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setColorFunc(wave)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialSetAlphaFunc(wave)
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setAlphaFunc(wave)
	_ScenedMaterialUpdatePassControls()
end

function _ScenedMaterialTextureOpenAccepted(full_file_name)
	local layer = _ScenedMaterialGetSelectedLayer()
	local texture = MyDriver:getTexture(full_file_name)
	if texture ~= nil then
		layer:setTexture(texture)
		_ScenedMaterialUpdateLayerControls()
	end
end

function _ScenedMaterialGPUProgOpenAccepted(full_file_name)
	local pass = _ScenedMaterialGetSelectedPass()
	local gpu_prog = MyDriver:getGPUProgram(full_file_name, ShaderGen.getCurrentTag(), true)
	if gpu_prog ~= nil then
		pass:setGPUProgram(gpu_prog)
		_ScenedMaterialUpdatePassControls()
	end
end

function _ScenedMaterialSaveAccepted(full_file_name)
	local material = Helper.getSelectedMaterial()
	local fpath = core.extractFilePath(full_file_name)
	if fpath == MyResMgr:getMediaDirFull(res.EMT_MATERIALS) then
		local fname = core.extractFileName(full_file_name)
		local mat_idx = MyMatMgr:findMaterialByName(fname)
		if -1 ~= mat_idx then
			MyMatMgr:setMaterial(mat_idx, material)
			
		else
			MyMatMgr:addMaterial(material, fname)
		end
	end
	material:saveToFile(full_file_name)
end

function _ScenedMaterialSelectionAccepted(sel_material)
	local material = Helper.getSelectedMaterial()
	material:set(sel_material)
	_ScenedUpdateMaterialControls()
end

function _ScenedMaterialNameValidate(sel_name)
	local text = string.gsub(sel_name, "(%c+)", "")
	text = string.gsub(text, "(%s+)", "_")
	text = string.gsub(text, "(%p+)", "_")
	text = string.lower(text)
	return text
end

function _ScenedMaterialNameAccept(sel_name)
	local material = Helper.getSelectedMaterial()
	local name = core.extractFileName(sel_name)..".material"
	if -1 ~= MyMatMgr:findMaterialByName(name) then
		Helper.GUI.MessageBox.show(0, 0, "Warning", string.format(
			"Material with name\n'%s'\nalready registered in library", name))
		return
	end
	material:saveToFile(MyResMgr:getMediaDirFull(res.EMT_MATERIALS)..name)
	MyMatMgr:addMaterial(material, name)
end

function _ScenedMaterialOpenAccepted(full_file_name)
	local material = Helper.getSelectedMaterial()
	if material:loadFromFile(full_file_name) then
		_ScenedUpdateMaterialControls()
	end
end

function _ScenedMaterialLayerAddTCMod(sel_type)
	local layer = _ScenedMaterialGetSelectedLayer()
	layer:addTexCoordModifier(sel_type)
	_ScenedMaterialUpdateLayerControls()
end

function _ScenedMaterialSetTCModSetWave(wave)
	local tcmod =  _ScenedMaterialGetSelectedLayerTCMod()
	tcmod:setTurbulent(wave)
	_ScenedMaterialUpdateLayerTCModControls()
end

function _ScenedMaterialShaderOpenDialogShow(accept_callback)
	local progDir = MyResMgr:getMediaDirFull(res.EMT_GPU_PROGRAMS)
	if not MyFileSys:existDir(progDir) then
		progDir = MyResMgr:getMediaDirFull(res.EMT_MATERIALS)
	end
	Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Choose GPU program...",
		progDir, nil,
		{[1] = { Descr = "GPU programs", Value = "gpu" },
			["gpu"] = { Descr = "GPU programs", Value = "gpu" },},
		accept_callback)
end

function _ScenedMaterialShaderSaveDialogShow(accept_callback)
	local progDir = MyResMgr:getMediaDirFull(res.EMT_GPU_PROGRAMS)
	if not MyFileSys:existDir(progDir) then
		progDir = MyResMgr:getMediaDirFull(res.EMT_MATERIALS)
	end
	Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save GPU Program As...",
		progDir, Helper.GUI.Scened.ShaderEditor.getFileName(),
		{[1] = { Descr = "GPU programs", Value = "gpu" },
			["gpu"] = { Descr = "GPU programs", Value = "gpu" },},
		accept_callback)
end

local _ScenedMaterialShaderEditorOpenAcceptCallback = nil

function _ScenedMaterialShaderOpenDialogFromShaderEditorAcceptCallback(file_name)
	_ScenedMaterialGPUProgOpenAccepted(file_name)
	_ScenedMaterialShaderEditorOpenAcceptCallback(file_name)
	_ScenedMaterialShaderEditorOpenAcceptCallback = nil
end

function _ScenedMaterialShaderEditorOpenCallback(accept_callback)
	_ScenedMaterialShaderEditorOpenAcceptCallback = accept_callback
	_ScenedMaterialShaderOpenDialogShow(_ScenedMaterialShaderOpenDialogFromShaderEditorAcceptCallback)
end

local _ScenedMaterialShaderEditorSaveAcceptCallback = nil

function _ScenedMaterialShaderSaveDialogFromShaderEditorAcceptCallback(full_file_name)
	_ScenedMaterialShaderEditorSaveAcceptCallback(full_file_name)
	_ScenedMaterialGPUProgOpenAccepted(full_file_name)
	_ScenedMaterialShaderEditorSaveAcceptCallback = nil
end

function _ScenedMaterialShaderEditorSaveCallback(save_callback)
	_ScenedMaterialShaderEditorSaveAcceptCallback = save_callback
	_ScenedMaterialShaderSaveDialogShow(_ScenedMaterialShaderSaveDialogFromShaderEditorAcceptCallback)
end

local function _ScenedMaterialReloadGPUProgram()
	local pass = _ScenedMaterialGetSelectedPass()
	local gpu_prog = pass:getGPUProgram()
	if gpu_prog ~= nil then
		local full_file_name = MyDriver:findGPUProgramFullFileName(gpu_prog)
		gpu_prog = MyDriver:getGPUProgram(full_file_name, ShaderGen.getCurrentTag(), true)
		if gpu_prog ~= nil then
			pass:setGPUProgram(nil)
			pass:setGPUProgram(gpu_prog)
			return full_file_name
		end
	end
	return nil
end

local function _ScenedMaterialRegenerateGPUProgram()
	local sources = nil
	local scene_node = MyScnMgr:getSelectedSceneNode(0)
	local pass = _ScenedMaterialGetSelectedPass()
	if scene_node:getRenderBuffersCount() > 0 then
		local rend_buf = scene_node:getRenderBuffer(0)				
		local vertex_buf = rend_buf:getVertices()
		if vertex_buf ~= nil then
			local vertex_type = vertex_buf:getType()
			sources = ShaderGen.getSourcesFor(
				vertex_type, pass, ShaderGen.getMaxLightsCount())
		end
	end
	return sources
end

function _ScenedMaterialShaderEditorReloadCallback(reload_callback)
	local full_file_name = _ScenedMaterialReloadGPUProgram()
	if full_file_name == nil then
		local sources = _ScenedMaterialRegenerateGPUProgram()
		if sources ~= nil then
			Helper.GUI.Scened.ShaderEditor.setSources(sources)
		end
	else
		Helper.GUI.Scened.ShaderEditor.setFileName(full_file_name, true)
	end
	reload_callback()
end

function _ScenedMaterialShaderEditorAcceptCallback(accept_callback)
	accept_callback()
end

local _ScenedMaterialShaderEditorInitialGPUProg = nil

function _ScenedMaterialShaderEditorCancelCallback(cancel_callback)
	cancel_callback()
	local pass = _ScenedMaterialGetSelectedPass()
	pass:setGPUProgram(_ScenedMaterialShaderEditorInitialGPUProg)
	_ScenedMaterialUpdatePassControls()
	_ScenedMaterialShaderEditorInitialGPUProg = nil
end

local SCENED_MATERIAL_FILE_FILTER =
{
	[1]			= { Descr = "Material file",  Value = "material" },
	["material"]= { Descr = "Material file",  Value = "material" },
}

function _ScenedMaterialWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Material.PassAddBtn" then
		local material = Helper.getSelectedMaterial()
		material:addPass()
		_ScenedUpdateMaterialControls()
		_Ctrls.PassesListbox.Ctrl:setItemSelectState(material:getPassesCount() - 1, true)
	elseif name == "Scened.Material.PassRemBtn" then
		local material = Helper.getSelectedMaterial()
		local pass = _ScenedMaterialGetSelectedPass()
		local pass_index = Helper.getMaterialPassIndex(material, pass)		
		if not material:removePass(pass_index) then
			LOG_WARN(string.format("Can't remove pass, material must have at least one pass"))
		end
		_ScenedUpdateMaterialControls()
	elseif name == "Scened.Material.OpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open material from...",
			MyResMgr:getMediaDirFull(res.EMT_MATERIALS), nil,
			SCENED_MATERIAL_FILE_FILTER, _ScenedMaterialOpenAccepted)
	elseif name == "Scened.Material.SaveBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save material as...",
			MyResMgr:getMediaDirFull(res.EMT_MATERIALS), nil,
			SCENED_MATERIAL_FILE_FILTER, _ScenedMaterialSaveAccepted)
	elseif name == "Scened.Material.ChooseBtn" then
		local material = Helper.getSelectedMaterial()
		Helper.GUI.MaterialSelectionDialog.show(0, 0, "Choose material from library...",
			material, _ScenedMaterialSelectionAccepted)
	elseif name == "Scened.Material.AddBtn" then
		local material = Helper.getSelectedMaterial()
		local mat_idx = MyMatMgr:findMaterialByHash(material:getHash())
		if mat_idx == -1 then
			Helper.GUI.TextEnterDialog.show(0, 0, "Enter material name", "",
				_ScenedMaterialNameAccept, _ScenedMaterialNameValidate)
		else
			Helper.GUI.MessageBox.show(0, 0, "Warning", "Material already registered in library")
		end
	elseif name == "Scened.Material.Pass.Layers.AddBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		local units_max = MyDriver:getMaximalTextureUnitsAmount()
		local index = 0
		for i = 0, units_max - 1 do
			local layer = pass.Layers[i]
			local texture = layer:getTexture()
			if texture == nil then
				index = i
				break
			end
		end
		if index < units_max then
			local layer = pass.Layers[index]
			layer:setTexture(MyDriver:getTexture("#DefaultTexture"))
			_ScenedMaterialUpdatePassControls()
			_Ctrls.Layers.Listbox.Ctrl:setItemSelectState(index, true)
		end
	elseif name == "Scened.Material.Pass.Layers.RemBtn" then
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexture(nil)
		_ScenedMaterialUpdatePassControls()
	elseif name == "Scened.Material.Pass.Layers.MoveUpBtn" or
			name == "Scened.Material.Pass.Layers.MoveDownBtn" then
		local layer = _ScenedMaterialGetSelectedLayer()
		local pass = _ScenedMaterialGetSelectedPass()
		local units_max = MyDriver:getMaximalTextureUnitsAmount()
		local idx = Helper.getPassLayerIndex(pass, layer)
		local idx_new = idx
		if name == "Scened.Material.Pass.Layers.MoveUpBtn" then
			idx_new = idx_new - 1
		else
			idx_new = idx_new + 1
		end
		idx_new = Helper.getClampedValue(idx_new, 0, units_max - 1)
		if layer:getType() == vid.ETLT_NORMAL_MAP or
				pass.Layers[idx_new]:getType() == vid.ETLT_NORMAL_MAP  then
			local msg_text = "Can't move Normal Map Layer (it must have index 3)"
			Helper.GUI.MessageBox.show(0, 0, "Warning!", msg_text)
			LOG_WARN(msg_text)
			idx_new = idx
		end			
		if idx_new ~= idx then
			if pass:swapLayers(idx, idx_new) then
				_ScenedMaterialUpdatePassControls()
				local list_idx_new = -1
				for i = 0, idx_new do
					if pass.Layers[i]:getTexture() ~= nil then
						list_idx_new = list_idx_new + 1
					end
				end
				if list_idx_new ~= -1 and
						list_idx_new < _Ctrls.Layers.Listbox.Ctrl:getItemCount() then
					_Ctrls.Layers.Listbox.Ctrl:setItemSelectState(list_idx_new, true)
				end
			end
		end
	elseif name == "Scened.Material.Pass.Props.DiffuseBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, pass:getDiffuseColor(), _ScenedMaterialSetDiffuseColor)		
	elseif name == "Scened.Material.Pass.Props.AmbientBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.ColorSelectionDialog.show(
			"Ambient Color", 0, pass:getAmbientColor(), _ScenedMaterialSetAmbientColor)		
	elseif name == "Scened.Material.Pass.Props.SpecularBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.ColorSelectionDialog.show(
			"Specular Color", 0, pass:getSpecularColor(), _ScenedMaterialSetSpecularColor)		
	elseif name == "Scened.Material.Pass.Props.EmissiveBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.ColorSelectionDialog.show(
			"Emissive Color", 0, pass:getEmissiveColor(), _ScenedMaterialSetEmissiveColor)		
	elseif name == "Scened.Material.Pass.Props.ColorGenBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.WaveSetupDialog.show(
			"Color function", 0, pass:getColorFunc(), _ScenedMaterialSetColorFunc)		
	elseif name == "Scened.Material.Pass.Props.AlphaGenBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		Helper.GUI.WaveSetupDialog.show(
			"Alpha function", 0, pass:getAlphaFunc(), _ScenedMaterialSetAlphaFunc)		
	elseif name == "Scened.Material.Pass.Layers.Tex.OpenBtn" then
		local layer = _ScenedMaterialGetSelectedLayer()
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture...",
			MyResMgr:getMediaDirFull(res.EMT_TEXTURES), nil,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedMaterialTextureOpenAccepted)
	elseif name == "Scened.Material.Pass.Layers.Tex.ClearBtn" then
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexture(nil)
		_ScenedMaterialUpdatePassControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.AddBtn" then
		local types_table = {}
		for i = 0, vid.E_TEXCOORD_MOD_TYPE_COUNT - 1 do
			types_table[i + 1] = { Name = vid.getTexCoordModTypeReadableName(i), Value = i }
		end
		Helper.GUI.TypeSelectionDialog.show(0, 0, "Modificator type",
			types_table, _ScenedMaterialLayerAddTCMod)
	elseif name == "Scened.Material.Pass.Layers.TCMod.RemBtn" then
		local list = _Ctrls.Layers.TCMod.Listbox.Ctrl
		local index = list:getItemIndex(list:getFirstSelectedItem())
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:removeTexCoordModifier(index)
		_ScenedMaterialUpdateLayerControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.MoveUpBtn" or
			name == "Scened.Material.Pass.Layers.TCMod.MoveDownBtn" then
		local layer = _ScenedMaterialGetSelectedLayer()
		local idx = _ScenedMaterialGetSelectedLayerTCModIndex()
		local idx_new = idx
		if name == "Scened.Material.Pass.Layers.TCMod.MoveUpBtn" then
			idx_new = idx_new - 1
		else
			idx_new = idx_new + 1
		end
		idx_new = Helper.getClampedValue(idx_new, 0, layer:getTexCoordModifiersCount() - 1)
		if idx_new ~= idx then
			if layer:swapTexCoordModifiers(idx, idx_new) then
				_Ctrls.Layers.TCMod.Listbox.Ctrl:setItemSelectState(idx_new, true)
			end
		end
	elseif name == "Scened.Material.Pass.Layers.TCMod.Turbulent.Btn" then
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		Helper.GUI.WaveSetupDialog.show(
			"TCMod function", 0, tcmod:getTurbulent(), _ScenedMaterialSetTCModSetWave)
	elseif name == "Scened.Material.Pass.Shader.OpenBtn" then
		_ScenedMaterialShaderOpenDialogShow(_ScenedMaterialGPUProgOpenAccepted)
	elseif name == "Scened.Material.Pass.Shader.EditBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		local full_file_name = nil
		local sources = nil
		local gpu_prog = pass:getGPUProgram()
		_ScenedMaterialShaderEditorInitialGPUProg = gpu_prog
		if gpu_prog ~= nil then
			full_file_name = MyDriver:findGPUProgramFullFileName(gpu_prog)
		else
			sources = _ScenedMaterialRegenerateGPUProgram()
		end
		if full_file_name ~= nil or sources ~= nil then
			Helper.GUI.Scened.ShaderEditor.show(0, 0, "Shader editor",
				full_file_name, sources,
				_ScenedMaterialShaderEditorOpenCallback,
				_ScenedMaterialShaderEditorReloadCallback,
				_ScenedMaterialShaderEditorSaveCallback,
				_ScenedMaterialShaderEditorAcceptCallback,
				_ScenedMaterialShaderEditorCancelCallback
				)
		end
	elseif name == "Scened.Material.Pass.Shader.ReloadBtn" then
		_ScenedMaterialReloadGPUProgram()
	elseif name == "Scened.Material.Pass.Shader.ClearBtn" then
		local pass = _ScenedMaterialGetSelectedPass()
		pass:setGPUProgram(nil)
		_ScenedMaterialUpdatePassControls()
	end
end

function _ScenedMaterialItemSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Material.PassesListbox" then
		_ScenedMaterialUpdatePassControls()
	elseif name == "Scened.Material.Pass.Layers.Listbox" then
		_ScenedMaterialUpdateLayerControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.Listbox" then
		_ScenedMaterialUpdateLayerTCModControls()
	end
end

function _ScenedMaterialCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local pass = _ScenedMaterialGetSelectedPass()
	if name == "Scened.Material.Pass.Props.LightingCheck" then
		if _Ctrls.Props.LightingCheck.Ctrl:isSelected() then
			pass:setLightingMode(vid.ELM_8_LIGHTS)
		else
			pass:setLightingMode(vid.ELM_NONE)
		end
	elseif name == "Scened.Material.Pass.Layers.Tex.FilterCheck" then
		local filter = _Ctrls.Layers.Tex.FilterCheck.Ctrl:isSelected()
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:enableFilter(filter)
	else
		for i = 0, vid.E_MATERIAL_FLAG_COUNT - 1 do
			if name == "Scened.Material.Pass.Props."..string.format("Flag%dCheckbox", i) then
				local checkbox = tolua.cast(we.window, "CEGUI::Checkbox")
				pass:setFlag(i, checkbox:isSelected())
				break
			end
		end
	end
end

function _ScenedMaterialListSelectionAccepted(args)
	local pass = _ScenedMaterialGetSelectedPass()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Material.Pass.Props.EqBlendCombo" then
		local list = _Ctrls.Props.Combos.EqBlendCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setBlendEquation(index)
	elseif name == "Scened.Material.Pass.Props.SourceBlendCombo" then
		local list = _Ctrls.Props.Combos.SourceBlendCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setBlendFuncs(index, pass:getDstBlendFunc())
	elseif name == "Scened.Material.Pass.Props.DestBlendCombo" then
		local list = _Ctrls.Props.Combos.DestBlendCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setBlendFuncs(pass:getSrcBlendFunc(), index)
	elseif name == "Scened.Material.Pass.Props.ColorGenCombo" then
		local list = _Ctrls.Props.Combos.ColorGenCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setColorGen(index)
		_Ctrls.Props.Buttons.ColorGenBtn.Ctrl:setVisible(index == vid.ECGT_FUNCTION)
	elseif name == "Scened.Material.Pass.Props.AlphaGenCombo" then
		local list = _Ctrls.Props.Combos.AlphaGenCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setAlphaGen(index)
		_Ctrls.Props.Buttons.AlphaGenBtn.Ctrl:setVisible(index == vid.EAGT_FUNCTION)
	elseif name == "Scened.Material.Pass.Props.DepthTestCombo" then
		local list = _Ctrls.Props.Combos.DepthTestCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setDepthTest(index)
	elseif name == "Scened.Material.Pass.Props.AlphaTestCombo" then
		local list = _Ctrls.Props.Combos.AlphaTestCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		pass:setAlphaTest(index)
	elseif name == "Scened.Material.Pass.Layers.Tex.TypeCombo" then
		local list = _Ctrls.Layers.Tex.TypeCombo.Ctrl:getDropList()
		local texType = list:getItemIndex(list:getFirstSelectedItem())
		local layer = _ScenedMaterialGetSelectedLayer()
		local layerIdx = Helper.getPassLayerIndex(pass, layer)
		if texType ~= vid.ETLT_NORMAL_MAP or layerIdx == 3 then
			layer:setType(texType)
		else
			local msg_text = string.format(
				"Can't setup Layer %d for Normal Map (it must have index 3)",
				layerIdx)
			Helper.GUI.MessageBox.show(0, 0, "Warning!", msg_text)
			LOG_WARN(msg_text)
			_ScenedMaterialUpdateLayerControls()
		end
	elseif name == "Scened.Material.Pass.Layers.TCMod.TypeCombo" then
		local layer = _ScenedMaterialGetSelectedLayer()
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		local list = _Ctrls.Layers.TCMod.TypeCombo.Ctrl:getDropList()
		local modType = list:getItemIndex(list:getFirstSelectedItem())
		tcmod:setType(modType)
		_ScenedMaterialUpdateLayerTCModControls()
	elseif name == "Scened.Material.Pass.Layers.TCoord.AddrCombo" then
		local list = _Ctrls.Layers.TCoord.AddrCombo.Ctrl:getDropList()
		local addr = list:getItemIndex(list:getFirstSelectedItem())
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexCoordAddr(addr)
	elseif name == "Scened.Material.Pass.Layers.TCoord.GenCombo" then
		local list = _Ctrls.Layers.TCoord.GenCombo.Ctrl:getDropList()
		local index = list:getItemIndex(list:getFirstSelectedItem())
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexCoordGen(index)
		_ScenedMaterialUpdateLayerControls()
	end
end

function _ScenedMaterialValueChanged(args)
	local pass = _ScenedMaterialGetSelectedPass()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Material.Pass.Props.ShininessSpinner" then
		local shi = _Ctrls.Props.ShininessSpinner.Ctrl:getCurrentValue()
		shi = Helper.getClampedValue(shi, 0, 128)
		pass:setShininess(shi)
	elseif name == "Scened.Material.Pass.Layers.Tex.RGBScaleSpinner" then
		local rgbScale = _Ctrls.Layers.Tex.RGBScaleSpinner.Ctrl:getCurrentValue()
		rgbScale = Helper.getClampedValue(rgbScale, 1, 3)
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexRGBScale(rgbScale)
	elseif name == "Scened.Material.Pass.Layers.TCoord.ChannelSpinner" then
		local chnl = _Ctrls.Layers.TCoord.ChannelSpinner.Ctrl:getCurrentValue()
		chnl = Helper.getClampedValue(chnl, 0, 1)
		local layer = _ScenedMaterialGetSelectedLayer()
		layer:setTexCoordChannel(chnl)
	end
end

function _ScenedMaterialTextAccepted(args)
	local pass = _ScenedMaterialGetSelectedPass()
	if pass == nil then
		return
	end
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	
	if name == "Scened.Material.Pass.Layers.TCMod.Transform.Matrix00Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Transform.Matrix01Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Transform.Matrix10Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Transform.Matrix11Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Transform.Translate0Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Transform.Translate1Edit" then
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		tcmod:setMatrix(
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix00Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix01Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix10Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Matrix11Edit.Ctrl:getText()))
		tcmod:setTranslate(
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Translate0Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Transform.Editboxes.Translate1Edit.Ctrl:getText()))
		_ScenedMaterialUpdateLayerTCModControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.Scale.Scale0Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Scale.Scale1Edit" then
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		tcmod:setScale(
			tonumber(_Ctrls.Layers.TCMod.Clients.Scale.Editboxes.Scale0Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Scale.Editboxes.Scale1Edit.Ctrl:getText()))
		_ScenedMaterialUpdateLayerTCModControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.Scroll.Scroll0Edit" or
			name == "Scened.Material.Pass.Layers.TCMod.Scroll.Scroll1Edit" then
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		tcmod:setScroll(
			tonumber(_Ctrls.Layers.TCMod.Clients.Scroll.Editboxes.Scroll0Edit.Ctrl:getText()),
			tonumber(_Ctrls.Layers.TCMod.Clients.Scroll.Editboxes.Scroll1Edit.Ctrl:getText()))
		_ScenedMaterialUpdateLayerTCModControls()
	elseif name == "Scened.Material.Pass.Layers.TCMod.Rotate.RotateSpeedEdit" then
		local tcmod = _ScenedMaterialGetSelectedLayerTCMod()
		tcmod:setRotateSpeed(
			tonumber(_Ctrls.Layers.TCMod.Clients.Rotate.Editboxes.RotateSpeedEdit.Ctrl:getText()))
		_ScenedMaterialUpdateLayerTCModControls()
	else	
		for key, value in pairs(_Ctrls.Layers.TCoord.Gen.Editboxes) do
			local layer = _ScenedMaterialGetSelectedLayer()
			if name == value.Ctrl:getName() then
				local width =
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjWidth.Ctrl:getText())
				local height =
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjHeight.Ctrl:getText())
				local pos = core.vector3df(
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosX.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosY.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjPosZ.Ctrl:getText()))
				local dir = core.vector3df(
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirX.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirY.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjDirZ.Ctrl:getText()))
				local up = core.vector3df(
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpX.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpY.Ctrl:getText()),
					tonumber(_Ctrls.Layers.TCoord.Gen.Editboxes.ProjUpZ.Ctrl:getText()))
				layer:setProjectionParams(width, height, pos, dir, up)
				_ScenedMaterialUpdateLayerControls()
				return
			end
		end
	end
end
