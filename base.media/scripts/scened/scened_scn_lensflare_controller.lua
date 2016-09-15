
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
			DiffuseImage = {},
			CoronaTexImage = {},
			Buttons =
			{
				DiffuseBtn = {},
				CoronaTexOpenBtn = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				CoronaTexViewBtn = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				LensAddBtn       = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"      },
			},
			Lens =
			{
				ScrollArea = {},
				LensArray = {},
			},
			EtalonScreenWidthSpinner = {},
			EtalonScreenHeightSpinner = {},
		},
	}
}

local _Resources =
{
	Textures =
	{
		DiffuseImage = nil,
		CoronaTexImage = nil,
		CoronaTexImageEmpty = nil,
	},
	Imagesets =
	{
		CoronaTexImage = nil,
		CoronaTexImageEmpty = nil,
	},
}

local _DiffuseColor = img.SColor()

local function _ScenedLensFlareGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local lensflare_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_LENS_FLARE_SCENE_NODE then
			lensflare_scene_node = tolua.cast(scene_node, "scn::ILensFlareSceneNode")
		end
	end
	return lensflare_scene_node
end

local function _ScenedLensFlareClearLensControls(startFromIndex)
	for i = startFromIndex + 1, table.getn(_Ctrls.Tabs.Props.Lens.LensArray) do
		for key, btn in pairs(_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons) do
			CEGUIWinMgr:destroyWindow(btn.Ctrl)
		end
		CEGUIWinMgr:destroyWindow(_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceLabel.Ctrl)
		CEGUIWinMgr:destroyWindow(_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceSpinner.Ctrl)
		CEGUIWinMgr:destroyWindow(_Ctrls.Tabs.Props.Lens.LensArray[i].Image.Ctrl)
		CEGUIWinMgr:destroyWindow(_Ctrls.Tabs.Props.Lens.LensArray[i].Ctrl)
		
		Helper.GUI.freeCEGUIResources(_Ctrls.Tabs.Props.Lens.LensArray[i].Resources)
		
		_Ctrls.Tabs.Props.Lens.LensArray[i] = nil
	end
end

local function _ScenedLensFlareUpdateLensControls(index, lens)

	local lensWnd =  nil
	local lensImg = nil
	local lensSpaceSpinner = nil
	local i = index + 1

	local namePfx = "Scened.Scene.LensFlare.Props.Lens.ScrollArea.Lens"..tostring(index)
		
	if _Ctrls.Tabs.Props.Lens.LensArray[i] == nil then

		local lensHeight = 77
		local lensH1 = lensHeight * index
		local lensH2 = lensH1 + lensHeight

		lensWnd =  CEGUIWinMgr:createWindow("MyScened/StaticText", namePfx)
		_Ctrls.Tabs.Props.Lens.ScrollArea.Ctrl:addChildWindow(lensWnd)
		_Ctrls.Tabs.Props.Lens.LensArray[i] = {}
		_Ctrls.Tabs.Props.Lens.LensArray[i].Ctrl = lensWnd
		lensWnd:setVisible(true)
		lensWnd:setProperty("FrameEnabled", "true")
		lensWnd:setProperty("UnifiedAreaRect",
			"{{0.0,0},{0.0,"..tostring(lensH1).."},{0.95,0},{0.0,"..tostring(lensH2).."}}")

		local imgMargine = 8
		local imageset = nil
		local cegui_tex = nil

		lensImg =  CEGUIWinMgr:createWindow("MyScened/StaticImage", namePfx..".Image")
		lensWnd:addChildWindow(lensImg)
		_Ctrls.Tabs.Props.Lens.LensArray[i].Image = {}
		_Ctrls.Tabs.Props.Lens.LensArray[i].Image.Ctrl = lensImg
		lensImg:setVisible(true)
		lensImg:setProperty("FrameEnabled", "true")
		lensImg:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(imgMargine).."},"..
			"{0.0,"..tostring(imgMargine).."},"..
			"{0.0,"..tostring(lensHeight - imgMargine).."},"..
			"{0.0,"..tostring(lensHeight - imgMargine).."}}")

		_Ctrls.Tabs.Props.Lens.LensArray[i].Resources =
			{ Textures = { LensTexImage = nil, LensTexImageEmpty = nil }, Imagesets = { LensTexImage = nil, LensTexImageEmpty = nil } }
			
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		_Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Textures.LensTexImageEmpty = cegui_tex
		imageset = CEGUIImgsetMgr:create(namePfx..".LensTexImageEmpty", cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		_Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Imagesets.LensTexImageEmpty = imageset
		
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		_Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Textures.LensTexImage = cegui_tex
		imageset = CEGUIImgsetMgr:create(namePfx..".LensTexImage", cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		_Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Imagesets.LensTexImage = imageset

		local btnMargine = 3
		local btnSize = 30
		local btn = nil
		_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons = {}

		btn =  CEGUIWinMgr:createWindow("MyScened/Button", namePfx..".OpenBtn")
		lensWnd:addChildWindow(btn)
		btn:setVisible(true)
		btn:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(lensHeight).."},"..
			"{0.0,"..tostring(imgMargine).."},"..
			"{0.0,"..tostring(lensHeight + btnSize).."},"..
			"{0.0,"..tostring(imgMargine + btnSize).."}}")
		btn:setProperty("Tooltip", "Open image for Lens"..tostring(index))
		_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons.OpenBtn = { Ctrl = btn, ImageName = "set:TangoGUIIcons image:actions.document-open" }

		btn =  CEGUIWinMgr:createWindow("MyScened/Button", namePfx..".ViewBtn")
		lensWnd:addChildWindow(btn)
		btn:setVisible(true)
		btn:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(lensHeight + btnSize + btnMargine).."},"..
			"{0.0,"..tostring(imgMargine).."},"..
			"{0.0,"..tostring(lensHeight + 2 * btnSize + btnMargine).."},"..
			"{0.0,"..tostring(imgMargine + btnSize).."}}")
		btn:setProperty("Tooltip", "View image for Lens"..tostring(index))
		_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons.ViewBtn = { Ctrl = btn, ImageName = "set:TangoGUIIcons image:actions.image-view" }

		btn =  CEGUIWinMgr:createWindow("MyScened/Button", namePfx..".RemBtn")
		lensWnd:addChildWindow(btn)
		btn:setVisible(true)
		btn:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(lensHeight + 2 * btnSize + 2 * btnMargine).."},"..
			"{0.0,"..tostring(imgMargine).."},"..
			"{0.0,"..tostring(lensHeight + 3 * btnSize + 2 * btnMargine).."},"..
			"{0.0,"..tostring(imgMargine + btnSize).."}}")
		btn:setProperty("Tooltip", "Remove Lens"..tostring(index))
		_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons.RemBtn = { Ctrl = btn, ImageName = "set:TangoGUIIcons image:actions.list-remove" }
		
		Helper.GUI.initButtons(namePfx..".", _Ctrls.Tabs.Props.Lens.LensArray[i].Buttons, _ScenedLensFlareWidgetClicked)
		
		local spinnerW = 80
		local spinnerH = 30
		
		lensSpaceSpinner =  tolua.cast(CEGUIWinMgr:createWindow("MyScened/Spinner", namePfx..".SpaceSpinner"), "CEGUI::Spinner")
		lensWnd:addChildWindow(lensSpaceSpinner)
		_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceSpinner = {}
		_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceSpinner.Ctrl = lensSpaceSpinner
		lensSpaceSpinner:setVisible(true)
		lensSpaceSpinner:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(lensHeight + spinnerW * 0.75).."},"..
			"{0.0,"..tostring(imgMargine + btnSize + btnMargine).."},"..
			"{0.0,"..tostring(lensHeight + spinnerW * 1 + spinnerW).."},"..
			"{0.0,"..tostring(imgMargine + btnSize + btnMargine + spinnerH).."}}")
		lensSpaceSpinner:subscribeEvent("ValueChanged", _ScenedLensFlareSpinnerValueChanged)
		lensSpaceSpinner:setProperty("Font", "Tahoma-10")
		lensSpaceSpinner:setTextInputMode(CEGUI.Spinner.FloatingPoint)
		lensSpaceSpinner:setCurrentValue(0.0)
		lensSpaceSpinner:setStepSize(0.01)
		lensSpaceSpinner:setMaximumValue(2)
		lensSpaceSpinner:setMinimumValue(-2)

		local lensSpaceLabel =  CEGUIWinMgr:createWindow("MyScened/StaticText", namePfx..".SpaceLabel")
		lensWnd:addChildWindow(lensSpaceLabel)
		_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceLabel = {}
		_Ctrls.Tabs.Props.Lens.LensArray[i].SpaceLabel.Ctrl = lensSpaceLabel
		lensSpaceLabel:setVisible(true)
		lensSpaceLabel:setProperty("Text", "Space")
		lensSpaceLabel:setProperty("UnifiedAreaRect",
			"{{0.0,"..tostring(lensHeight ).."},"..
			"{0.0,"..tostring(imgMargine + btnSize + btnMargine).."},"..
			"{0.0,"..tostring(lensHeight + spinnerW * 0.75 - 5).."},"..
			"{0.0,"..tostring(imgMargine + btnSize + btnMargine + spinnerH).."}}")
		lensSpaceLabel:setProperty("Font", "Tahoma-10")
		lensSpaceLabel:setProperty("TextColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
		lensSpaceLabel:setProperty("FrameEnabled", "False")
		lensSpaceLabel:setProperty("HorzFormatting", "RightAligned")
		lensSpaceLabel:setProperty("BackgroundEnabled", "False")

		_Ctrls.Tabs.Props.Lens.LensArray[i].Index = index
	else
		lensWnd          = _Ctrls.Tabs.Props.Lens.LensArray[i].Ctrl
		lensImg          = _Ctrls.Tabs.Props.Lens.LensArray[i].Image.Ctrl
		lensSpaceSpinner = _Ctrls.Tabs.Props.Lens.LensArray[i].SpaceSpinner.Ctrl
	end

	local my_tex = lens:getTexture()
	local imageset = _Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Imagesets.LensTexImageEmpty
	
	if my_tex ~= nil then
		local cegui_tex = _Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Textures.LensTexImage
		MyCEGUI.setTexture(cegui_tex, my_tex, false)
		imageset = _Ctrls.Tabs.Props.Lens.LensArray[i].Resources.Imagesets.LensTexImage
		imageset:undefineAllImages()
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	end

	lensImg:setProperty("Image", "set:"..imageset:getName().." image:ColorImage")

	lensSpaceSpinner:setCurrentValue(lens.spaceConst)

end

local function _ScenedUpdateLensFlareControls()
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(lensflare_scene_node ~= nil)
	if lensflare_scene_node == nil then
		return
	end
	_ScenedLensFlareClearLensControls(lensflare_scene_node:getLensCount())
	for i = 0, lensflare_scene_node:getLensCount() - 1 do
		_ScenedLensFlareUpdateLensControls(i, lensflare_scene_node:getLens(i))
	end
	
	_DiffuseColor:set(lensflare_scene_node:getColor())

	Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.DiffuseImage, _DiffuseColor)
	
	local imageset = _Resources.Imagesets.CoronaTexImageEmpty
	
	local my_tex = lensflare_scene_node:getCorona()

	if my_tex ~= nil then
		local cegui_tex = _Resources.Textures.CoronaTexImage

		MyCEGUI.setTexture(cegui_tex, my_tex, false)
		imageset = _Resources.Imagesets.CoronaTexImage
		imageset:undefineAllImages()
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	end

	_Ctrls.Tabs.Props.CoronaTexImage.Ctrl:setProperty(
		"Image", "set:"..imageset:getName().." image:ColorImage")
	
	local scrSize = lensflare_scene_node:getEtalonScreenSize()
	
	_Ctrls.Tabs.Props.EtalonScreenWidthSpinner.Ctrl:setCurrentValue(scrSize.Width)
	_Ctrls.Tabs.Props.EtalonScreenHeightSpinner.Ctrl:setCurrentValue(scrSize.Height)

end

local function _ScenedLensFlareSetDiffuseColor(color)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	_DiffuseColor:set(color)
	lensflare_scene_node:setColor(_DiffuseColor)
	_ScenedUpdateLensFlareControls()
end

local function _ScenedLensFlareCoronaOpenAccepted(full_file_name)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	local tex = MyDriver:getTexture(full_file_name)
	if tex ~= nil then
		lensflare_scene_node:setCorona(tex)
		_ScenedUpdateLensFlareControls()
	else
		LOG_WARN("Can not open Corona texture from "..full_file_name)
	end
end

local function _ScenedLensFlareCoronaClosedImage()
end

local function _ScenedLensFlareLensOpenAccepted(full_file_name, rel_full_file_name, user_data)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	local lensIdx = user_data
	local tex = MyDriver:getTexture(full_file_name)
	if tex ~= nil then
		local lens = lensflare_scene_node:getLens(lensIdx)
		lensflare_scene_node:setLens(lensIdx, tex, lens.spaceConst)
		_ScenedLensFlareUpdateLensControls(lensIdx, lens)
	else
		LOG_WARN("Can not open Lens"..tostring(lensIdx).." texture from "..full_file_name)
	end
end

local function _ScenedLensFlareLensClosedImage(full_file_name, rel_full_file_name, user_data)
end

local function _ScenedLensFlareLensAddAccepted(full_file_name)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	local tex = MyDriver:getTexture(full_file_name)
	if tex ~= nil then
		lensflare_scene_node:addLens(tex, 0.0)
		_ScenedUpdateLensFlareControls()
	else
		LOG_WARN("Can not open Lens texture from "..full_file_name)
	end
end

function _ScenedLensFlareSpinnerValueChanged(args)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.LensFlare.Props.EtalonScreenWidthSpinner" then
		local w = _Ctrls.Tabs.Props.EtalonScreenWidthSpinner.Ctrl:getCurrentValue()
		local h = lensflare_scene_node:getEtalonScreenSize().Height
		lensflare_scene_node:setEtalonScreenSize(w, h)
	elseif name == "Scened.Scene.LensFlare.Props.EtalonScreenHeightSpinner" then
		local w = lensflare_scene_node:getEtalonScreenSize().Width
		local h = _Ctrls.Tabs.Props.EtalonScreenHeightSpinner.Ctrl:getCurrentValue()
		lensflare_scene_node:setEtalonScreenSize(w, h)
	else
		for i = 1, table.getn(_Ctrls.Tabs.Props.Lens.LensArray) do
			local spinner = _Ctrls.Tabs.Props.Lens.LensArray[i].SpaceSpinner.Ctrl
			if spinner:getName() == name then
				local lensIdx = _Ctrls.Tabs.Props.Lens.LensArray[i].Index
				local lens = lensflare_scene_node:getLens(lensIdx)
				local spaceConst = spinner:getCurrentValue()
				spinner:setText(string.format("%.3f", spaceConst))
				spaceConst = tonumber(spinner:getText())
				lensflare_scene_node:setLens(lensIdx, lens:getTexture(), spaceConst)
			end
		end
	end
end

function _ScenedLensFlareWidgetClicked(args)
	local lensflare_scene_node = _ScenedLensFlareGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.LensFlare.Props.DiffuseBtn" then
		Helper.GUI.ColorSelectionDialog.show(
			"Diffuse Color", 0, _DiffuseColor, _ScenedLensFlareSetDiffuseColor)
	elseif name == "Scened.Scene.LensFlare.Props.CoronaTexOpenBtn" then
		local tex = lensflare_scene_node:getCorona()
		local tex_fname = nil
		local start_dir = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if tex ~= nil then
			tex_fname = MyDriver:findTextureName(tex)
			start_dir = core.extractFilePath(tex_fname)
		end						
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Corona texture from...",
			start_dir, tex_fname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedLensFlareCoronaOpenAccepted)
	elseif name == "Scened.Scene.LensFlare.Props.CoronaTexViewBtn" then
		local tex = lensflare_scene_node:getCorona()
		local tex = lensflare_scene_node:getCorona()
		if tex ~= nil then
			local tex_fname = MyDriver:findTextureName(tex)
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", tex_fname,
				_ScenedLensFlareCoronaOpenAccepted, nil, nil,
				_ScenedLensFlareCoronaClosedImage)
		end
	elseif name == "Scened.Scene.LensFlare.Props.LensAddBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Add Lens texture from...",
			MyResMgr:getMediaDirFull(res.EMT_TEXTURES), nil,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedLensFlareLensAddAccepted)
	else
		for i = 1, table.getn(_Ctrls.Tabs.Props.Lens.LensArray) do
			for key, btn in pairs(_Ctrls.Tabs.Props.Lens.LensArray[i].Buttons) do
				if btn.Ctrl:getName() == name then
					local lensIdx = _Ctrls.Tabs.Props.Lens.LensArray[i].Index
					local lens = lensflare_scene_node:getLens(lensIdx)
					local tex = lens:getTexture()
					if key == "OpenBtn" then
						local tex_fname = nil
						local start_dir = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
						if tex ~= nil then
							tex_fname = MyDriver:findTextureName(tex)
							start_dir = core.extractFilePath(tex_fname)
						end						
						Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Lens"..tostring(lensIdx).." texture from...",
							start_dir, tex_fname,
							SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedLensFlareLensOpenAccepted, lensIdx)
					elseif key == "ViewBtn" then
						if tex ~= nil then
							local tex_fname = MyDriver:findTextureName(tex)
							Helper.GUI.ImageEditor.show(0,
								IMAGE_EDITOR_FLAGS.OPEN,
								"Image Viewer", tex_fname,
								_ScenedLensFlareLensOpenAccepted, nil, nil,
								_ScenedLensFlareLensClosedImage, lensIdx)
						end
					elseif key == "RemBtn" then						
						lensflare_scene_node:removeLens(lensIdx)
						_ScenedUpdateLensFlareControls()
					end
					return
				end
			end
		end
	end
end

local function _ScenedLensFlareUpdate()
end

function _ScenedInitLensFlareControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_lensflare.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.LensFlare.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.LensFlare.TabControl"), "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for key, value in pairs(_Ctrls.Tabs) do
		value.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.LensFlare."..key..".Root")
		value.Root.Ctrl:setText(value.Caption)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Props.Root.Ctrl)

	local cegui_tex = nil
	local imageset = nil
	
	-- DiffuseImage

	_Ctrls.Tabs.Props.DiffuseImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.LensFlare.Props.DiffuseImage")

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	_Resources.Textures.DiffuseImage = cegui_tex
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	imageset = CEGUIImgsetMgr:create("Scened.Scene.LensFlare.Props.DiffuseImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0))
	
	_Ctrls.Tabs.Props.DiffuseImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.LensFlare.Props.DiffuseImage image:ColorImage")

	-- CoronaTexImage

	_Ctrls.Tabs.Props.CoronaTexImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.LensFlare.Props.CoronaTexImage")

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.CoronaTexImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.LensFlare.Props.CoronaTexImageEmpty", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.CoronaTexImageEmpty = imageset

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.CoronaTexImage = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.ParticleSystem.Emitter.Props.CoronaTexImage", cegui_tex)
	imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.CoronaTexImage = imageset

	_Ctrls.Tabs.Props.CoronaTexImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.LensFlare.Props.CoronaTexImageEmpty image:ColorImage")
	
	Helper.GUI.initButtons("Scened.Scene.LensFlare.Props.",
		_Ctrls.Tabs.Props.Buttons, _ScenedLensFlareWidgetClicked)

	_Ctrls.Tabs.Props.Lens.ScrollArea.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.LensFlare.Props.Lens.ScrollArea")

	_Ctrls.Tabs.Props.EtalonScreenWidthSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.LensFlare.Props.EtalonScreenWidthSpinner"), "CEGUI::Spinner")
	_Ctrls.Tabs.Props.EtalonScreenHeightSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.LensFlare.Props.EtalonScreenHeightSpinner"), "CEGUI::Spinner")

	_Ctrls.Tabs.Props.EtalonScreenWidthSpinner.Ctrl:subscribeEvent("ValueChanged", _ScenedLensFlareSpinnerValueChanged)
	_Ctrls.Tabs.Props.EtalonScreenHeightSpinner.Ctrl:subscribeEvent("ValueChanged", _ScenedLensFlareSpinnerValueChanged)
	
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedLensFlareUpdate,
		updateControls = _ScenedUpdateLensFlareControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end
