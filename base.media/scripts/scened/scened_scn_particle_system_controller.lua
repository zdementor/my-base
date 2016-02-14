
local _Ctrls =
{
	Root = {},
	EmittersListbox = {},
	Buttons =
	{
		EmitterAddBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"		},
		EmitterRemBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"	},
	},
	Emitter =
	{
		Root = {},
		TabControl = {},
		Props =
		{
			Root = {},
			Buttons =
			{
				TexOpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
				TexClearBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
				StartColorMinBtn= {Ctrl = nil, ImageName = nil	},
				StartColorMaxBtn= {Ctrl = nil, ImageName = nil	},
				IncSpeedBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"		},
				DecSpeedBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"	},
			},
			StartColorMinImage = {},
			StartColorMaxImage = {},
			TextureImage = {},
			MultiLineEditboxes =
			{			
				TextureNameEditbox = {},
			},
			Editboxes =
			{
				DirXEditbox = {},
				DirYEditbox = {},
				DirZEditbox = {},
			},
			Spinners =
			{
				ParticleHeightSpinner = {},
				ParticleWidthSpinner = {},
				ParticlesPerSecMinSpinner = {},
				ParticlesPerSecMaxSpinner = {},
				LifeTimeMinSpinner = {},
				LifeTimeMaxSpinner = {},
				AngleDegSpinner = {},
				AppearVolumeMinXSpinner = {},
				AppearVolumeMinYSpinner = {},
				AppearVolumeMinZSpinner = {},
				AppearVolumeMaxXSpinner = {},
				AppearVolumeMaxYSpinner = {},
				AppearVolumeMaxZSpinner = {},				
			},
		},
		Affectors =
		{
			Root = {},
			Listbox = {},
			ClientArea = {},
			Buttons =
			{
				AddBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"		},
				RemBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"	},
			},
			Affector =
			{
				Root = {},
				EnabledCheck = {},
				AffectTimeMsSpinner = {},
				Clients =
				{
					FadeOutRoot = {},
					GravityRoot = {},
				},
				FadeOut =
				{
					TargetColorImage = {},
					TargetColorBtn = {},
				},
				Gravity =
				{
					Editboxes =
					{
						XEditbox = {},
						YEditbox = {},
						ZEditbox = {},
					},
				},
			},
		},
		EnabledCheck = {},
	},
}

local _Resources =
{
	Textures =
	{
		StartColorMinImage = nil,
		StartColorMaxImage = nil,
		TextureImageEmpty = nil,
		TextureImage = nil,
		FadeOutTargetColorImage = nil,
	},
	Imagesets =
	{
		TextureImageEmpty = nil,
		TextureImage = nil,
		FadeOutTargetColorImage = nil,
	},
}

function _ScenedInitParticleSystemControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_particle_system.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)
	
	_Ctrls.Emitter.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Root")
	_Ctrls.Emitter.Root.Ctrl:setVisible(false)
	
	_Ctrls.Emitter.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.TabControl"), "CEGUI::TabControl")
	
	_Ctrls.Emitter.Props.Root.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Props.Root")
	CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Props.Tab"):addChildWindow(
		_Ctrls.Emitter.Props.Root.Ctrl)
	_Ctrls.Emitter.Props.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")

	_Ctrls.Emitter.EnabledCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.EnabledCheck"), "CEGUI::Checkbox")
	_Ctrls.Emitter.EnabledCheck.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedParticleSystemCheckStateChanged)
	
	_Ctrls.EmittersListbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.EmittersListbox"), "CEGUI::Listbox")
	_Ctrls.EmittersListbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedParticleSystemItemSelectionChanged)
	
	Helper.GUI.initButtons("Scened.Scene.ParticleSystem.",
		_Ctrls.Buttons, "_ScenedParticleSystemWidgetClicked")
	Helper.GUI.initButtons("Scened.Scene.ParticleSystem.Emitter.Props.",
		_Ctrls.Emitter.Props.Buttons, "_ScenedParticleSystemWidgetClicked")

	_Ctrls.Emitter.Props.StartColorMinImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Props.StartColorMinImage")
	_Ctrls.Emitter.Props.StartColorMaxImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Props.StartColorMaxImage")
	_Ctrls.Emitter.Props.TextureImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Props.TextureImage")

	_Ctrls.Emitter.Props.MultiLineEditboxes.TextureNameEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Props.TextureNameEditbox"), "CEGUI::Editbox")
		
	local cegui_tex = nil
	local imageset = nil
	
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	_Resources.Textures.StartColorMinImage = cegui_tex
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	imageset = CEGUIImgsetMgr:create("Scened.Scene.ParticleSystem.Emitter.Props.StartColorMinImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Ctrls.Emitter.Props.StartColorMinImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.ParticleSystem.Emitter.Props.StartColorMinImage image:ColorImage")
	
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	_Resources.Textures.StartColorMaxImage = cegui_tex
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	imageset = CEGUIImgsetMgr:create("Scened.Scene.ParticleSystem.Emitter.Props.StartColorMaxImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Ctrls.Emitter.Props.StartColorMaxImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.ParticleSystem.Emitter.Props.StartColorMaxImage image:ColorImage")

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.TextureImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.ParticleSystem.Emitter.Props.TextureImageEmpty", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.TextureImageEmpty = imageset
		
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.TextureImage = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.ParticleSystem.Emitter.Props.TextureImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.TextureImage = imageset
	
	_Ctrls.Emitter.Props.TextureImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.ParticleSystem.Emitter.Props.TextureImageEmpty image:ColorImage")

	for key, value in pairs(_Ctrls.Emitter.Props.Editboxes) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Props."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedParticleSystemEditboxHandler)
	end
	for key, value in pairs(_Ctrls.Emitter.Props.Spinners) do
		value.Ctrl = tolua.cast(
			CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Props."..key), "CEGUI::Spinner")
		value.Ctrl:subscribeEvent("ValueChanged", _ScenedParticleSystemSpinnerHandler)
	end
		
	_Ctrls.Emitter.Affectors.Root.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Root")
	CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Affectors.Tab"):addChildWindow(
		_Ctrls.Emitter.Affectors.Root.Ctrl)
	_Ctrls.Emitter.Affectors.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	
	_Ctrls.Emitter.Affectors.ClientArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.ClientArea")
	_Ctrls.Emitter.Affectors.ClientArea.Ctrl:setVisible(false)
	
	Helper.GUI.initButtons("Scened.Scene.ParticleSystem.Emitter.Affectors.",
		_Ctrls.Emitter.Affectors.Buttons, _ScenedParticleSystemWidgetClicked)
		
	_Ctrls.Emitter.Affectors.Listbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Listbox"), "CEGUI::Listbox")
	_Ctrls.Emitter.Affectors.Listbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedParticleSystemItemSelectionChanged)
	
	_Ctrls.Emitter.Affectors.Affector.Root.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.Root")	
	_Ctrls.Emitter.Affectors.Affector.EnabledCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.EnabledCheck"), "CEGUI::Checkbox")
	_Ctrls.Emitter.Affectors.Affector.EnabledCheck.Ctrl:subscribeEvent(
		"CheckStateChanged",	_ScenedParticleSystemCheckStateChanged)
	_Ctrls.Emitter.Affectors.Affector.AffectTimeMsSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.AffectTimeMsSpinner"), "CEGUI::Spinner")
	_Ctrls.Emitter.Affectors.Affector.AffectTimeMsSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedParticleSystemSpinnerHandler)
		
	for key, value in pairs(_Ctrls.Emitter.Affectors.Affector.Clients) do
		value.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Affectors.Affector."..key)
		CEGUIWinMgr:getWindow("Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.ClientArea"):addChildWindow(
			value.Ctrl)
		value.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
		value.Ctrl:setVisible(false)			
	end
	
	_Ctrls.Emitter.Affectors.Affector.FadeOut.TargetColorImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.FadeOut.TargetColorImage")

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.FadeOutTargetColorImage = cegui_tex
	imageset = CEGUIImgsetMgr:create(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.FadeOut.TargetColorImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.FadeOutTargetColorImage = imageset
	_Ctrls.Emitter.Affectors.Affector.FadeOut.TargetColorImage.Ctrl:setProperty(
		"Image", "set:"..imageset:getName().." image:ColorImage")

	_Ctrls.Emitter.Affectors.Affector.FadeOut.TargetColorBtn.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.FadeOut.TargetColorBtn"), "CEGUI::ButtonBase")
	_Ctrls.Emitter.Affectors.Affector.FadeOut.TargetColorBtn.Ctrl:subscribeEvent(
		"Clicked", _ScenedParticleSystemWidgetClicked)
		
	for key, value in pairs(_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes) do
		value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.Gravity."..key), "CEGUI::Editbox")
		value.Ctrl:subscribeEvent("TextAccepted", _ScenedParticleSystemEditboxHandler)
	end	
	
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedParticleSystemUpdate,
		updateControls = _ScenedUpdateParticleSystemControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end

function _ScenedParticleSystemUpdate()
end

function _ScenedUpdateParticleSystemControls()
	local particles = _ScenedParticleSystemGetSelectedParticleSystem()
	_Ctrls.Root.Ctrl:setVisible(particles ~= nil)
	if particles == nil then
		return
	end
	_Ctrls.EmittersListbox.Ctrl:resetList()
	local emit_cnt = particles:getEmittersCount()
	for i = 0, emit_cnt - 1 do
		local emitter = particles:getEmitter(i)
		Helper.GUI.addListboxItem(_Ctrls.EmittersListbox.Ctrl, OPTIONS.ScenedGUI.SchemeName,
			scn.getParticleEmitterTypeReadableName(emitter:getType()), emitter:getType(), emitter)
	end
	if _Ctrls.EmittersListbox.Ctrl:getItemCount() > 0 then
		_Ctrls.EmittersListbox.Ctrl:setItemSelectState(0, true)
	end
	_ScenedParticleSystemUpdateEmitterControls()
end

function _ScenedParticleSystemGetSelectedEmitter()
	local item = _Ctrls.EmittersListbox.Ctrl:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	local emitter = tolua.cast(item:getUserData(), "scn::IParticleEmitter")
	return emitter
end

function _ScenedParticleSystemGetSelectedAffector()
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter == nil then
		return nil
	end
	local item = _Ctrls.Emitter.Affectors.Listbox.Ctrl:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	local affector = tolua.cast(item:getUserData(), "scn::IParticleAffector")
	return affector
end


function _ScenedParticleSystemGetSelectedParticleSystem()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local particles = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_PARTICLE_SYSTEM_SCENE_NODE then
			particles = tolua.cast(scene_node, "scn::IParticleSystemSceneNode")
		end
	end
	return particles
end

function _ScenedParticleSystemUpdateAffectorControls(update_affectors_list)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter == nil then
		return
	end
	if update_affectors_list ~= nil then
		if update_affectors_list then
			local sel_idx = 0
			local item = _Ctrls.Emitter.Affectors.Listbox.Ctrl:getFirstSelectedItem()
			if item ~= nil then
				sel_idx = _Ctrls.Emitter.Affectors.Listbox.Ctrl:getItemIndex(item)
			end
			_Ctrls.Emitter.Affectors.Listbox.Ctrl:resetList()
			local aff_cnt = emitter:getAffectorsCount()
			for i = 0, aff_cnt - 1 do
				local affector = emitter:getAffector(i)
				Helper.GUI.addListboxItem(_Ctrls.Emitter.Affectors.Listbox.Ctrl, OPTIONS.ScenedGUI.SchemeName,
					scn.getParticleAffectorTypeReadableName(affector:getType()), affector:getType(), affector)
			end
			if _Ctrls.Emitter.Affectors.Listbox.Ctrl:getItemCount() > sel_idx then
				_Ctrls.Emitter.Affectors.Listbox.Ctrl:setItemSelectState(sel_idx, true)
			end
		end
	end
	_Ctrls.Emitter.Affectors.ClientArea.Ctrl:setVisible(false)
	for key, value in pairs(_Ctrls.Emitter.Affectors.Affector.Clients) do
		value.Ctrl:setVisible(false)			
	end
	local affector = _ScenedParticleSystemGetSelectedAffector()
	if affector == nil then
		return
	end
	_Ctrls.Emitter.Affectors.ClientArea.Ctrl:setVisible(true)
	_Ctrls.Emitter.Affectors.Affector.EnabledCheck.Ctrl:setSelected(affector:isEnabled())
	_Ctrls.Emitter.Affectors.Affector.AffectTimeMsSpinner.Ctrl:setCurrentValue(affector:getAffectTimeMs())
	if affector:getType() == scn.EPAT_FADEOUT then
		_Ctrls.Emitter.Affectors.Affector.Clients.FadeOutRoot.Ctrl:setVisible(true)
		local aff_fade = tolua.cast(affector, "scn::IParticleFadeOutAffector")
		if aff_fade ~= nil then
			Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.FadeOutTargetColorImage, aff_fade:getTargetColor())
		end
	elseif affector:getType() == scn.EPAT_GRAVITY then
		_Ctrls.Emitter.Affectors.Affector.Clients.GravityRoot.Ctrl:setVisible(true)
		local aff_grav = tolua.cast(affector, "scn::IParticleGravityAffector")
		if aff_grav ~= nil then
			_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.XEditbox.Ctrl:setText(string.format("%.4f", aff_grav:getGravity().X))
			_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.YEditbox.Ctrl:setText(string.format("%.4f", aff_grav:getGravity().Y))
			_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.ZEditbox.Ctrl:setText(string.format("%.4f", aff_grav:getGravity().Z))
		end
	end
	if affector:isEnabled() then
		_Ctrls.Emitter.Affectors.Affector.Root.Ctrl:enable()
	else
		_Ctrls.Emitter.Affectors.Affector.Root.Ctrl:disable()
	end
end

function _ScenedParticleSystemUpdateEmitterControls()
	_Ctrls.Emitter.Root.Ctrl:setVisible(false)
	Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.StartColorMinImage, img.SColor(255, 0, 0, 0))
	Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.StartColorMaxImage, img.SColor(255, 0, 0, 0))
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter == nil then
		return
	end
	_Ctrls.Emitter.Root.Ctrl:setVisible(true)
	local imageset = _Resources.Imagesets.TextureImage
	local cegui_tex = _Resources.Textures.TextureImage
	local my_tex = emitter:getTexture()

	if my_tex ~= nil then
		local fullfname = MyDriver:findTextureName(my_tex)
		MyCEGUI.setTexture(cegui_tex, my_tex)
		imageset:undefineAllImages()
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		local relfname = Helper.getRelFileName(fullfname, MyResMgr:getMediaDirFull(res.EMT_TEXTURES))
		_Ctrls.Emitter.Props.MultiLineEditboxes.TextureNameEditbox.Ctrl:setText(relfname)
	else
		imageset = _Resources.Imagesets.TextureImageEmpty
		_Ctrls.Emitter.Props.MultiLineEditboxes.TextureNameEditbox.Ctrl:setText("")
	end

	_Ctrls.Emitter.Props.TextureImage.Ctrl:setProperty(
		"Image", "set:"..imageset:getName().." image:ColorImage")
	Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.StartColorMinImage, emitter:getMinStartColor())
	Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.StartColorMaxImage, emitter:getMaxStartColor())
	
	_Ctrls.Emitter.Props.Editboxes.DirXEditbox.Ctrl:setText(string.format("%.4f", emitter:getDirection().X))
	_Ctrls.Emitter.Props.Editboxes.DirYEditbox.Ctrl:setText(string.format("%.4f", emitter:getDirection().Y))
	_Ctrls.Emitter.Props.Editboxes.DirZEditbox.Ctrl:setText(string.format("%.4f", emitter:getDirection().Z))
	
	_Ctrls.Emitter.Props.Spinners.ParticleHeightSpinner.Ctrl:setText(string.format("%d", emitter:getParticleSize().Width))
	_Ctrls.Emitter.Props.Spinners.ParticleWidthSpinner.Ctrl:setText(string.format("%d", emitter:getParticleSize().Height))
	_Ctrls.Emitter.Props.Spinners.ParticlesPerSecMinSpinner.Ctrl:setText(string.format("%d", emitter:getMinParticlePerSecond()))
	_Ctrls.Emitter.Props.Spinners.ParticlesPerSecMaxSpinner.Ctrl:setText(string.format("%d", emitter:getMaxParticlePerSecond()))
	_Ctrls.Emitter.Props.Spinners.LifeTimeMinSpinner.Ctrl:setText(string.format("%d", emitter:getMinLifeTime()))
	_Ctrls.Emitter.Props.Spinners.LifeTimeMaxSpinner.Ctrl:setText(string.format("%d", emitter:getMaxLifeTime()))
	_Ctrls.Emitter.Props.Spinners.AngleDegSpinner.Ctrl:setText(string.format("%d", emitter:getMaxAngleDegrees()))
	
	local box = core.aabbox3df(emitter:getAppearVolume())
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMinXSpinner.Ctrl:setText(string.format("%d", box.MinEdge.X))
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMinYSpinner.Ctrl:setText(string.format("%d", box.MinEdge.Y))
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMinZSpinner.Ctrl:setText(string.format("%d", box.MinEdge.Z))
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxXSpinner.Ctrl:setText(string.format("%d", box.MaxEdge.X))
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxYSpinner.Ctrl:setText(string.format("%d", box.MaxEdge.Y))
	_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxZSpinner.Ctrl:setText(string.format("%d", box.MaxEdge.Z))
	
	_Ctrls.Emitter.Affectors.Listbox.Ctrl:resetList()
	_ScenedParticleSystemUpdateAffectorControls(true)
		
	_Ctrls.Emitter.EnabledCheck.Ctrl:setSelected(emitter:isEnabled())
	if emitter:isEnabled() then
		_Ctrls.Emitter.TabControl.Ctrl:enable()
	else
		_Ctrls.Emitter.TabControl.Ctrl:setSelectedTabAtIndex(0)
		_Ctrls.Emitter.TabControl.Ctrl:disable()
	end
end

function _ScenedParticleSystemAddNewEmitter(emitter_type)
	local particles = _ScenedParticleSystemGetSelectedParticleSystem()
	if particles ~= nil then
		particles:addEmitter(emitter_type)
		_ScenedUpdateParticleSystemControls()
		if _Ctrls.EmittersListbox.Ctrl:getItemCount() == particles:getEmittersCount() then
			_Ctrls.EmittersListbox.Ctrl:setItemSelectState(particles:getEmittersCount() - 1, true)
			_ScenedParticleSystemUpdateEmitterControls()
			_Ctrls.Emitter.TabControl.Ctrl:setSelectedTabAtIndex(0)
		end
	end
end

function _ScenedParticleSystemAddNewAffector(affector_type)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter ~= nil then
		emitter:addAffector(affector_type)
		_ScenedParticleSystemUpdateEmitterControls()
		if _Ctrls.Emitter.Affectors.Listbox.Ctrl:getItemCount() == emitter:getAffectorsCount() then
			_Ctrls.Emitter.Affectors.Listbox.Ctrl:setItemSelectState(emitter:getAffectorsCount() - 1, true)
			_ScenedParticleSystemUpdateAffectorControls()
		end
	end
end

function _ScenedParticleSystemSetMinStartColor(color)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter ~= nil then
		emitter:setMinStartColor(color)
		_ScenedParticleSystemUpdateEmitterControls()
	end	
end

function _ScenedParticleSystemSetMaxStartColor(color)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter ~= nil then
		emitter:setMaxStartColor(color)
		_ScenedParticleSystemUpdateEmitterControls()
	end	
end

function _ScenedParticleSystemSetFadeOutTargetColor(color)
	local affector = _ScenedParticleSystemGetSelectedAffector()
	local aff_fade = nil
	if affector ~= nil and affector:getType() == scn.EPAT_FADEOUT then
		aff_fade = tolua.cast(affector, "scn::IParticleFadeOutAffector")
	end
	if aff_fade ~= nil then
		aff_fade:setTargetColor(color)
		_ScenedParticleSystemUpdateAffectorControls()
	end
end

function _ScenedParticleSystemTextureOpenAccepted(full_file_name)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter ~= nil then
		local texture = MyDriver:getTexture(full_file_name)
		if texture ~= nil then
			emitter:setTexture(texture)
			_ScenedParticleSystemUpdateEmitterControls()
		end
	end
end
	
function _ScenedParticleSystemWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.ParticleSystem.EmitterAddBtn" then
		local types_table = {}
		for i = 0, scn.E_PARTICLE_EMITTER_TYPE_COUNT - 1 do
			types_table[i + 1] = {Name = scn.getParticleEmitterTypeReadableName(i), Value = i}
		end
		Helper.GUI.TypeSelectionDialog.show(0, 0,
			"Add Emitter", types_table, _ScenedParticleSystemAddNewEmitter)
	elseif name == "Scened.Scene.ParticleSystem.EmitterRemBtn" then
		local particles = _ScenedParticleSystemGetSelectedParticleSystem()
		local item = _Ctrls.EmittersListbox.Ctrl:getFirstSelectedItem()
		if item ~= nil and particles ~= nil then
			local idx = _Ctrls.EmittersListbox.Ctrl:getItemIndex(item)
			particles:removeEmitter(idx)
			_ScenedUpdateParticleSystemControls()			
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.TexOpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture...",
			MyResMgr:getMediaDirFull(res.EMT_TEXTURES), nil,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedParticleSystemTextureOpenAccepted)
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.TexClearBtn" then
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			emitter:setTexture(nil)
			_ScenedParticleSystemUpdateEmitterControls()
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.StartColorMinBtn" then
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			Helper.GUI.ColorSelectionDialog.show(
				"Min Start Color", 0, emitter:getMinStartColor(), _ScenedParticleSystemSetMinStartColor)		
		end		
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.StartColorMaxBtn" then
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			Helper.GUI.ColorSelectionDialog.show(
				"Max Start Color", 0, emitter:getMaxStartColor(), _ScenedParticleSystemSetMaxStartColor)
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.AddBtn" then
		local types_table = {}
		for i = 0, scn.E_PARTICLE_AFFECTOR_TYPE_COUNT - 1 do
			types_table[i + 1] = {Name = scn.getParticleAffectorTypeReadableName(i), Value = i}
		end
		Helper.GUI.TypeSelectionDialog.show(0, 0,
			"Add Affector", types_table, _ScenedParticleSystemAddNewAffector)
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.RemBtn" then	
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		local item = _Ctrls.Emitter.Affectors.Listbox.Ctrl:getFirstSelectedItem()
		if item ~= nil and emitter ~= nil then
			local idx = _Ctrls.Emitter.Affectors.Listbox.Ctrl:getItemIndex(item)
			emitter:removeAffector(idx)
			_ScenedParticleSystemUpdateAffectorControls(true)
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.FadeOut.TargetColorBtn" then
		local affector = _ScenedParticleSystemGetSelectedAffector()
		local aff_fade = nil
		if affector ~= nil and affector:getType() == scn.EPAT_FADEOUT then
			aff_fade = tolua.cast(affector, "scn::IParticleFadeOutAffector")
		end
		if aff_fade ~= nil then
			Helper.GUI.ColorSelectionDialog.show(
				"Target Color", 0, aff_fade:getTargetColor(), _ScenedParticleSystemSetFadeOutTargetColor)	
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.IncSpeedBtn" then
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			local dir = core.vector3df(emitter:getDirection())
			dir = dir * 1.1
			emitter:setDirection(dir)
			_ScenedParticleSystemUpdateEmitterControls()
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.DecSpeedBtn" then
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			local dir = core.vector3df(emitter:getDirection())
			dir = dir * 0.9
			emitter:setDirection(dir)
			_ScenedParticleSystemUpdateEmitterControls()
		end
	end
end

function _ScenedParticleSystemCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.ParticleSystem.Emitter.EnabledCheck" then
		local selected = _Ctrls.Emitter.EnabledCheck.Ctrl:isSelected()
		local emitter = _ScenedParticleSystemGetSelectedEmitter()
		if emitter ~= nil then
			emitter:setEnabled(selected)
			_ScenedParticleSystemUpdateEmitterControls()
		end
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.EnabledCheck" then
		local selected = _Ctrls.Emitter.Affectors.Affector.EnabledCheck.Ctrl:isSelected()
		local affector = _ScenedParticleSystemGetSelectedAffector()
		if affector ~= nil then
			affector:setEnabled(selected)
			_ScenedParticleSystemUpdateAffectorControls()
		end
	end
end

function _ScenedParticleSystemItemSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.ParticleSystem.EmittersListbox" then
		_ScenedParticleSystemUpdateEmitterControls()
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Listbox" then
		_ScenedParticleSystemUpdateAffectorControls()
	end
end

function _ScenedParticleSystemEditboxHandler(args)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter == nil then
		return
	end
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.ParticleSystem.Emitter.Props.DirXEditbox" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.DirYEditbox" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.DirZEditbox" then
		local dir = core.vector3df(
			tonumber(_Ctrls.Emitter.Props.Editboxes.DirXEditbox.Ctrl:getText()),
			tonumber(_Ctrls.Emitter.Props.Editboxes.DirYEditbox.Ctrl:getText()),
			tonumber(_Ctrls.Emitter.Props.Editboxes.DirZEditbox.Ctrl:getText()))
		emitter:setDirection(dir)
		_ScenedParticleSystemUpdateEmitterControls()
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.Gravity.XEditbox" or
			 name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.Gravity.YEditbox" or
			  name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.Gravity.ZEditbox" then
		local affector = _ScenedParticleSystemGetSelectedAffector()
		local aff_grav = nil
		if affector ~= nil and affector:getType() == scn.EPAT_GRAVITY then
			aff_grav = tolua.cast(affector, "scn::IParticleGravityAffector")
		end
		if aff_grav ~= nil then
			local dir = core.vector3df(
				tonumber(_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.XEditbox.Ctrl:getText()),
				tonumber(_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.YEditbox.Ctrl:getText()),
				tonumber(_Ctrls.Emitter.Affectors.Affector.Gravity.Editboxes.ZEditbox.Ctrl:getText()))
			aff_grav:setGravity(dir)
			_ScenedParticleSystemUpdateAffectorControls()
		end
	end
end

function _ScenedParticleSystemSpinnerHandler(args)
	local emitter = _ScenedParticleSystemGetSelectedEmitter()
	if emitter == nil then
		return
	end
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	
	if name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMinXSpinner" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMinYSpinner" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMinZSpinner" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMaxXSpinner" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMaxYSpinner" or
			name == "Scened.Scene.ParticleSystem.Emitter.Props.AppearVolumeMaxZSpinner" then
		local box = core.aabbox3df(emitter:getAppearVolume())
		box.MinEdge:set(
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMinXSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMinYSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMinZSpinner.Ctrl:getCurrentValue())
		box.MaxEdge:set(
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxXSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxYSpinner.Ctrl:getCurrentValue(),
			_Ctrls.Emitter.Props.Spinners.AppearVolumeMaxZSpinner.Ctrl:getCurrentValue())
		emitter:setAppearVolume(box)
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.ParticleHeightSpinner" then
		local size = core.dimension2df(emitter:getParticleSize())
		size.Width = _Ctrls.Emitter.Props.Spinners.ParticleHeightSpinner.Ctrl:getCurrentValue()
		emitter:setParticleSize(size)
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.ParticleWidthSpinner" then
		local size = core.dimension2df(emitter:getParticleSize())
		size.Height = _Ctrls.Emitter.Props.Spinners.ParticleWidthSpinner.Ctrl:getCurrentValue()
		emitter:setParticleSize(size)		
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.ParticlesPerSecMinSpinner" then
		emitter:setMinParticlePerSecond(_Ctrls.Emitter.Props.Spinners.ParticlesPerSecMinSpinner.Ctrl:getCurrentValue())
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.ParticlesPerSecMaxSpinner" then
		emitter:setMaxParticlePerSecond(_Ctrls.Emitter.Props.Spinners.ParticlesPerSecMaxSpinner.Ctrl:getCurrentValue())
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.LifeTimeMinSpinner" then
		emitter:setMinLifeTime(_Ctrls.Emitter.Props.Spinners.LifeTimeMinSpinner.Ctrl:getCurrentValue())
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.LifeTimeMaxSpinner" then
		emitter:setMaxLifeTime(_Ctrls.Emitter.Props.Spinners.LifeTimeMaxSpinner.Ctrl:getCurrentValue())
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Props.AngleDegSpinner" then
		emitter:setMaxAngleDegrees(_Ctrls.Emitter.Props.Spinners.AngleDegSpinner.Ctrl:getCurrentValue())
	elseif name == "Scened.Scene.ParticleSystem.Emitter.Affectors.Affector.AffectTimeMsSpinner" then
		local affector = _ScenedParticleSystemGetSelectedAffector()
		if affector ~= nil then
			affector:setAffectTimeMs(
				_Ctrls.Emitter.Affectors.Affector.AffectTimeMsSpinner.Ctrl:getCurrentValue())
		end
	end
end
