
local _Ctrls =
{
	Root = {},
	TabControl = {},
	Tabs =
	{
		TileSets =
		{
			Caption = "Tile sets",
			Root = {},
			Buttons =
			{
				Tex0OpenBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				Tex0ViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				Tex0ClearBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				Tex1OpenBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				Tex1ViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				Tex1ClearBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				Tex2OpenBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				Tex2ViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				Tex2ClearBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				SplatMapOpenBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				SplatMapViewBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				AddBtn           = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"      },
				RemBtn           = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"   },
			},
			Images =
			{
				Tex0Image     = {Ctrl = nil},
				Tex1Image     = {Ctrl = nil},
				Tex2Image     = {Ctrl = nil},
				SplatMapImage = {Ctrl = nil},
			},
			Spinners = {},
			Editboxes = {},
			Listbox = {Ctrl = nil},
			ScrollRoot = {Ctrl = nil},
		},
		Props =
		{
			Caption = "Properties",
			Root = {},
			Buttons =
			{
				HeightMapOpenBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				HeightMapViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				HeightMapClearBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
			},
			Images =
			{
				HeightMapImage = {Ctrl = nil},
			},
			Spinners =
			{
				TileRepeatNumberSpinner = {Ctrl = nil},
			},
			Editboxes =
			{
				HeightScaleEditbox      = {Ctrl = nil},
				GridPointSpacingEditbox = {Ctrl = nil},
			},
		},
	}
}

local function _ScenedTerrainGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local terrain_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_TERRAIN_SCENE_NODE then
			terrain_scene_node = tolua.cast(scene_node, "scn::ITerrainSceneNode")
		end
	end
	return terrain_scene_node
end

local function _ScenedTerrainGetSelectedTileSetIndex(terrain_scene_node)
	local listbox = _Ctrls.Tabs.TileSets.Listbox.Ctrl
	local item = listbox:getFirstSelectedItem()
	if item == nil then
		return nil
	end
	local idx = listbox:getItemIndex(item)
	if idx >= terrain_scene_node:getTileSetsCount() then
		return nil
	end
	return idx
end

local function _ScenedTerrainGetSelectedTileSet(terrain_scene_node)
	local idx = _ScenedTerrainGetSelectedTileSetIndex(terrain_scene_node)
	if idx == nil then
		return nil
	end
	return terrain_scene_node:getTileSet(idx)
end

local function _ScenedTerrainUpdateCurrentTileSetControls(terrain_scene_node)
	local tile = _ScenedTerrainGetSelectedTileSet(terrain_scene_node)
	local idx = _ScenedTerrainGetSelectedTileSetIndex(terrain_scene_node)
	
	_Ctrls.Tabs.TileSets.Buttons.RemBtn.Ctrl:setEnabled(idx ~= nil)

	_Ctrls.Tabs.TileSets.ScrollRoot.Ctrl:setVisible(tile ~= nil)
	if tile == nil then
		for keyi, valuei in pairs(_Ctrls.Tabs.TileSets.Images) do
			Helper.GUI.setImageTexture(valuei, nil)
		end
		return
	end
	Helper.GUI.setImageTexture( _Ctrls.Tabs.TileSets.Images.Tex0Image, tile:getTile0Texture())
	Helper.GUI.setImageTexture( _Ctrls.Tabs.TileSets.Images.Tex1Image, tile:getTile1Texture())
	Helper.GUI.setImageTexture( _Ctrls.Tabs.TileSets.Images.Tex2Image, tile:getTile2Texture())
	local splatmap_file_name = tile:getSplatMapFileName()
	local tex = MyDriver:getTexture(splatmap_file_name)
	Helper.GUI.setImageTexture( _Ctrls.Tabs.TileSets.Images.SplatMapImage, tex)

	_Ctrls.Tabs.TileSets.Buttons.Tex0ViewBtn.Ctrl:setEnabled(tile:getTile0Texture() ~= nil)
	_Ctrls.Tabs.TileSets.Buttons.Tex0ClearBtn.Ctrl:setEnabled(tile:getTile0Texture() ~= nil)
	_Ctrls.Tabs.TileSets.Buttons.Tex1ViewBtn.Ctrl:setEnabled(tile:getTile1Texture() ~= nil)
	_Ctrls.Tabs.TileSets.Buttons.Tex1ClearBtn.Ctrl:setEnabled(tile:getTile1Texture() ~= nil)
	_Ctrls.Tabs.TileSets.Buttons.Tex2ViewBtn.Ctrl:setEnabled(tile:getTile2Texture() ~= nil)
	_Ctrls.Tabs.TileSets.Buttons.Tex2ClearBtn.Ctrl:setEnabled(tile:getTile2Texture() ~= nil)
				
end

local function _ScenedTerrainUpdateControls()
	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(terrain_scene_node ~= nil)
	if terrain_scene_node == nil then
		for key, value in pairs(_Ctrls.Tabs) do
			for keyi, valuei in pairs(value.Images) do
				Helper.GUI.setImageTexture(valuei, nil)
			end
		end
		return
	end

	local hmap_tex = terrain_scene_node:getHeightMapTexture()
	Helper.GUI.setImageTexture(_Ctrls.Tabs.Props.Images.HeightMapImage, hmap_tex)

	_Ctrls.Tabs.Props.Spinners.TileRepeatNumberSpinner.Ctrl:setCurrentValue(terrain_scene_node:getTileRepeatNumber())
	
	_Ctrls.Tabs.Props.Editboxes.GridPointSpacingEditbox.Ctrl:setText(string.format("%.2f", terrain_scene_node:getGridPointSpacing()))
	_Ctrls.Tabs.Props.Editboxes.HeightScaleEditbox.Ctrl:setText(string.format("%.4f", terrain_scene_node:getHeightScale()))

	local listbox = _Ctrls.Tabs.TileSets.Listbox.Ctrl
	listbox:resetList()
	local tilesCount = terrain_scene_node:getTileSetsCount()
	for i = 0, tilesCount - 1 do
		Helper.GUI.addListboxItem(listbox, OPTIONS.ScenedGUI.SchemeName,
			"Tile set "..i, i, nil)
	end
	if listbox:getItemCount() > 0 then
		listbox:setItemSelectState(0, true)
	else
		_ScenedTerrainUpdateCurrentTileSetControls(terrain_scene_node)
	end

end

function _ScenedTerrainAddTileSetAccepted(full_file_name)

	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()

	local tile = terrain_scene_node:getTileSet(idx)
	local tile0tex_file_name = "#DefaultTexture"
	local tile1tex_file_name = nil
	local tile2tex_file_name = nil
	local splatmap_file_name = full_file_name
	
	terrain_scene_node:addTileSet(
		tile0tex_file_name,
		tile1tex_file_name,
		tile2tex_file_name,
		splatmap_file_name)
		
	_ScenedTerrainUpdateControls()
end

function _ScenedTerrainTextureOpenAccepted(full_file_name, rel_full_file_name, user_data)

	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
	local idx = _ScenedTerrainGetSelectedTileSetIndex(terrain_scene_node)
	if idx == nil then
		return
	end
	
	local tile = terrain_scene_node:getTileSet(idx)
	local tile0tex_file_name = MyDriver:findTextureName(tile:getTile0Texture())
	local tile1tex_file_name = MyDriver:findTextureName(tile:getTile1Texture())
	local tile2tex_file_name = MyDriver:findTextureName(tile:getTile2Texture())
	local splatmap_file_name = tile:getSplatMapFileName()
	
	if user_data == 0 then
		tile0tex_file_name = full_file_name
	elseif user_data == 1 then
		tile1tex_file_name = full_file_name
	elseif user_data == 2 then
		tile2tex_file_name = full_file_name
	elseif user_data == 3 then
		splatmap_file_name = full_file_name
	else
		return
	end
	
	terrain_scene_node:setTileSet(idx,
		tile0tex_file_name,
		tile1tex_file_name,
		tile2tex_file_name,
		splatmap_file_name)
		
	_ScenedTerrainUpdateCurrentTileSetControls(terrain_scene_node)
end

function _ScenedTerrainTextureClearAccepted(user_data)
	_ScenedTerrainTextureOpenAccepted(nil, nil, user_data)
end

local function _ScenedTerrainHeightmapOpenAccepted(full_file_name, rel_full_file_name, user_data)
	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()

	terrain_scene_node:setMaps(
		full_file_name, nil,
		terrain_scene_node:getGridPointSpacing(), terrain_scene_node:getHeightScale())

	_ScenedTerrainUpdateControls()
end

local function _ScenedTerrainHeightmapClearAccepted()
	_ScenedTerrainHeightmapOpenAccepted(nil, nil, user_data)
end

function _ScenedTerrainWidgetClicked(args)
	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
	local tile = _ScenedTerrainGetSelectedTileSet(terrain_scene_node)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Tabs.TileSets.Buttons.AddBtn.Ctrl:getName() then
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open splatting map...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainAddTileSetAccepted)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.RemBtn.Ctrl:getName() then
		local idx = _ScenedTerrainGetSelectedTileSetIndex(terrain_scene_node)
		if idx ~= nil then
			terrain_scene_node:removeTileSet(idx)
			 _ScenedTerrainUpdateControls()
		end
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex0OpenBtn.Ctrl:getName() then
		local tex = tile:getTile0Texture()
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if tex ~= nil then
			fullfname = MyDriver:findTextureName(tex)
			if string.len(fullfname) > 0 and string.sub(fullfname, 0, 1) == "#" then
				fullfname = nil
			else
				fpath = core.extractFilePath(fullfname)
			end
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture 0...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainTextureOpenAccepted, 0)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex0ViewBtn.Ctrl:getName() then
		local tex = tile:getTile0Texture()
		if tex ~= nil then
			local fullfname = MyDriver:findTextureName(tex)
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", fullfname,
				_ScenedTerrainTextureOpenAccepted, nil, nil, nil, 0)
		end
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex0ClearBtn.Ctrl:getName() then
		_ScenedTerrainTextureClearAccepted(0)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex1OpenBtn.Ctrl:getName() then
		local tex = tile:getTile1Texture()
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if tex ~= nil then
			fullfname = MyDriver:findTextureName(tex)
			if string.len(fullfname) > 0 and string.sub(fullfname, 0, 1) == "#" then
				fullfname = nil
			else
				fpath = core.extractFilePath(fullfname)
			end
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture 1...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainTextureOpenAccepted, 1)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex1ViewBtn.Ctrl:getName() then
		local tex = tile:getTile1Texture()
		if tex ~= nil then
			local fullfname = MyDriver:findTextureName(tex)
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", fullfname,
				_ScenedTerrainTextureOpenAccepted, nil, nil, nil, 1)
		end
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex1ClearBtn.Ctrl:getName() then
		_ScenedTerrainTextureClearAccepted(1)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex2OpenBtn.Ctrl:getName() then
		local tex = tile:getTile2Texture()
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if tex ~= nil then
			fullfname = MyDriver:findTextureName(tex)
			if string.len(fullfname) > 0 and string.sub(fullfname, 0, 1) == "#" then
				fullfname = nil
			else
				fpath = core.extractFilePath(fullfname)
			end
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture 2...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainTextureOpenAccepted, 2)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex2ViewBtn.Ctrl:getName() then
		local tex = tile:getTile2Texture()
		if tex ~= nil then
			local fullfname = MyDriver:findTextureName(tex)
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", fullfname,
				_ScenedTerrainTextureOpenAccepted, nil, nil, nil, 2)
		end
	elseif name == _Ctrls.Tabs.TileSets.Buttons.Tex2ClearBtn.Ctrl:getName() then
		_ScenedTerrainTextureClearAccepted(2)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.SplatMapOpenBtn.Ctrl:getName() then
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if MyFileSys:existFile(tile:getSplatMapFileName()) then
			fullfname = tile:getSplatMapFileName()
			fpath = core.extractFilePath(fullfname)
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open splatting map...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainTextureOpenAccepted, 3)
	elseif name == _Ctrls.Tabs.TileSets.Buttons.SplatMapViewBtn.Ctrl:getName() then
		if MyFileSys:existFile(tile:getSplatMapFileName()) then
			local fullfname = tile:getSplatMapFileName()
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", fullfname,
				_ScenedTerrainTextureOpenAccepted, nil, nil, nil, 3)
		end
	elseif name == _Ctrls.Tabs.Props.Buttons.HeightMapOpenBtn.Ctrl:getName() then
		local fullfname = nil
		local fpath = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if MyFileSys:existFile(terrain_scene_node:getHeightMapFileName()) then
			fullfname = terrain_scene_node:getHeightMapFileName()
			fpath = core.extractFilePath(fullfname)
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open height map...",
			fpath, fullfname,
			SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedTerrainHeightmapOpenAccepted, nil)
	elseif name == _Ctrls.Tabs.Props.Buttons.HeightMapViewBtn.Ctrl:getName() then
		if MyFileSys:existFile(terrain_scene_node:getHeightMapFileName()) then
			local fullfname = terrain_scene_node:getHeightMapFileName()
			Helper.GUI.ImageEditor.show(0,
				IMAGE_EDITOR_FLAGS.OPEN,
				"Image Viewer", fullfname,
				_ScenedTerrainHeightmapOpenAccepted, nil, nil, nil, nil)
		end
	elseif name == _Ctrls.Tabs.Props.Buttons.HeightMapClearBtn.Ctrl:getName() then
		_ScenedTerrainHeightmapClearAccepted()
	else
	end
end

function _ScenedTerrainItemSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Tabs.TileSets.Listbox.Ctrl:getName() then
		local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
		_ScenedTerrainUpdateCurrentTileSetControls(terrain_scene_node)
	end
end

function _ScenedTerrainSpinnerValueChanged(args)
	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Tabs.Props.Spinners.TileRepeatNumberSpinner.Ctrl:getName() then
		local val = _Ctrls.Tabs.Props.Spinners.TileRepeatNumberSpinner.Ctrl:getCurrentValue()
		terrain_scene_node:setTileRepeatNumber(val)
	end
end

function _ScenedTerrainTextAccepted(args)
	local terrain_scene_node = _ScenedTerrainGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Tabs.Props.Editboxes.HeightScaleEditbox.Ctrl:getName() then
		local val = tonumber(_Ctrls.Tabs.Props.Editboxes.HeightScaleEditbox.Ctrl:getText())
		terrain_scene_node:setHeightScale(val)		
		_ScenedTerrainUpdateControls()
	elseif name == _Ctrls.Tabs.Props.Editboxes.GridPointSpacingEditbox.Ctrl:getName() then
		local val = tonumber(_Ctrls.Tabs.Props.Editboxes.GridPointSpacingEditbox.Ctrl:getText())
		if val < 1.0 then
			val = 1.0
		elseif val > 1280.0 then
			val = 1280.0
		end
		terrain_scene_node:setMaps(terrain_scene_node:getHeightMapFileName(), nil, val)
		_ScenedTerrainUpdateControls()
	end
end

local function _ScenedTerrainUpdate()
end
	
function _ScenedInitTerrainControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_terrain.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Terrain.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Terrain.TabControl"), "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for key, value in pairs(_Ctrls.Tabs) do
		local name_pfx = "Scened.Scene.Terrain."..key.."."
		value.Root.Ctrl = CEGUIWinMgr:getWindow(name_pfx.."Root")
		value.Root.Ctrl:setText(value.Caption)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
		Helper.GUI.initButtons(name_pfx, value.Buttons, _ScenedTerrainWidgetClicked)
		Helper.GUI.initImages(name_pfx, value.Images, img.SColor(255, 0, 0, 0))
		for skey, svalue in pairs(value.Spinners) do
			svalue.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Terrain."..key.."."..skey), "CEGUI::Spinner")
			svalue.Ctrl:subscribeEvent("ValueChanged", _ScenedTerrainSpinnerValueChanged)
		end
		for ekey, evalue in pairs(value.Editboxes) do
			evalue.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Terrain."..key.."."..ekey), "CEGUI::Editbox")
			evalue.Ctrl:subscribeEvent("TextAccepted", _ScenedTerrainTextAccepted)
		end
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Props.Root.Ctrl)
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.TileSets.Root.Ctrl)

	_Ctrls.Tabs.TileSets.Listbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Terrain.TileSets.Listbox"), "CEGUI::Listbox")
	_Ctrls.Tabs.TileSets.Listbox.Ctrl:subscribeEvent(
		"ItemSelectionChanged", _ScenedTerrainItemSelectionChanged)
	
	_Ctrls.Tabs.TileSets.ScrollRoot.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Terrain.TileSets.ScrollRoot")
	
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedTerrainUpdate,
		updateControls = _ScenedTerrainUpdateControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end
