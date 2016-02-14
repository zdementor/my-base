
local _Ctrls =
{
	Root = {},
	TabControl = {},
	Tabs =
	{
		Tex =
		{
			Caption = "Textures",
			Root = {},
			Buttons =
			{
				AddBtn       = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"       },
				RemBtn       = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"    },
				ViewBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.image-view"     },
			},
			List = {},
			ParamsArea = {},
			TexImage = {},
			InfoLabel = {},
		},
		Anim =
		{
			Caption = "Animations",
			Root = {},
			ParamsArea = {},
			DirParamsArea = {},
			FrameParamsArea = {},
			Buttons =
			{
				AddBtn       = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"       },
				RemBtn       = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"    },
				DirAddBtn    = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"       },
				DirRemBtn    = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"    },
				FrameAddBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"       },
				FrameRemBtn  = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"    },
				PlayBtn      = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.media-playback-start" },
				PauseBtn     = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.media-playback-pause" },
			},
			List = {},
			DirList = {},
			FrameList = {},
			FrameImage = {},
			Editboxes =
			{
				FrameTCLeftEditbox = {},
				FrameTCTopEditbox = {},
				FrameTCRightEditbox = {},
				FrameTCBottomEditbox = {},
				FrameTCOffsetXEditbox = {},
				FrameTCOffsetYEditbox = {},
			},
		},
	},
}

local _Resources =
{
	Textures =
	{
		FrameImageEmpty = nil,
		FrameImage = nil,
		TexImageEmpty = nil,
		TexImage = nil,
	},
	Imagesets =
	{
		FrameImageEmpty = nil,
		FrameImage = nil,
		TexImageEmpty = nil,
		TexImage = nil,
	},
}

local _ScenedBillboardCEGUIColorWhite = CEGUI.colour(1.0, 1.0, 1.0, 1.0)
local _ScenedBillboardCEGUIColorYellow = CEGUI.colour(1.0, 1.0, 0.5, 1.0)

function _ScenedBillboardGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local bill_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_BILLBOARD_SCENE_NODE then
			bill_scene_node = tolua.cast(scene_node, "scn::IBillboardSceneNode")
		end
	end
	return bill_scene_node
end

local function _ScenedBillboardDialogUpdateCurrentImageFrame(bill_scene_node, anim_idx, dir_idx, frame_idx)

	local imageset = _Resources.Imagesets.FrameImageEmpty
	local frame = nil
	local my_tex = nil
	
	if frame_idx >= 0 then
		frame = bill_scene_node:getAnimationDirectionSetFrame(anim_idx, dir_idx, frame_idx)
		my_tex = frame.Texture
	end

	if my_tex ~= nil then

		local billparams = bill_scene_node:getBillboardParameters()
		local tleft = frame.TCoords.Left * my_tex:getSize().Width + 0.5
		local ttop = frame.TCoords.Top * my_tex:getSize().Height + 0.5
		local tright = frame.TCoords.Right * my_tex:getSize().Width + 0.5
		local tbottom = frame.TCoords.Bottom* my_tex:getSize().Height + 0.5
		local toffsx = frame.TOffset.X * my_tex:getSize().Width + 0.5
		local toffsy = frame.TOffset.Y * my_tex:getSize().Height + 0.5
		local twidth = tright - tleft
		local theight = tbottom - ttop
		
		_Ctrls.Tabs.Anim.Editboxes.FrameTCLeftEditbox.Ctrl:setText(string.format("%d", tleft))
		_Ctrls.Tabs.Anim.Editboxes.FrameTCTopEditbox.Ctrl:setText(string.format("%d",  ttop))
		_Ctrls.Tabs.Anim.Editboxes.FrameTCRightEditbox.Ctrl:setText(string.format("%d", tright))
		_Ctrls.Tabs.Anim.Editboxes.FrameTCBottomEditbox.Ctrl:setText(string.format("%d", tbottom))
		_Ctrls.Tabs.Anim.Editboxes.FrameTCOffsetXEditbox.Ctrl:setText(string.format("%d", toffsx))
		_Ctrls.Tabs.Anim.Editboxes.FrameTCOffsetYEditbox.Ctrl:setText(string.format("%d", toffsy))
		
		local cegui_tex = _Resources.Textures.FrameImage
		MyCEGUI.setTexture(cegui_tex, my_tex)
		imageset = _Resources.Imagesets.FrameImage
		imageset:undefineAllImages()
		imageset:defineImage("FrameImage",
			CEGUI.Vector2(tleft, ttop),
			CEGUI.Vector2(twidth, theight),
			CEGUI.Vector2(0, 0))
		
		local left = 0.05
		local top = 0.05
		local width = 0.9
		local height = 0.9
		left = left + width * (toffsx) / billparams.FrameResolution.Width
		top = top + height * (toffsy) / billparams.FrameResolution.Height
		width = width * twidth / billparams.FrameResolution.Width
		height = height * theight / billparams.FrameResolution.Height
		_Ctrls.Tabs.Anim.FrameImage.Ctrl:setProperty("UnifiedAreaRect",
			string.format("{{%f,0},{%f,0.0},{%f,0},{%f,0.0}}",
			left, top, left + width, top + height))
	else
		for k, v in _Ctrls.Tabs.Anim.Editboxes do
			v.Ctrl:setText("0")
		end
		_Ctrls.Tabs.Anim.FrameImage.Ctrl:setProperty("UnifiedAreaRect",
			"{{0.05,0.0},{0.05,0.0},{0.95,0.0},{0.95,0.0}}")
	end
	
	_Ctrls.Tabs.Anim.FrameImage.Ctrl:setProperty(
		"Image", "set:"..imageset:getName().." image:FrameImage")
end

local _ScenedBillboardCurrentAnimationFrameImageIdx = 0
local _ScenedBillboardCurrentAnimationFrameImageTimeMs = 0
local _ScenedBillboardTexTable = {}

local function _ScenedBillboardUpdateImageAnimation(timeMs)

	if not _Ctrls.Tabs.Anim.ParamsArea.Ctrl:isVisible()
			or _Ctrls.Tabs.Anim.FrameParamsArea.Ctrl:isVisible() then
		_ScenedBillboardCurrentAnimationFrameImageIdx = 0
		return
	end

	local bill_scene_node = _ScenedBillboardGetSelectedSceneNode()
	local anim_idx = _Ctrls.Tabs.Anim.List.Ctrl:getFirstSelectedItem():getID()
	
	local animparams = bill_scene_node:getAnimationFramesParameters(anim_idx)
	
	if _ScenedBillboardCurrentAnimationFrameImageTimeMs + animparams.TimePerFrame > timeMs then
		return
	end
	
	local dir_idx = 0
	if _Ctrls.Tabs.Anim.DirList.Ctrl:getFirstSelectedItem() ~= nil then
		dir_idx = _Ctrls.Tabs.Anim.DirList.Ctrl:getFirstSelectedItem():getID()
	end

	local frames_count = bill_scene_node:getAnimationDirectionSetFramesCount(anim_idx, dir_idx)
	local billparams = bill_scene_node:getBillboardParameters()
		
	_ScenedBillboardCurrentAnimationFrameImageTimeMs = timeMs
	
	_ScenedBillboardCurrentAnimationFrameImageIdx = _ScenedBillboardCurrentAnimationFrameImageIdx + 1
	if _ScenedBillboardCurrentAnimationFrameImageIdx >= frames_count then
		_ScenedBillboardCurrentAnimationFrameImageIdx = 0
		if _ScenedBillboardCurrentAnimationFrameImageIdx >= frames_count then
			_ScenedBillboardCurrentAnimationFrameImageIdx = -1
		end
	end

	_ScenedBillboardDialogUpdateCurrentImageFrame(
		bill_scene_node, anim_idx, dir_idx, _ScenedBillboardCurrentAnimationFrameImageIdx)

end

function _ScenedBillboardUpdate(timeMs)
	_ScenedBillboardUpdateImageAnimation(timeMs)
end

function _ScenedBillboardDialogUpdateCurrentAnimation(bill_scene_node)
	for k, v in _Ctrls.Tabs.Anim.Buttons do
		v.Ctrl:setEnabled(false)
	end
	_Ctrls.Tabs.Anim.Buttons.AddBtn.Ctrl:setEnabled(true)
	local item = _Ctrls.Tabs.Anim.List.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	if bill_scene_node:getAnimationsCount() == 0 then
		return
	end

	local anim_idx = item:getID()
	local playing = false
	if bill_scene_node:getCurrentAnimation() == anim_idx
		and not MyTimer:isSuspended() then
		playing = true
	end
	_Ctrls.Tabs.Anim.Buttons.PlayBtn.Ctrl:setEnabled(not playing)
	_Ctrls.Tabs.Anim.Buttons.PauseBtn.Ctrl:setEnabled(playing)

	_Ctrls.Tabs.Anim.Buttons.RemBtn.Ctrl:setEnabled(true)
	
	_Ctrls.Tabs.Anim.DirList.Ctrl:resetList()
	for d = 0, bill_scene_node:getAnimationDirectionSetsCount(anim_idx) - 1 do
		local name = string.format("Dir %d", d)
		Helper.GUI.addMultiColumnListItem(_Ctrls.Tabs.Anim.DirList.Ctrl,
			OPTIONS.HelperGUI.SchemeName, {
				[1] = {Name = d, ID = d, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				[2] = {Name = name, ID = d, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				})
	end
end

function _ScenedBillboardDialogUpdateCurrentDirection(bill_scene_node)
	local item = _Ctrls.Tabs.Anim.List.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	local anim_idx = item:getID()
	
	item = _Ctrls.Tabs.Anim.DirList.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.DirParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	local dir_idx = item:getID()
	
	_Ctrls.Tabs.Anim.FrameList.Ctrl:resetList()
	for d = 0, bill_scene_node:getAnimationDirectionSetFramesCount(anim_idx, dir_idx) - 1 do
		local name = string.format("Frame %d", d)
		Helper.GUI.addMultiColumnListItem(_Ctrls.Tabs.Anim.FrameList.Ctrl,
			OPTIONS.HelperGUI.SchemeName, {
				[1] = {Name = d, ID = d, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				[2] = {Name = name, ID = d, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				})
	end
end

function _ScenedBillboardDialogUpdateCurrentFrame(bill_scene_node)
	local item = _Ctrls.Tabs.Anim.List.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	local anim_idx = item:getID()

	item = _Ctrls.Tabs.Anim.DirList.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.DirParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	local dir_idx = item:getID()

	item = _Ctrls.Tabs.Anim.FrameList.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.FrameParamsArea.Ctrl:setVisible(item ~= nil)

	local frame_idx = -1
	if item ~= nil then
		frame_idx = item:getID()
	end
	
	_ScenedBillboardDialogUpdateCurrentImageFrame(bill_scene_node, anim_idx, dir_idx, frame_idx)

end

function _ScenedBillboardUpdateCurrentPlayingAnimation(bill_scene_node)
	local animation_idx = bill_scene_node:getCurrentAnimation()
	for i = 0, _Ctrls.Tabs.Anim.List.Ctrl:getRowCount() - 1 do
		for j = 0, _Ctrls.Tabs.Anim.List.Ctrl:getColumnCount() - 1 do
			local item = _Ctrls.Tabs.Anim.List.Ctrl:getItemAtGridReference(CEGUI.MCLGridRef(i, j))
			local textitem = tolua.cast(item, "CEGUI::ListboxTextItem")	
			if i == animation_idx then
				if j == 0 then
					textitem:setText(string.format("%d*", i))
				end
				textitem:setTextColours(_ScenedBillboardCEGUIColorYellow)
			else
				if j == 0 then
					textitem:setText(string.format("%d", i))
				end
				textitem:setTextColours(_ScenedBillboardCEGUIColorWhite)
			end
		end
	end
	-- to update formatted cells
	local item = _Ctrls.Tabs.Anim.List.Ctrl:getFirstSelectedItem()
	if item ~= nil then
		_Ctrls.Tabs.Anim.List.Ctrl:setItemSelectState(item, false)
		_Ctrls.Tabs.Anim.List.Ctrl:setItemSelectState(item, true)
	else
		if _Ctrls.Tabs.Anim.List.Ctrl:getRowCount() > 0
				and _Ctrls.Tabs.Anim.List.Ctrl:getColumnCount() > 0 then
			local first_item = _Ctrls.Tabs.Anim.List.Ctrl:getItemAtGridReference(CEGUI.MCLGridRef(0, 0))
			_Ctrls.Tabs.Anim.List.Ctrl:setItemSelectState(first_item, true)
			_Ctrls.Tabs.Anim.List.Ctrl:setItemSelectState(first_item, false)
		end
	end

end

function _ScenedUpdateBillboardControls()
	local bill_scene_node = _ScenedBillboardGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(bill_scene_node ~= nil)
	if bill_scene_node == nil then
		return
	end
	
	local tex_cache = {}
	local tex_table = {}
	
	local anim_cnt = bill_scene_node:getAnimationsCount()
	local tex_index = 1
	for anim_idx = 0, anim_cnt - 1 do
		local dir_cnt = bill_scene_node:getAnimationDirectionSetsCount(anim_idx)
		for dir_idx = 0, dir_cnt - 1 do
			local fr_cnt = bill_scene_node:getAnimationDirectionSetFramesCount(anim_idx, dir_idx)
			for fr_idx = 0, fr_cnt - 1 do
				local frame = bill_scene_node:getAnimationDirectionSetFrame(anim_idx, dir_idx, fr_idx)
				local tex_fname = MyDriver:findTextureName(frame.Texture)
				local tex = tex_cache[tex_fname]
				if tex == nil then
					tex_cache[tex_fname] = frame.Texture
					tex_table[tex_index] = tex_fname
					tex_index = tex_index + 1
				end
			end
		end
	end
	table.sort(tex_table)
	
	_Ctrls.Tabs.Tex.List.Ctrl:resetList()
	for t = 1, table.getn(tex_table) do
		local full_file_name = tex_table[t]
		local name = nil
		if full_file_name ~= nil then
			name = core.extractFileName(full_file_name)
		end
		if name ~= nil then
			Helper.GUI.addMultiColumnListItem(_Ctrls.Tabs.Tex.List.Ctrl,
				OPTIONS.HelperGUI.SchemeName, {
					[1] = {Name = t - 1, ID = t - 1, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
					[2] = {Name = name, ID = t - 1, nil, Color = _ScenedBillboardCEGUIColorWhite},
					})
		end
	end
	_ScenedBillboardTexTable = tex_table

	_Ctrls.Tabs.Anim.List.Ctrl:resetList()
	for a = 0, bill_scene_node:getAnimationsCount() - 1 do
		local name = string.format("Anim %d", a)
		Helper.GUI.addMultiColumnListItem(_Ctrls.Tabs.Anim.List.Ctrl,
			OPTIONS.HelperGUI.SchemeName, {
				[1] = {Name = a, ID = a, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				[2] = {Name = name, ID = a, Data = nil, Color = _ScenedBillboardCEGUIColorWhite},
				})
	end

	_ScenedBillboardDialogUpdateCurrentAnimation(bill_scene_node)
	_ScenedBillboardDialogUpdateCurrentDirection(bill_scene_node)
	_ScenedBillboardDialogUpdateCurrentFrame(bill_scene_node)
	_ScenedBillboardUpdateCurrentPlayingAnimation(bill_scene_node)
end

local function _ScenedAnimatedMeshTextAccepted(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Billboard.Anim.FrameTCLeftEditbox"
			or name == "Scened.Scene.Billboard.Anim.FrameTCTopEditbox"
			or name == "Scened.Scene.Billboard.Anim.FrameTCRightEditbox"
			or name == "Scened.Scene.Billboard.Anim.FrameTCBottomEditbox"
			or name == "Scened.Scene.Billboard.Anim.FrameTCOffsetXEditbox"
			or name == "Scened.Scene.Billboard.Anim.FrameTCOffsetYEditbox"
			then
	end
end

local function _ScenedBillboardOpenImage(full_file_name)
end

local function _ScenedBillboardReloadImage(full_file_name)
end

local function _ScenedBillboardSaveImage(full_file_name)
end

local function _ScenedBillboardClosedImage()
end
				
local function _ScenedBillboardWidgetClicked(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Billboard.Tex.AddBtn" then
	elseif name == "Scened.Scene.Billboard.Tex.RemBtn" then
	elseif name == "Scened.Scene.Billboard.Tex.ViewBtn" then
		local item = _Ctrls.Tabs.Tex.List.Ctrl:getFirstSelectedItem()
		_Ctrls.Tabs.Tex.ParamsArea.Ctrl:setVisible(item ~= nil)
		if item == nil then
			return
		end
		local tex_idx = item:getID()
		local file_name = _ScenedBillboardTexTable[tex_idx + 1]
		Helper.GUI.ImageEditor.show(0,
			bit.bor(IMAGE_EDITOR_FLAGS.OPEN, IMAGE_EDITOR_FLAGS.SAVE, IMAGE_EDITOR_FLAGS.RELOAD),
			"Image Viewer", file_name,
			_ScenedBillboardOpenImage,
			_ScenedBillboardReloadImage,
			_ScenedBillboardSaveImage,
			_ScenedBillboardClosedImage)
	end
end

local function _ScenedBillboardSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Billboard.Tex.List" then
		local item = _Ctrls.Tabs.Tex.List.Ctrl:getFirstSelectedItem()
		_Ctrls.Tabs.Tex.ParamsArea.Ctrl:setVisible(item ~= nil)
		if item == nil then
			return
		end
		local tex_idx = item:getID()
		local full_file_name = _ScenedBillboardTexTable[tex_idx + 1]

		local cegui_tex = _Resources.Textures.TexImage
		local my_tex = MyDriver:findTexture(full_file_name)
		local image = nil

		image = MyImgLib:getImage(full_file_name)
		if image ~= nil then
			if my_tex == nil then
				my_tex = MyDriver:addTexture(full_file_name, image)
			end
		end

		MyCEGUI.setTexture(cegui_tex, my_tex)

		if my_tex ~= nil then
			local imageset = _Resources.Imagesets.TexImage
			imageset:undefineAllImages()
			imageset:defineImage("TexImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0))
			_Ctrls.Tabs.Tex.TexImage.Ctrl:setProperty("Image", "set:Scened.Scene.Billboard.Tex.TexImage image:TexImage")
			
			local aspect = cegui_tex:getSize().width / cegui_tex:getSize().height

			local img_sz
			if aspect > 1 then
				img_sz = CEGUI.UVector2(
					CEGUI.UDim(1.0, 0.0),
					CEGUI.UDim(1.0 / aspect, 0.0))
			else
				img_sz = CEGUI.UVector2(
					CEGUI.UDim(1.0 * aspect, 0.0),
					CEGUI.UDim(1.0, 0.0))
			end

			local img_min = CEGUI.UVector2(
				CEGUI.UDim((1.0 - img_sz.x.scale) / 2.0 + 0.05, 0.0),
				CEGUI.UDim((1.0 - img_sz.y.scale) / 2.0 + 0.05, 0.0))
			local img_max = CEGUI.UVector2(
				CEGUI.UDim(img_min.x.scale + img_sz.x.scale - 0.1, 0.0),
				CEGUI.UDim(img_min.y.scale + img_sz.y.scale - 0.1, 0.0))
			local img_area = CEGUI.URect(img_min, img_max)

			_Ctrls.Tabs.Tex.TexImage.Ctrl:setArea(img_area)

			_Ctrls.Tabs.Tex.InfoLabel.Ctrl:setText(
				string.format("%d\nx\n%d", my_tex:getSize().Width, my_tex:getSize().Height))
		else
			_Ctrls.Tabs.Tex.TexImage.Ctrl:setProperty(
				"Image", "set:Scened.Scene.Billboard.Tex.EmptyTexImage image:TexImage")
			_Ctrls.Tabs.Tex.InfoLabel.Ctrl:setText("")
			MyLogger:logErr("Can't open image from file '"..tostring(full_file_name).."'")
		end
	elseif name == "Scened.Scene.Billboard.Anim.List" then
		local bill_scene_node = _ScenedBillboardGetSelectedSceneNode()
		_ScenedBillboardDialogUpdateCurrentAnimation(bill_scene_node)
		_ScenedBillboardDialogUpdateCurrentDirection(bill_scene_node)
		_ScenedBillboardDialogUpdateCurrentFrame(bill_scene_node)
	elseif name == "Scened.Scene.Billboard.Anim.DirList" then
		local bill_scene_node = _ScenedBillboardGetSelectedSceneNode()
		_ScenedBillboardDialogUpdateCurrentDirection(bill_scene_node)
		_ScenedBillboardDialogUpdateCurrentFrame(bill_scene_node)
	elseif name == "Scened.Scene.Billboard.Anim.FrameList" then
		local bill_scene_node = _ScenedBillboardGetSelectedSceneNode()
		_ScenedBillboardDialogUpdateCurrentFrame(bill_scene_node)
	end
end

local function _ScenedBillboardDoubleClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Billboard.Tex.List" then
	end
end

function _ScenedInitBillboardControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_billboard.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:createWindow(
		"MyScened/TabControl", "Scened.Scene.BillboardTabControl")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0.02,0},{0.02,0},{0.98,0},{0.98,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(_Ctrls.Root.Ctrl, "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for key, value in _Ctrls.Tabs do
		value.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Billboard."..key..".Root")
		value.Root.Ctrl:setText(value.Caption)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Tex.Root.Ctrl)
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Anim.Root.Ctrl)

	Helper.GUI.initButtons("Scened.Scene.Billboard.Tex.", _Ctrls.Tabs.Tex.Buttons,
		_ScenedBillboardWidgetClicked)

	_Ctrls.Tabs.Tex.List.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Tex.List"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.Tabs.Tex.List.Ctrl, 0, {
			[1] = {Name = "#", Dim = CEGUI.UDim(0.15, 0)},
			[2] = {Name = "File Name", Dim = CEGUI.UDim(0.70, 0)},
			})
	_Ctrls.Tabs.Tex.List.Ctrl:subscribeEvent("SelectionChanged", _ScenedBillboardSelectionChanged)
	_Ctrls.Tabs.Tex.List.Ctrl:subscribeEvent("MouseDoubleClick", _ScenedBillboardDoubleClick)
	
	_Ctrls.Tabs.Tex.ParamsArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Tex.ParamsArea")
	_Ctrls.Tabs.Tex.ParamsArea.Ctrl:hide()
	
	_Ctrls.Tabs.Tex.TexImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Tex.Image")
	
	Helper.GUI.initButtons("Scened.Scene.Billboard.Anim.", _Ctrls.Tabs.Anim.Buttons,
		_ScenedBillboardWidgetClicked)

	_Ctrls.Tabs.Anim.List.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.List"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.Tabs.Anim.List.Ctrl, 0, {
			[1] = {Name = "#", Dim = CEGUI.UDim(0.15, 0)},
			[2] = {Name = "Animation", Dim = CEGUI.UDim(0.70, 0)},
			})
	_Ctrls.Tabs.Anim.List.Ctrl:subscribeEvent("SelectionChanged", _ScenedBillboardSelectionChanged)
	_Ctrls.Tabs.Anim.List.Ctrl:subscribeEvent("MouseDoubleClick", _ScenedBillboardDoubleClick)

	_Ctrls.Tabs.Anim.DirList.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.DirList"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.Tabs.Anim.DirList.Ctrl, 0, {
			[1] = {Name = "#", Dim = CEGUI.UDim(0.20, 0)},
			[2] = {Name = "Direction", Dim = CEGUI.UDim(0.60, 0)},
			})
	_Ctrls.Tabs.Anim.DirList.Ctrl:subscribeEvent("SelectionChanged", _ScenedBillboardSelectionChanged)
	_Ctrls.Tabs.Anim.DirList.Ctrl:subscribeEvent("MouseDoubleClick", _ScenedBillboardDoubleClick)

	_Ctrls.Tabs.Anim.FrameList.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.FrameList"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.Tabs.Anim.FrameList.Ctrl, 0, {
			[1] = {Name = "#", Dim = CEGUI.UDim(0.25, 0)},
			[2] = {Name = "Frame", Dim = CEGUI.UDim(0.55, 0)},
			})
	_Ctrls.Tabs.Anim.FrameList.Ctrl:subscribeEvent("SelectionChanged", _ScenedBillboardSelectionChanged)
	_Ctrls.Tabs.Anim.FrameList.Ctrl:subscribeEvent("MouseDoubleClick", _ScenedBillboardDoubleClick)

	_Ctrls.Tabs.Anim.ParamsArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.ParamsArea")
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl:hide()

	_Ctrls.Tabs.Anim.DirParamsArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.DirParamsArea")
	_Ctrls.Tabs.Anim.DirParamsArea.Ctrl:hide()

	_Ctrls.Tabs.Anim.FrameParamsArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.FrameParamsArea")
	_Ctrls.Tabs.Anim.FrameParamsArea.Ctrl:hide()
	
	for k, v in _Ctrls.Tabs.Anim.Editboxes do
		v.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
			"Scened.Scene.Billboard.Anim."..k), "CEGUI::Editbox")
		v.Ctrl:subscribeEvent("TextAccepted", _ScenedBillboardTextAccepted)
	end

	_Ctrls.Tabs.Anim.FrameImage.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Anim.FrameImage")

	local cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.FrameImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.Billboard.Anim.FrameImageEmpty", cegui_tex)
	imageset:defineImage("FrameImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.FrameImageEmpty = imageset
	
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.FrameImage = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.Billboard.Anim.FrameImage", cegui_tex)
	imageset:defineImage("FrameImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.FrameImage = imageset

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.TexImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.Billboard.Tex.TexImageEmpty", cegui_tex)
	imageset:defineImage("TexImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.TexImageEmpty = imageset

	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.TexImage = cegui_tex
	imageset = CEGUIImgsetMgr:create("Scened.Scene.Billboard.Tex.TexImage", cegui_tex)
	imageset:defineImage("TexImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.TexImage = imageset
	
	_Ctrls.Tabs.Anim.FrameImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.Billboard.Anim.FrameImageEmpty image:FrameImage")
	
	_Ctrls.Tabs.Tex.TexImage.Ctrl:setProperty(
		"Image", "set:Scened.Scene.Billboard.Tex.TexImageEmpty image:TexImage")

	_Ctrls.Tabs.Tex.InfoLabel.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.Billboard.Tex.InfoLabel")
	
	_Ctrls.Tabs.Tex.InfoLabel.Ctrl:setText("")
		
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedBillboardUpdate,
		updateControls = _ScenedUpdateBillboardControls,
		onSelectSceneNode = nil,
		onDeselectSceneNode = nil,}
end