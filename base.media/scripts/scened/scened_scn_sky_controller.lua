
local _Ctrls =
{
	Root = {},
	TabControl = {},
	Tabs =
	{
		Sides =
		{
			Caption = "Sides",
			Root = {},
			Buttons =
			{
				TopTexOpenBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				TopTexViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				TopTexClearBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				BottomTexOpenBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				BottomTexViewBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				BottomTexClearBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				LeftTexOpenBtn    = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				LeftTexViewBtn    = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				LeftTexClearBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				RightTexOpenBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				RightTexViewBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				RightTexClearBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				FrontTexOpenBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				FrontTexViewBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				FrontTexClearBtn = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
				BackTexOpenBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open" },
				BackTexViewBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"    },
				BackTexClearBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"    },
			},
			Images =
			{
				TopTexImage    = {Ctrl = nil},
				BottomTexImage = {Ctrl = nil},
				LeftTexImage  = {Ctrl = nil},
				RightTexImage = {Ctrl = nil},
				FrontTexImage  = {Ctrl = nil},
				BackTexImage   = {Ctrl = nil},
			},
			ScrollRoot = {Ctrl = nil},
		},
	}
}

local function _ScenedSkyGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local typed_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_SKY_BOX_SCENE_NODE then
			typed_scene_node = tolua.cast(scene_node, "scn::ISkyBoxSceneNode")
		end
	end
	return typed_scene_node
end

local function _ScenedSkyUpdateControls()

	local sky_scene_node = _ScenedSkyGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(sky_scene_node ~= nil)
	if sky_scene_node == nil then
		return
	end

	local img_tex = {
		[1] = { tex = sky_scene_node:getTopSideTexture(),    name_pfx = "TopTex",    },
		[2] = { tex = sky_scene_node:getBottomSideTexture(), name_pfx = "BottomTex", },
		[3] = { tex = sky_scene_node:getLeftSideTexture(),   name_pfx = "LeftTex",   },
		[4] = { tex = sky_scene_node:getRightSideTexture(),  name_pfx = "RightTex",  },
		[5] = { tex = sky_scene_node:getFrontSideTexture(),  name_pfx = "FrontTex",  },
		[6] = { tex = sky_scene_node:getBackSideTexture(),   name_pfx = "BackTex",   },
	}
	for k, v in pairs(img_tex) do
		local tex = v.tex
		local fullfname = MyDriver:findTextureName(tex)
		local image = _Ctrls.Tabs.Sides.Images[v.name_pfx.."Image"]
		Helper.GUI.setImageTexture(image, tex)
		image.Ctrl:setAlpha(1.0)
		if string.len(fullfname) > 0 and string.sub(fullfname, 0, 1) == "#" then
			tex = nil
		end
		_Ctrls.Tabs.Sides.Buttons[v.name_pfx.."OpenBtn"].Ctrl:setEnabled(true)
		_Ctrls.Tabs.Sides.Buttons[v.name_pfx.."ViewBtn"].Ctrl:setEnabled(tex ~= nil)
		_Ctrls.Tabs.Sides.Buttons[v.name_pfx.."ClearBtn"].Ctrl:setEnabled(tex ~= nil)
	end
end

function _ScenedSkyTextureOpenAccepted(full_file_name, rel_full_file_name, user_data)
	local texi = tonumber(user_data)
	local sky_scene_node = _ScenedSkyGetSelectedSceneNode()
	local tex = nil
	if full_file_name ~= nil then
		tex = MyDriver:getTexture(full_file_name)
	end
	local img_tex = {
		[1] = { tex = sky_scene_node:getTopSideTexture(),    name_pfx = "TopTex",    },
		[2] = { tex = sky_scene_node:getBottomSideTexture(), name_pfx = "BottomTex", },
		[3] = { tex = sky_scene_node:getLeftSideTexture(),   name_pfx = "LeftTex",   },
		[4] = { tex = sky_scene_node:getRightSideTexture(),  name_pfx = "RightTex",  },
		[5] = { tex = sky_scene_node:getFrontSideTexture(),  name_pfx = "FrontTex",  },
		[6] = { tex = sky_scene_node:getBackSideTexture(),   name_pfx = "BackTex",   },
	}
	img_tex[texi].tex = tex
	sky_scene_node:setSides(
		img_tex[1].tex,
		img_tex[2].tex,
		img_tex[3].tex,
		img_tex[4].tex,
		img_tex[5].tex,
		img_tex[6].tex)
	_ScenedSkyUpdateControls(sky_scene_node)
end

function _ScenedSkyTextureClearAccepted(user_data)
	_ScenedSkyTextureOpenAccepted(nil, nil, user_data)
end

function _ScenedSkyWidgetClicked(args)
	local sky_scene_node = _ScenedSkyGetSelectedSceneNode()
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()

	local img_tex = {
		[1] = { tex = sky_scene_node:getTopSideTexture(),    name_pfx = "TopTex",    },
		[2] = { tex = sky_scene_node:getBottomSideTexture(), name_pfx = "BottomTex", },
		[3] = { tex = sky_scene_node:getLeftSideTexture(),   name_pfx = "LeftTex",   },
		[4] = { tex = sky_scene_node:getRightSideTexture(),  name_pfx = "RightTex",  },
		[5] = { tex = sky_scene_node:getFrontSideTexture(),  name_pfx = "FrontTex",  },
		[6] = { tex = sky_scene_node:getBackSideTexture(),   name_pfx = "BackTex",   },
	}

	for i = 1, table.getn(img_tex) do
		local img_pfx = img_tex[i].name_pfx
		local tex = img_tex[i].tex
		if name == _Ctrls.Tabs.Sides.Buttons[img_pfx.."OpenBtn"].Ctrl:getName() then
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
			Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open texture...",
				fpath, fullfname,
				SCENED_TEXTURE_OPEN_FILE_FILTER, _ScenedSkyTextureOpenAccepted, i)
			return
		elseif name == _Ctrls.Tabs.Sides.Buttons[img_pfx.."ViewBtn"].Ctrl:getName() then
			if tex ~= nil then
				local fullfname = MyDriver:findTextureName(tex)
				Helper.GUI.ImageEditor.show(0,
					IMAGE_EDITOR_FLAGS.OPEN,
					"Image Viewer", fullfname,
					_ScenedSkyTextureOpenAccepted, nil, nil, nil, i)
			end
			return
		elseif name == _Ctrls.Tabs.Sides.Buttons[img_pfx.."ClearBtn"].Ctrl:getName() then
			_ScenedSkyTextureClearAccepted(i)
			return
		end
	end

end

local function _ScenedSkyUpdate()
end
	
function _ScenedInitSkyControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_sky.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Sky.Root")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(CEGUIWinMgr:getWindow("Scened.Scene.Sky.TabControl"), "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for k, v in pairs(_Ctrls.Tabs) do
		local name_pfx = "Scened.Scene.Sky."..k.."."
		v.Root.Ctrl = CEGUIWinMgr:getWindow(name_pfx.."Root")
		v.Root.Ctrl:setText(v.Caption)
		v.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
		Helper.GUI.initButtons(name_pfx, v.Buttons, _ScenedSkyWidgetClicked)
		Helper.GUI.initImages(name_pfx, v.Images, img.SColor(255, 0, 0, 0))
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Sides.Root.Ctrl)

	_Ctrls.Tabs.Sides.ScrollRoot.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Sky.Sides.ScrollRoot")
	
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedSkyUpdate,
		updateControls = _ScenedSkyUpdateControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end
