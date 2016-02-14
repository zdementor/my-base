
local _Ctrls =
{
	Root = {},
	TabControl = {},
	Animation =
	{
		Root = {},
		Frame = {},
		Buttons =
		{
			OKBtn		= {Ctrl = nil, ImageName = nil },
			CancelBtn	= {Ctrl = nil, ImageName = nil },
		},
		BeginFrameSpinner = {},
		EndFrameSpinner = {},
	},		
	Tabs =
	{
		Mesh =
		{
			Caption = "Mesh",
			Root = {},
			Buttons =
			{
				LOD0OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
				LOD1OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
				LOD2OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"	},
				LOD0ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
				LOD1ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
				LOD2ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"		},
			},
			LOD0Editbox = {},
			LOD1Editbox = {},
			LOD2Editbox = {},
		},
		Anim =
		{
			Caption = "Anim.",
			Root = {},
			ParamsArea = {},
			Buttons =
			{
				AnimAddBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-add"		},
				AnimRemBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.list-remove"	},
				AnimMoveUpBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-up"			},
				AnimMoveDownBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-down"		},
				PlayBtn			= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.media-playback-start" },
				PauseBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.media-playback-pause" },
			},
			AnimList = {},
			LoopedCheck = {},
			SpeedEditbox = {},
			SmoothCheck = {},
			OveralSpeedEditbox = {},
		},
		Imposter =
		{
			Caption = "Imposter",
			Root = {},
			Buttons =
			{
				OpenBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-open"    },
				SaveBtn		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.document-save-as" },
				ClearBtn	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-clear"       },
				ApplyBtn    = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-apply"       },
				CancelBtn   = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.edit-undo"        },
				RecreateBtn = {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh"     },
				DiffuseBtn  = {Ctrl = nil, ImageName = nil },
				AmbientBtn  = {Ctrl = nil, ImageName = nil },
				SpecularBtn = {Ctrl = nil, ImageName = nil },
			},
			ParamsArea = {},
			EnabledCheckbox = {},
			CenteredCheckbox = {},
			FileNameEditbox = {},
			ShiftXEditbox = {},
			ShiftYEditbox = {},
			ShiftZEditbox = {},
			ViewOffsetEditbox = {},
			CompressedCheckbox = {},
			ViewsCountSpinner = {},
			OptimalViewAngleEditbox = {},
			SizeScaleWidthEditbox = {},
			SizeScaleHeightEditbox = {},
			ViewScaleWidthEditbox = {},
			ViewScaleHeightEditbox = {},
			TextureResolutionEditbox = {},
			FrameStepSpinner = {},
			ColorImages =
			{
				ImposterDiffuseImage = {},
				ImposterAmbientImage = {},
				ImposterSpecularImage = {},
			},
		},
		Shadow =
		{
			Caption = "Shadow",
			Root = {},
		},
	},
}

local _Resources =
{
	Textures =
	{
		ColorImages =
		{
			ImposterDiffuseImage = nil,
			ImposterAmbientImage = nil,
			ImposterSpecularImage = nil,
		},
	},
}

local _ColorTbl =
{
	ImposterDiffuseImage = img.SColor(),
	ImposterAmbientImage = img.SColor(),
	ImposterSpecularImage = img.SColor(),
}

local _TmpColor = img.SColor()

local _ScenedAnimationMd2Handler = nil
local _ScenedAnimationMd2StartFrameInit = 0
local _ScenedAnimationMd2EndFrameInit = 0
local _ScenedAnimationCreateMode = false

local _ScenedAnimatedMeshImposterParameters = scn.SImposterParameters()
local _ScenedAnimatedMeshImposterCreationParameters = scn.SImposterCreationParameters()

local function _ScenedAnimatedMeshColorUpdate()
	_ColorTbl.ImposterDiffuseImage:set(_ScenedAnimatedMeshImposterCreationParameters.DiffuseColor)
	_ColorTbl.ImposterAmbientImage:set(_ScenedAnimatedMeshImposterCreationParameters.AmbientColor)
	_ColorTbl.ImposterSpecularImage:set(_ScenedAnimatedMeshImposterCreationParameters.SpecularColor)
	for key, value in pairs(_Resources.Textures.ColorImages) do
		if _ColorTbl[key] ~= nil then
			Helper.GUI.fillCEGUITextureWithColor(_Resources.Textures.ColorImages[key], _ColorTbl[key])
		else
			MyLogger:logInfo(string.format("Incorrect ColorImage in Scene Animated Mesh (key='%s')", key))
		end
	end
end

function _ScenedAnimationCal3dReset(full_file_name)
	local list = _Ctrls.Tabs.Anim.AnimList.Ctrl
	local item = list:getFirstSelectedItem()
	if item == nil then
		return
	end
	local index = item:getID()
	MyLogger:logInfo(string.format("Reset Cal3d animation %s", full_file_name))
	_ScenedAnimatedMeshUpdateControls()
end

function _ScenedAnimationCal3dNew(full_file_name)
	local amesh_scene_node =
		_ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	scn.IAnimatedMesh:startRegisteringAnimations(amesh_scene_node)
	scn.IAnimatedMesh:registerAnimation(amesh_scene_node, full_file_name, 1.0, true)
	scn.IAnimatedMesh:endRegisteringAnimations(amesh_scene_node)
	_ScenedAnimatedMeshUpdateControls()
end

function _ScenedAnimationMd2Reset(begin_frame, end_frame)
	local amesh_scene_node =
		_ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	local list = _Ctrls.Tabs.Anim.AnimList.Ctrl
	local item = list:getFirstSelectedItem()
	if item == nil then
		return
	end
	local index = item:getID()
	amesh_scene_node:setAnimationBeginFrame(index, begin_frame)
	amesh_scene_node:setAnimationEndFrame(index, end_frame)
end

function _ScenedAnimationMd2New(begin_frame, end_frame)
	local amesh_scene_node =
		_ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	scn.IAnimatedMesh:startRegisteringAnimations(amesh_scene_node)
	scn.IAnimatedMesh:registerAnimation(amesh_scene_node, begin_frame, end_frame, 1.0, true)
	scn.IAnimatedMesh:endRegisteringAnimations(amesh_scene_node)
	_ScenedAnimatedMeshUpdateControls()
end

function _ScenedAnimationDialogShow(create_mode)
	local amesh_scene_node =
		_ScenedAnimatedMeshGetSelectedSceneNode()
	_ScenedAnimationCreateMode = create_mode
	if create_mode then
		for key, value in pairs({
			{lod=scn.ELL_LOD_NEAREST},
			{lod=scn.ELL_LOD_NEAR},
			{lod=scn.ELL_LOD_FAR},
			}) do
			local mesh = amesh_scene_node:getAnimatedMesh(value.lod)
			if mesh ~= nil then
				local full_file_name = MyScnMgr:findMeshFullFileName(mesh)
				local file_path = core.extractFilePath(full_file_name)
				local mesh_type = mesh:getAnimatedMeshType()
				if file_path ~= nil and mesh_type == scn.EMT_CAL3D then
					Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Cal3d animation...",
						file_path, nil,
						{
							[1]     = { Descr = "Cal3d animation files",  Value = "caf" },
							["caf"] = { Descr = "Cal3d animation files",  Value = "caf" },
						},
						_ScenedAnimationCal3dNew)
				elseif mesh_type == scn.EMT_MD2 then
					_Ctrls.Animation.Root.Ctrl:setVisible(true)
					_Ctrls.Animation.Root.Ctrl:moveToFront()
					_Ctrls.Animation.BeginFrameSpinner.Ctrl:setStepSize(1)
					_Ctrls.Animation.BeginFrameSpinner.Ctrl:setMinimumValue(0)
					_Ctrls.Animation.BeginFrameSpinner.Ctrl:setMaximumValue(
						amesh_scene_node:getAnimationOveralFramesCount() - 1)
					_Ctrls.Animation.BeginFrameSpinner.Ctrl:setCurrentValue(
						0)
					_Ctrls.Animation.EndFrameSpinner.Ctrl:setStepSize(1)
					_Ctrls.Animation.EndFrameSpinner.Ctrl:setMinimumValue(0)
					_Ctrls.Animation.EndFrameSpinner.Ctrl:setMaximumValue(
						amesh_scene_node:getAnimationOveralFramesCount() - 1)
					_Ctrls.Animation.EndFrameSpinner.Ctrl:setCurrentValue(
						amesh_scene_node:getAnimationOveralFramesCount() - 1)
					_ScenedAnimationMd2Handler = _ScenedAnimationMd2New
				end
				break
			end
		end			
	else
		if amesh_scene_node:getAnimationsCount() == 0 then
			return
		end
		local list = _Ctrls.Tabs.Anim.AnimList.Ctrl
		local item = list:getFirstSelectedItem()
		if item == nil then
			return
		end
		local index = item:getID()
		local full_file_name = amesh_scene_node:getAnimationFileName(index)
		if full_file_name ~= nil then
			local file_path = core.extractFilePath(full_file_name)
			local file_name = core.extractFileName(full_file_name)
			Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Cal3d animation...",
				file_path, file_name,
				{
					[1]     = { Descr = "Cal3d animation files",  Value = "caf" },
					["caf"] = { Descr = "Cal3d animation files",  Value = "caf" },
				},
				_ScenedAnimationCal3dReset)
		else
			local begin_frame = amesh_scene_node:getAnimationBeginFrame(index)
			local end_frame = amesh_scene_node:getAnimationEndFrame(index)		
			_Ctrls.Animation.Root.Ctrl:setVisible(true)
			_Ctrls.Animation.Root.Ctrl:moveToFront()
			_Ctrls.Animation.BeginFrameSpinner.Ctrl:setStepSize(1)
			_Ctrls.Animation.BeginFrameSpinner.Ctrl:setMinimumValue(0)
			_Ctrls.Animation.BeginFrameSpinner.Ctrl:setMaximumValue(
				amesh_scene_node:getAnimationOveralFramesCount() - 1)
			_Ctrls.Animation.BeginFrameSpinner.Ctrl:setCurrentValue(begin_frame)
			_Ctrls.Animation.EndFrameSpinner.Ctrl:setStepSize(1)
			_Ctrls.Animation.EndFrameSpinner.Ctrl:setMinimumValue(0)
			_Ctrls.Animation.EndFrameSpinner.Ctrl:setMaximumValue(
				amesh_scene_node:getAnimationOveralFramesCount() - 1)
			_Ctrls.Animation.EndFrameSpinner.Ctrl:setCurrentValue(end_frame)
			_ScenedAnimationMd2Handler = _ScenedAnimationMd2Reset
			_ScenedAnimationMd2StartFrameInit = begin_frame
			_ScenedAnimationMd2EndFrameInit = end_frame
		end
	end
end

function _ScenedAnimationDialogUpdatePlayingMode()
	local amesh_scene_node =
		_ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	for k, v in _Ctrls.Tabs.Anim.Buttons do
		v.Ctrl:setEnabled(false)
	end
	_Ctrls.Tabs.Anim.Buttons.AnimAddBtn.Ctrl:setEnabled(true)
	local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl:setVisible(item ~= nil)
	if item == nil then
		return
	end
	if amesh_scene_node:getAnimationsCount() == 0 then
		return
	end
	
	local animation_idx = item:getID()
	local playing = false
	if amesh_scene_node:getCurrentAnimation() == animation_idx
		and not MyTimer:isSuspended() then
		playing = true
	end
	_Ctrls.Tabs.Anim.Buttons.PlayBtn.Ctrl:setEnabled(not playing)
	_Ctrls.Tabs.Anim.Buttons.PauseBtn.Ctrl:setEnabled(playing)
	
	_Ctrls.Tabs.Anim.SpeedEditbox.Ctrl:setText(string.format(
		"%.2f", amesh_scene_node:getAnimationSpeed(animation_idx)))

	_Ctrls.Tabs.Anim.LoopedCheck.Ctrl:setSelected(
		amesh_scene_node:getAnimationLoopMode(animation_idx))
	
	local item_idx = _Ctrls.Tabs.Anim.AnimList.Ctrl:getItemRowIndex(item)
	local row_cnt = _Ctrls.Tabs.Anim.AnimList.Ctrl:getRowCount()
	if item_idx >= 0 and item_idx < row_cnt then
		if item_idx > 0 then
			_Ctrls.Tabs.Anim.Buttons.AnimMoveUpBtn.Ctrl:setEnabled(true)
		end
		if item_idx <  row_cnt - 1 then
			_Ctrls.Tabs.Anim.Buttons.AnimMoveDownBtn.Ctrl:setEnabled(true)
		end
	end
	_Ctrls.Tabs.Anim.Buttons.AnimRemBtn.Ctrl:setEnabled(true)
end

local _TimerSuspended = false

function _ScenedAnimatedMeshUpdate()
	if _TimerSuspended ~= MyTimer:isSuspended() then
		_ScenedAnimationDialogUpdatePlayingMode()
		_TimerSuspended = MyTimer:isSuspended()
	end
end

function _ScenedAnimatedMeshGetSelectedSceneNode()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local amesh_scene_node = nil
	if sel_nodes == 1 then
		local scene_node = MyScnMgr:getSelectedSceneNode(0)
		if scene_node:getSceneNodeType() == scn.ESNT_ANIMATED_MESH_SCENE_NODE then
			amesh_scene_node = tolua.cast(scene_node, "scn::IAnimatedMeshSceneNode")
		end
	end
	return amesh_scene_node
end

function _ScenedAnimatedMeshUpdateControls()
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	_Ctrls.Root.Ctrl:setVisible(amesh_scene_node ~= nil)
	if amesh_scene_node == nil then
		return
	end
	for key, value in pairs({
			{lod=scn.ELL_LOD_NEAREST, editbox=_Ctrls.Tabs.Mesh.LOD0Editbox.Ctrl,
				clear_btn=_Ctrls.Tabs.Mesh.Buttons.LOD0ClearBtn.Ctrl},
			{lod=scn.ELL_LOD_NEAR, editbox=_Ctrls.Tabs.Mesh.LOD1Editbox.Ctrl,
				clear_btn=_Ctrls.Tabs.Mesh.Buttons.LOD1ClearBtn.Ctrl},
			{lod=scn.ELL_LOD_FAR, editbox=_Ctrls.Tabs.Mesh.LOD2Editbox.Ctrl,
				clear_btn=_Ctrls.Tabs.Mesh.Buttons.LOD2ClearBtn.Ctrl},
			}) do
		local mesh = amesh_scene_node:getAnimatedMesh(value.lod)
		if mesh ~= nil then
			local full_file_name = MyScnMgr:findMeshFullFileName(mesh)
			local rel_file_name = Helper.getRelFileName(
				full_file_name, MyResMgr:getMediaDirFull(res.EMT_MESHES))
			value.editbox:setText(rel_file_name)
		else
			value.editbox:setText("")
		end
		value.clear_btn:setEnabled(mesh ~= nil)
	end

	local color = CEGUI.colour(1.0, 1.0, 1.0, 1.0)
	_Ctrls.Tabs.Anim.AnimList.Ctrl:resetList()
	for a = 0, amesh_scene_node:getAnimationsCount() - 1 do
		local full_file_name = amesh_scene_node:getAnimationFileName(a)
		local name = nil
		if full_file_name ~= nil then
			--local rel_file_name = Helper.getRelFileName(
				--full_file_name, MyResMgr:getMediaDirFull(res.EMT_MESHES))
			name = core.extractFileName(full_file_name)--rel_file_name
		else
			name = "frames "..tostring(amesh_scene_node:getAnimationBeginFrame(a))..
				"-"..tostring(amesh_scene_node:getAnimationEndFrame(a))
		end
		Helper.GUI.addMultiColumnListItem(
			_Ctrls.Tabs.Anim.AnimList.Ctrl,
			OPTIONS.HelperGUI.SchemeName, {
				[1] = {Name = a, ID = a, Data = nil, Color = color},
				[2] = {Name = name, ID = a, Data = nil, Color = color},
				})
	end
	
	_Ctrls.Tabs.Anim.AnimList.Ctrl:autoSizeColumnHeader(1)
	local dim = _Ctrls.Tabs.Anim.AnimList.Ctrl:getColumnHeaderWidth(1)
	if dim.offset < 150 then
		dim.scale = 0
		dim.offset = 150
		_Ctrls.Tabs.Anim.AnimList.Ctrl:setColumnHeaderWidth(1, dim)
	end
	_ScenedAnimationDialogUpdatePlayingMode()
	_ScenedAnimatedMeshUpdateCurrentPlayingAnimation(amesh_scene_node)

	_Ctrls.Tabs.Anim.SmoothCheck.Ctrl:setSelected(amesh_scene_node:isAnimationOveralSmooth())
	_Ctrls.Tabs.Anim.OveralSpeedEditbox.Ctrl:setText(string.format(
		"%.2f", amesh_scene_node:getAnimationOveralSpeed()))

	local imposter = amesh_scene_node:getImposter()
	if imposter ~= nil then
		_Ctrls.Tabs.Imposter.ParamsArea.Ctrl:show()
		_Ctrls.Tabs.Imposter.EnabledCheckbox.Ctrl:setSelected(true)
		-- common params
		local file_name = _ScenedAnimatedMeshImposterParameters:getFileName()
		if file_name == nil then
			_Ctrls.Tabs.Imposter.FileNameEditbox.Ctrl:setText("<Auto created>")
			_Ctrls.Tabs.Imposter.Buttons.OpenBtn.Ctrl:setEnabled(true)
			_Ctrls.Tabs.Imposter.Buttons.ClearBtn.Ctrl:setEnabled(false)
		else
			local tex_fname_rel = MyResMgr:getMediaFileNameRel(res.EMT_XML_SCRIPTS, file_name)
			MyLogger:logInfo(string.format("'%s' '%s' '%s' '%s'",
				file_name, tex_fname_rel, MyResMgr:getMediaDirRel(res.EMT_XML_SCRIPTS), MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS)))
			_Ctrls.Tabs.Imposter.FileNameEditbox.Ctrl:setText(tex_fname_rel)
			_Ctrls.Tabs.Imposter.Buttons.OpenBtn.Ctrl:setEnabled(true)
			_Ctrls.Tabs.Imposter.Buttons.ClearBtn.Ctrl:setEnabled(true)
		end
		_Ctrls.Tabs.Imposter.Buttons.SaveBtn.Ctrl:setEnabled(true)
		
		_Ctrls.Tabs.Imposter.ShiftXEditbox.Ctrl:setText(string.format("%.2f", _ScenedAnimatedMeshImposterParameters.ShiftCustom.X))
		_Ctrls.Tabs.Imposter.ShiftYEditbox.Ctrl:setText(string.format("%.2f", _ScenedAnimatedMeshImposterParameters.ShiftCustom.Y))
		_Ctrls.Tabs.Imposter.ShiftZEditbox.Ctrl:setText(string.format("%.2f", _ScenedAnimatedMeshImposterParameters.ShiftCustom.Z))
		_Ctrls.Tabs.Imposter.ViewOffsetEditbox.Ctrl:setText(string.format("%.2f", _ScenedAnimatedMeshImposterParameters.ViewOffset))
		_Ctrls.Tabs.Imposter.CenteredCheckbox.Ctrl:setSelected(_ScenedAnimatedMeshImposterParameters.Centered)
		-- creation params
		_Ctrls.Tabs.Imposter.CompressedCheckbox.Ctrl:setSelected(_ScenedAnimatedMeshImposterCreationParameters.Compress)

		_Ctrls.Tabs.Imposter.OptimalViewAngleEditbox.Ctrl :setText(string.format("%d",   _ScenedAnimatedMeshImposterCreationParameters.OptimalViewAngle))
		_Ctrls.Tabs.Imposter.SizeScaleWidthEditbox.Ctrl   :setText(string.format("%.2f", _ScenedAnimatedMeshImposterCreationParameters.SizeScale.X))
		_Ctrls.Tabs.Imposter.SizeScaleHeightEditbox.Ctrl  :setText(string.format("%.2f", _ScenedAnimatedMeshImposterCreationParameters.SizeScale.Y))
		_Ctrls.Tabs.Imposter.ViewScaleWidthEditbox.Ctrl   :setText(string.format("%.2f", _ScenedAnimatedMeshImposterCreationParameters.CameraViewScale.X))
		_Ctrls.Tabs.Imposter.ViewScaleHeightEditbox.Ctrl  :setText(string.format("%.2f", _ScenedAnimatedMeshImposterCreationParameters.CameraViewScale.Y))
		_Ctrls.Tabs.Imposter.TextureResolutionEditbox.Ctrl:setText(string.format("%d",   _ScenedAnimatedMeshImposterCreationParameters.TextureResolution))

		_Ctrls.Tabs.Imposter.ViewsCountSpinner.Ctrl:setCurrentValue(_ScenedAnimatedMeshImposterCreationParameters.ViewsCount)
		_Ctrls.Tabs.Imposter.FrameStepSpinner.Ctrl:setCurrentValue(_ScenedAnimatedMeshImposterCreationParameters.AnimationFrameStep)
		
		_ScenedAnimatedMeshColorUpdate()

		local changed_params = not _ScenedAnimatedMeshImposterCreationParameters:equal(
			amesh_scene_node:getImposterCreationParameters())
		_Ctrls.Tabs.Imposter.Buttons.ApplyBtn.Ctrl:setEnabled(changed_params)
		_Ctrls.Tabs.Imposter.Buttons.CancelBtn.Ctrl:setEnabled(changed_params)
		_Ctrls.Tabs.Imposter.Buttons.RecreateBtn.Ctrl:setEnabled(not changed_params)
	else
		_Ctrls.Tabs.Imposter.ParamsArea.Ctrl:hide()
		_Ctrls.Tabs.Imposter.EnabledCheckbox.Ctrl:setSelected(false)
	end

end

function _ScenedAnimatedMeshUpdateCurrentPlayingAnimation(amesh_scene_node)
	local animation_idx = amesh_scene_node:getCurrentAnimation()
	for i = 0, _Ctrls.Tabs.Anim.AnimList.Ctrl:getRowCount() - 1 do
		for j = 0, _Ctrls.Tabs.Anim.AnimList.Ctrl:getColumnCount() - 1 do
			local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getItemAtGridReference(CEGUI.MCLGridRef(i, j))
			local textitem = tolua.cast(item, "CEGUI::ListboxTextItem")	
			if i == animation_idx then
				if j == 0 then
					textitem:setText(string.format("%d*", i))
				end
				textitem:setTextColours(CEGUI.colour(1.0, 1.0, 0.5, 1.0))
			else
				if j == 0 then
					textitem:setText(string.format("%d", i))
				end
				textitem:setTextColours(CEGUI.colour(1.0, 1.0, 1.0, 1.0))
			end
		end
	end
	-- to update formatted cells
	local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
	if item ~= nil then
		_Ctrls.Tabs.Anim.AnimList.Ctrl:setItemSelectState(item, false)
		_Ctrls.Tabs.Anim.AnimList.Ctrl:setItemSelectState(item, true)
	else
		if _Ctrls.Tabs.Anim.AnimList.Ctrl:getRowCount() > 0
				and _Ctrls.Tabs.Anim.AnimList.Ctrl:getColumnCount() > 0 then
			local first_item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getItemAtGridReference(CEGUI.MCLGridRef(0, 0))
			_Ctrls.Tabs.Anim.AnimList.Ctrl:setItemSelectState(first_item, true)
			_Ctrls.Tabs.Anim.AnimList.Ctrl:setItemSelectState(first_item, false)
		end
	end

end
	
function _ScenedAnimatedMeshOnAnimationBegin(scene_node, animation_idx)
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	if not core.isPtrsEq(amesh_scene_node, scene_node) then
		return
	end
	_ScenedAnimationDialogUpdatePlayingMode()
	_ScenedAnimatedMeshUpdateCurrentPlayingAnimation(amesh_scene_node)
end

function _ScenedAnimatedMeshOnAnimationEnd(scene_node, animation_idx)
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	if not core.isPtrsEq(amesh_scene_node, scene_node) then
		return
	end
end

function _ScenedAnimatedMeshOpen(full_file_name, lod)
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	local mesh = MyScnMgr:findMesh(full_file_name)
	if mesh == nil then
		mesh = MyScnMgr:getMesh(full_file_name)
		if mesh ~= nil then
			mesh:startRegisteringAnimations()
			for a = 0, amesh_scene_node:getAnimationsCount() - 1 do
				local file_name   = amesh_scene_node:getAnimationFileName(a)
				local begin_frame = amesh_scene_node:getAnimationBeginFrame(a)
				local end_frame   = amesh_scene_node:getAnimationEndFrame(a)
				local begin_frame = amesh_scene_node:getAnimationBeginFrame(a)
				local speed       = amesh_scene_node:getAnimationSpeed(a)
				local looped      = amesh_scene_node:getAnimationLoopMode(a)
				if file_name ~= nil then
					mesh:registerAnimation(file_name, speed, looped)
				else
					mesh:registerAnimation(begin_frame, end_frame, speed, looped)
				end
			end
			mesh:endRegisteringAnimations()
		end
	end
	if mesh ~= nil then
		amesh_scene_node:setAnimatedMesh(mesh, lod)
		_ScenedAnimatedMeshUpdateControls()
		return true
	end
	return false
end

function _ScenedAnimatedMeshLOD0Open(full_file_name)
	if _ScenedAnimatedMeshOpen(full_file_name, scn.ELL_LOD_NEAREST) then
		_ScenedAnimatedMeshUpdateControls()
	end
end

function _ScenedAnimatedMeshLOD1Open(full_file_name)
	if _ScenedAnimatedMeshOpen(full_file_name, scn.ELL_LOD_NEAR) then
		_ScenedAnimatedMeshUpdateControls()
	end
end

function _ScenedAnimatedMeshLOD2Open(full_file_name)
	if _ScenedAnimatedMeshOpen(full_file_name, scn.ELL_LOD_FAR) then
		_ScenedAnimatedMeshUpdateControls()
	end
end

function _ScenedAnimatedMesh_MoveAnimation(up)
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
	if item == nil then
		return
	end
	local idx_inc = 1
	if up then
		idx_inc = -1
	end
	local animation_idx = item:getID()
	local item_idx = _Ctrls.Tabs.Anim.AnimList.Ctrl:getItemRowIndex(item)
	scn.IAnimatedMesh:startRegisteringAnimations(amesh_scene_node)
	scn.IAnimatedMesh:swapAnimations(amesh_scene_node, animation_idx, animation_idx + idx_inc)
	scn.IAnimatedMesh:endRegisteringAnimations(amesh_scene_node)
	_ScenedAnimatedMeshUpdateControls()
	_Ctrls.Tabs.Anim.AnimList.Ctrl:setItemSelectState(CEGUI.MCLGridRef(item_idx + idx_inc, 0), true)
end

local function _ScenedAnimatedMeshGetCurrentMeshPath()
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	local mesh_path = MyResMgr:getMediaDirFull(res.EMT_MESHES)
	for lod = scn.ELL_LOD_NEAREST, scn.ELL_LOD_FAR do
		local mesh = amesh_scene_node:getAnimatedMesh(lod)
		if mesh ~= nil then
			local full_file_name = MyScnMgr:findMeshFullFileName(mesh)
			local file_path = core.extractFilePath(full_file_name)
			if file_path ~= nil then
				mesh_path = file_path
				break
			end
		end
	end
	return mesh_path
end

local function _ScenedAnimatedMeshSetImposterDiffuseColor(color)
	_ScenedAnimatedMeshImposterCreationParameters.DiffuseColor:set(color)
	_ScenedAnimatedMeshUpdateControls()
end

local function _ScenedAnimatedMeshSetImposterAmbientColor(color)
	_ScenedAnimatedMeshImposterCreationParameters.AmbientColor:set(color)
	_ScenedAnimatedMeshUpdateControls()
end

local function _ScenedAnimatedMeshSetImposterSpecularColor(color)
	_ScenedAnimatedMeshImposterCreationParameters.SpecularColor:set(color)
	_ScenedAnimatedMeshUpdateControls()
end

local function _ScenedAnimatedMeshOpenImposter(full_file_name)
	if not MyFileSys:existFile(full_file_name) then
		MyLogger:logInfo("Can't open imposter from "..full_file_name)
		return
	end
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()	
	_ScenedAnimatedMeshImposterParameters:setFileName(full_file_name)
	amesh_scene_node:setImposterParameters(_ScenedAnimatedMeshImposterParameters)
	amesh_scene_node:detachImposter()
	amesh_scene_node:attachImposter(
		_ScenedAnimatedMeshImposterCreationParameters,
		_ScenedAnimatedMeshImposterParameters,
		false)
	_ScenedAnimatedMeshUpdateControls()
end

local _ScenedImposterTexCache = {}

function os_copy (source_path, dest_path)    
    local source = io.open(source_path, "rb")  
    local dest = io.open(dest_path, "wb")  
    dest:write(source:read("*a"))  
    source:close()    
    dest:close()    
end

local function _ScenedAnimatedMeshSaveImposter(full_file_name)
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	local imposter = amesh_scene_node:getImposter()
	if imposter ~= nil then
		local imposter_bill = nil
		if imposter:getSceneNodeType() == scn.ESNT_BILLBOARD_SCENE_NODE then
			imposter_bill = tolua.cast(imposter, "scn::IBillboardSceneNode")
		end
		if imposter_bill ~= nil then
			_ScenedImposterTexCache = {}
			local anim_cnt = imposter_bill:getAnimationsCount()
			for anim_idx = 0, anim_cnt - 1 do
				local dir_cnt = imposter_bill:getAnimationDirectionSetsCount(anim_idx)
				for dir_idx = 0, dir_cnt - 1 do
					local fr_cnt = imposter_bill:getAnimationDirectionSetFramesCount(anim_idx, dir_idx)
					for fr_idx = 0, fr_cnt - 1 do
						local frame = imposter_bill:getAnimationDirectionSetFrame(anim_idx, dir_idx, fr_idx)
						local tex_fname = MyDriver:findTextureName(frame.Texture)
						local tex = _ScenedImposterTexCache[tex_fname]
						if tex == nil then
							_ScenedImposterTexCache[tex_fname] = frame.Texture
						end
					end
				end
			end
			for key, value in pairs(_ScenedImposterTexCache) do
				if MyFileSys:existFile(key) then
					local tex_fname_new = string.format("%s%s",
						MyResMgr:getMediaDirFull(res.EMT_TEXTURES),
						core.extractFileName(key))					
					os_copy(key, tex_fname_new)
					MyDriver:setTextureName(value, tex_fname_new)
				end
			end
			_ScenedImposterTexCache = {}
		end
		local text = Helper.getSceneNodeFileText(imposter, true)
		local f = io.open (full_file_name, "w")
		if f ~= nil then
			f:write(text)
			f:close()
			_ScenedAnimatedMeshImposterParameters:setFileName(full_file_name)
			amesh_scene_node:setImposterParameters(_ScenedAnimatedMeshImposterParameters)
			_ScenedAnimatedMeshUpdateControls()
		end
	end
end

local function _ScenedAnimatedMeshWidgetClicked(args)

	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	
	if name == "Scened.Scene.AnimatedMesh.Mesh.LOD0OpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open animated mesh for LOD0...",
			_ScenedAnimatedMeshGetCurrentMeshPath(), nil, SCENED_ANIMATED_MESH_FILE_FILTER,
			_ScenedAnimatedMeshLOD0Open)
	elseif name == "Scened.Scene.AnimatedMesh.Mesh.LOD1OpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open animated mesh for LOD1...",
			_ScenedAnimatedMeshGetCurrentMeshPath(), nil, SCENED_ANIMATED_MESH_FILE_FILTER,
			_ScenedAnimatedMeshLOD1Open)
	elseif name == "Scened.Scene.AnimatedMesh.Mesh.LOD2OpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open animated mesh for LOD2...",
			_ScenedAnimatedMeshGetCurrentMeshPath(), nil, SCENED_ANIMATED_MESH_FILE_FILTER,
			_ScenedAnimatedMeshLOD2Open)
	elseif name == "Scened.Scene.AnimatedMesh.Mesh.LOD0ClearBtn" then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		amesh_scene_node:setAnimatedMesh(nil, scn.ELL_LOD_NEAREST)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Mesh.LOD1ClearBtn" then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		amesh_scene_node:setAnimatedMesh(nil, scn.ELL_LOD_NEAR)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Mesh.LOD2ClearBtn" then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		amesh_scene_node:setAnimatedMesh(nil, scn.ELL_LOD_FAR)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.Animation.Frame" then
		_Ctrls.Animation.Root.Ctrl:setVisible(false)
	elseif name == "Scened.Scene.Animation.OKBtn" then
		_Ctrls.Animation.Root.Ctrl:setVisible(false)
		local begin_frame = _Ctrls.Animation.BeginFrameSpinner.Ctrl:getCurrentValue()
		local end_frame = _Ctrls.Animation.EndFrameSpinner.Ctrl:getCurrentValue()
		_ScenedAnimationMd2Handler(begin_frame, end_frame)
		_ScenedAnimationMd2Handler = nil
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.Animation.CancelBtn" then
		_Ctrls.Animation.Root.Ctrl:setVisible(false)
		_ScenedAnimationMd2Handler = nil
		if not _ScenedAnimationCreateMode then
			_ScenedAnimationMd2Reset(
				_ScenedAnimationMd2StartFrameInit,
				_ScenedAnimationMd2EndFrameInit)
		end		
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Anim.PlayBtn" then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		Scened.resumeTimer()
		local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			amesh_scene_node:setCurrentAnimation(item:getID(), 0.0)
		end
	elseif name == "Scened.Scene.AnimatedMesh.Anim.PauseBtn" then
		Scened.suspendTimer()
	elseif name == "Scened.Scene.AnimatedMesh.Anim.AnimAddBtn" then
		_ScenedAnimationDialogShow(true)
	elseif name == "Scened.Scene.AnimatedMesh.Anim.AnimRemBtn" then
		local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local animation_idx = item:getID()
			local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
			local cur_anim = amesh_scene_node:getCurrentAnimation()
			scn.IAnimatedMesh:startRegisteringAnimations(amesh_scene_node)
			scn.IAnimatedMesh:unregisterAnimation(amesh_scene_node, animation_idx)
			scn.IAnimatedMesh:endRegisteringAnimations(amesh_scene_node)
			_ScenedAnimatedMeshUpdateControls()
			if animation_idx <= cur_anim then
				cur_anim = cur_anim - 1
				if cur_anim < 0 then
					if amesh_scene_node:getAnimationsCount() > 0 then
						amesh_scene_node:setCurrentAnimation(0)
					else
						_ScenedAnimationDialogUpdatePlayingMode()
					end				
				else
					amesh_scene_node:setCurrentAnimation(cur_anim)
				end
			end
			_ScenedAnimationDialogUpdatePlayingMode()
		end
	elseif name == "Scened.Scene.AnimatedMesh.Anim.AnimMoveUpBtn" then
		_ScenedAnimatedMesh_MoveAnimation(true)
	elseif name == "Scened.Scene.AnimatedMesh.Anim.AnimMoveDownBtn" then
		_ScenedAnimatedMesh_MoveAnimation(false)
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.ApplyBtn"
			or name == "Scened.Scene.AnimatedMesh.Imposter.RecreateBtn"
			then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		local imposter = amesh_scene_node:getImposter()
		if imposter ~= nil then
			amesh_scene_node:detachImposter()
			amesh_scene_node:attachImposter(
				_ScenedAnimatedMeshImposterCreationParameters,
				_ScenedAnimatedMeshImposterParameters,
				true)
		end
		_ScenedAnimatedMeshImposterParameters:setFileName(nil)
		amesh_scene_node:setImposterParameters(_ScenedAnimatedMeshImposterParameters)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.CancelBtn" then
		local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
		local imposter = amesh_scene_node:getImposter()
		if imposter ~= nil then
			_ScenedAnimatedMeshImposterParameters:set(amesh_scene_node:getImposterParameters())
			_ScenedAnimatedMeshImposterCreationParameters:set(amesh_scene_node:getImposterCreationParameters())
		end
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.DiffuseBtn" then
		_TmpColor:set(_ScenedAnimatedMeshImposterCreationParameters.DiffuseColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Imposter Diffuse Color", 0, _TmpColor, _ScenedAnimatedMeshSetImposterDiffuseColor)
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.AmbientBtn" then
		_TmpColor:set(_ScenedAnimatedMeshImposterCreationParameters.AmbientColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Imposter Ambient Color", 0, _TmpColor, _ScenedAnimatedMeshSetImposterAmbientColor)
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.SpecularBtn" then
		_TmpColor:set(_ScenedAnimatedMeshImposterCreationParameters.SpecularColor)
		Helper.GUI.ColorSelectionDialog.show(
			"Imposter Specular Color", 0, _TmpColor, _ScenedAnimatedMeshSetImposterSpecularColor)
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.SaveBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save Imposter...",
			MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS), nil,
			SCENE_NODE_FILE_FILTER,
			_ScenedAnimatedMeshSaveImposter)	
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.ClearBtn" then
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.OpenBtn" then
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open Imposter...",
			MyResMgr:getMediaDirFull(res.EMT_XML_SCRIPTS), nil,
			SCENE_NODE_FILE_FILTER,
			_ScenedAnimatedMeshOpenImposter)
	end

end

local function _ScenedAnimatedMeshSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.AnimatedMesh.Anim.AnimList" then
		_ScenedAnimationDialogUpdatePlayingMode()
	end
end

local function _ScenedAnimatedMeshDoubleClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.AnimatedMesh.Anim.AnimList" then
		local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			_ScenedAnimationDialogShow(false)
		end
	end
end

local function _ScenedAnimatedMeshTextAccepted(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if name == "Scened.Scene.AnimatedMesh.Anim.SpeedEditbox" then
		local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local animation_idx = item:getID()
			amesh_scene_node:setAnimationSpeed(
				animation_idx, tonumber(_Ctrls.Tabs.Anim.SpeedEditbox.Ctrl:getText()))
		end
	elseif name == "Scened.Scene.AnimatedMesh.Anim.OveralSpeedEditbox" then	
		amesh_scene_node:setAnimationOveralSpeed(
			tonumber(_Ctrls.Tabs.Anim.OveralSpeedEditbox.Ctrl:getText()))
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.ShiftXEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.ShiftYEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.ShiftZEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.ViewOffsetEditbox"
			then	
		_ScenedAnimatedMeshImposterParameters.ShiftCustom.X = tonumber(_Ctrls.Tabs.Imposter.ShiftXEditbox.Ctrl:getText())
		_ScenedAnimatedMeshImposterParameters.ShiftCustom.Y = tonumber(_Ctrls.Tabs.Imposter.ShiftYEditbox.Ctrl:getText())
		_ScenedAnimatedMeshImposterParameters.ShiftCustom.Z = tonumber(_Ctrls.Tabs.Imposter.ShiftZEditbox.Ctrl:getText())
		_ScenedAnimatedMeshImposterParameters.ViewOffset = tonumber(_Ctrls.Tabs.Imposter.ViewOffsetEditbox.Ctrl:getText())
		amesh_scene_node:setImposterParameters(_ScenedAnimatedMeshImposterParameters)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.OptimalViewAngleEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.SizeScaleWidthEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.SizeScaleHeightEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.ViewScaleWidthEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.ViewScaleHeightEditbox"
			or name == "Scened.Scene.AnimatedMesh.Imposter.TextureResolutionEditbox"
			then
		_ScenedAnimatedMeshImposterCreationParameters.OptimalViewAngle  = tonumber(_Ctrls.Tabs.Imposter.OptimalViewAngleEditbox.Ctrl :getText())
		_ScenedAnimatedMeshImposterCreationParameters.SizeScale.X       = tonumber(_Ctrls.Tabs.Imposter.SizeScaleWidthEditbox.Ctrl   :getText())
		_ScenedAnimatedMeshImposterCreationParameters.SizeScale.Y       = tonumber(_Ctrls.Tabs.Imposter.SizeScaleHeightEditbox.Ctrl  :getText())
		_ScenedAnimatedMeshImposterCreationParameters.CameraViewScale.X = tonumber(_Ctrls.Tabs.Imposter.ViewScaleWidthEditbox.Ctrl   :getText())
		_ScenedAnimatedMeshImposterCreationParameters.CameraViewScale.Y = tonumber(_Ctrls.Tabs.Imposter.ViewScaleHeightEditbox.Ctrl  :getText())
		_ScenedAnimatedMeshImposterCreationParameters.TextureResolution = tonumber(_Ctrls.Tabs.Imposter.TextureResolutionEditbox.Ctrl:getText())
		_ScenedAnimatedMeshUpdateControls()
	end	
end
	
local function _ScenedAnimatedMeshValueChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Scened.Scene.Animation.BeginFrameSpinner"
			or name == "Scened.Scene.Animation.EndFrameSpinner" then
		if _ScenedAnimationMd2Handler ~= nil then
			local start_frame = _Ctrls.Animation.BeginFrameSpinner.Ctrl:getCurrentValue()
			local end_frame = _Ctrls.Animation.EndFrameSpinner.Ctrl:getCurrentValue()
			_ScenedAnimationMd2Handler(start_frame, end_frame)
		end
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.ViewsCountSpinner"
			or name == "Scened.Scene.AnimatedMesh.Imposter.FrameStepSpinner" then
		_ScenedAnimatedMeshImposterCreationParameters.ViewsCount = _Ctrls.Tabs.Imposter.ViewsCountSpinner.Ctrl:getCurrentValue()
		_ScenedAnimatedMeshImposterCreationParameters.AnimationFrameStep = _Ctrls.Tabs.Imposter.FrameStepSpinner.Ctrl:getCurrentValue()
		_ScenedAnimatedMeshUpdateControls()
	end
end

local function _ScenedAnimatedMeshCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if name == "Scened.Scene.AnimatedMesh.Anim.LoopedCheck" then
		local item = _Ctrls.Tabs.Anim.AnimList.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local animation_idx = item:getID()
			amesh_scene_node:setAnimationLoopMode(
				animation_idx, _Ctrls.Tabs.Anim.LoopedCheck.Ctrl:isSelected())			
		end
	elseif name == "Scened.Scene.AnimatedMesh.Anim.SmoothCheck" then
		amesh_scene_node:setAnimationOveralSmooth(
			_Ctrls.Tabs.Anim.SmoothCheck.Ctrl:isSelected())
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.EnabledCheckbox" then
		if _Ctrls.Tabs.Imposter.EnabledCheckbox.Ctrl:isSelected() then
			local imposter = amesh_scene_node:getImposter()
			if imposter == nil then
				amesh_scene_node:attachImposter(
					_ScenedAnimatedMeshImposterCreationParameters,
					_ScenedAnimatedMeshImposterParameters,
					false)
			end
		else
			local imposter = amesh_scene_node:getImposter()
			if imposter ~= nil then
				amesh_scene_node:detachImposter()
			end
		end
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.CenteredCheckbox" then
		_ScenedAnimatedMeshImposterParameters.Centered = _Ctrls.Tabs.Imposter.CenteredCheckbox.Ctrl:isSelected()
		amesh_scene_node:setImposterParameters(_ScenedAnimatedMeshImposterParameters)
		_ScenedAnimatedMeshUpdateControls()
	elseif name == "Scened.Scene.AnimatedMesh.Imposter.CompressedCheckbox" then
		_ScenedAnimatedMeshImposterCreationParameters.Compress = _Ctrls.Tabs.Imposter.CompressedCheckbox.Ctrl:isSelected()
		_ScenedAnimatedMeshUpdateControls()
	end
end

function _ScenedInitAnimatedMeshOnSelectSceneNode()
	local amesh_scene_node = _ScenedAnimatedMeshGetSelectedSceneNode()
	if amesh_scene_node == nil then
		return
	end
	local imposter = amesh_scene_node:getImposter()
	if imposter ~= nil then
		_ScenedAnimatedMeshImposterParameters:set(amesh_scene_node:getImposterParameters())
		_ScenedAnimatedMeshImposterCreationParameters:set(amesh_scene_node:getImposterCreationParameters())
	end
	_ScenedAnimatedMeshUpdateControls()
end

function _ScenedInitAnimatedMeshControls()

	CEGUIImgsetMgr:create("TangoGUIIcons.imageset", "CommonImagesets")

	CEGUIWinMgr:loadWindowLayout("myengine_scened_scn_animated_mesh.layout", "", "CommonLayouts")	
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:createWindow(
		"MyScened/TabControl", "Scened.Scene.AnimatedMeshTabControl")
	CEGUIWinMgr:getWindow("Scened.SceneTab"):addChildWindow(_Ctrls.Root.Ctrl)
	_Ctrls.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0.02,0},{0.02,0},{0.98,0},{0.98,0}}")
	_Ctrls.Root.Ctrl:setVisible(false)

	_Ctrls.TabControl.Ctrl = tolua.cast(_Ctrls.Root.Ctrl, "CEGUI::TabControl")
	_Ctrls.TabControl.Ctrl:setProperty("TabHeight", "{0,30}")
    _Ctrls.TabControl.Ctrl:setProperty("TabTextPadding", "{0,5}")
    _Ctrls.TabControl.Ctrl:setProperty("TabPanePosition", "Top")
	_Ctrls.TabControl.Ctrl:setProperty("Font", "Tahoma-8")
	for key, value in _Ctrls.Tabs do
		value.Root.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh."..key..".Root")
		value.Root.Ctrl:setText(value.Caption)
		value.Root.Ctrl:setProperty("UnifiedAreaRect", "{{0,0},{0.025,0},{1,0},{0.975,0}}")
	end
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Mesh.Root.Ctrl)
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Anim.Root.Ctrl)
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Imposter.Root.Ctrl)
	_Ctrls.TabControl.Ctrl:addTab(_Ctrls.Tabs.Shadow.Root.Ctrl)

	Helper.GUI.initButtons("Scened.Scene.AnimatedMesh.Mesh.", _Ctrls.Tabs.Mesh.Buttons,
		_ScenedAnimatedMeshWidgetClicked)

	_Ctrls.Tabs.Mesh.LOD0Editbox.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh.Mesh.LOD0Editbox")
	_Ctrls.Tabs.Mesh.LOD1Editbox.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh.Mesh.LOD1Editbox")
	_Ctrls.Tabs.Mesh.LOD2Editbox.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh.Mesh.LOD2Editbox")
	
	Helper.GUI.initButtons("Scened.Scene.AnimatedMesh.Anim.", _Ctrls.Tabs.Anim.Buttons,
		_ScenedAnimatedMeshWidgetClicked)
		
	_Ctrls.Tabs.Anim.ParamsArea.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh.Anim.ParamsArea")
	
	_Ctrls.Tabs.Anim.AnimList.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Anim.AnimList"), "CEGUI::MultiColumnList")
	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.Tabs.Anim.AnimList.Ctrl, 0, {
			[1] = {Name = "#", Dim = CEGUI.UDim(0.15, 0)},
			[2] = {Name = "Anim.", Dim = CEGUI.UDim(0.70, 0)},
			})
	_Ctrls.Tabs.Anim.AnimList.Ctrl:subscribeEvent("SelectionChanged", _ScenedAnimatedMeshSelectionChanged)
	_Ctrls.Tabs.Anim.AnimList.Ctrl:subscribeEvent("MouseDoubleClick", _ScenedAnimatedMeshDoubleClick)

	_Ctrls.Tabs.Anim.LoopedCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Anim.LoopedCheck"), "CEGUI::Checkbox")
	_Ctrls.Tabs.Anim.SmoothCheck.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Anim.SmoothCheck"), "CEGUI::Checkbox")

	_Ctrls.Tabs.Anim.LoopedCheck.Ctrl:subscribeEvent("CheckStateChanged", _ScenedAnimatedMeshCheckStateChanged)
	_Ctrls.Tabs.Anim.SmoothCheck.Ctrl:subscribeEvent("CheckStateChanged", _ScenedAnimatedMeshCheckStateChanged)
		
	_Ctrls.Tabs.Anim.SpeedEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Anim.SpeedEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Anim.OveralSpeedEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Anim.OveralSpeedEditbox"), "CEGUI::Editbox")

	_Ctrls.Tabs.Anim.SpeedEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	_Ctrls.Tabs.Anim.OveralSpeedEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
		
	_Ctrls.Animation.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_scened_scn_animated_mesh_animation.layout", "", "CommonLayouts")
	_Ctrls.Animation.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Animation.Root.Ctrl)
	
	_Ctrls.Animation.Frame.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.Animation.Frame")
	_Ctrls.Animation.Frame.Ctrl:subscribeEvent("CloseClicked", _ScenedAnimatedMeshWidgetClicked)
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Animation.Frame.Ctrl)
	
	Helper.GUI.initButtons("Scened.Scene.Animation.", _Ctrls.Animation.Buttons,
		_ScenedAnimatedMeshWidgetClicked)
	
	_Ctrls.Animation.BeginFrameSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Animation.BeginFrameSpinner"), "CEGUI::Spinner")
	_Ctrls.Animation.BeginFrameSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedAnimatedMeshValueChanged)

	_Ctrls.Animation.EndFrameSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.Animation.EndFrameSpinner"), "CEGUI::Spinner")
	_Ctrls.Animation.EndFrameSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedAnimatedMeshValueChanged)

	Helper.GUI.initButtons("Scened.Scene.AnimatedMesh.Imposter.", _Ctrls.Tabs.Imposter.Buttons,
		_ScenedAnimatedMeshWidgetClicked)

	_Ctrls.Tabs.Imposter.EnabledCheckbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.EnabledCheckbox"), "CEGUI::Checkbox")
	_Ctrls.Tabs.Imposter.EnabledCheckbox.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedAnimatedMeshCheckStateChanged)

	_Ctrls.Tabs.Imposter.CenteredCheckbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.CenteredCheckbox"), "CEGUI::Checkbox")
	_Ctrls.Tabs.Imposter.CenteredCheckbox.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedAnimatedMeshCheckStateChanged)
		
	_Ctrls.Tabs.Imposter.ShiftXEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ShiftXEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ShiftXEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ShiftYEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ShiftYEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ShiftYEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ShiftZEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ShiftZEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ShiftZEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ViewOffsetEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ViewOffsetEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ViewOffsetEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)

	_Ctrls.Tabs.Imposter.FileNameEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.FileNameEditbox"), "CEGUI::Editbox")

	-- Creation
	
	_Ctrls.Tabs.Imposter.CompressedCheckbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.CompressedCheckbox"), "CEGUI::Checkbox")
	_Ctrls.Tabs.Imposter.CompressedCheckbox.Ctrl:subscribeEvent(
		"CheckStateChanged", _ScenedAnimatedMeshCheckStateChanged)		
	
	_Ctrls.Tabs.Imposter.ViewsCountSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ViewsCountSpinner"), "CEGUI::Spinner")
	_Ctrls.Tabs.Imposter.ViewsCountSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedAnimatedMeshValueChanged)
	
	_Ctrls.Tabs.Imposter.FrameStepSpinner.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.FrameStepSpinner"), "CEGUI::Spinner")
	_Ctrls.Tabs.Imposter.FrameStepSpinner.Ctrl:subscribeEvent(
		"ValueChanged", _ScenedAnimatedMeshValueChanged)
		
	_Ctrls.Tabs.Imposter.OptimalViewAngleEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.OptimalViewAngleEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.OptimalViewAngleEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.SizeScaleWidthEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.SizeScaleWidthEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.SizeScaleWidthEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.SizeScaleHeightEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.SizeScaleHeightEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.SizeScaleHeightEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ViewScaleWidthEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ViewScaleWidthEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ViewScaleWidthEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ViewScaleHeightEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ViewScaleHeightEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.ViewScaleHeightEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.TextureResolutionEditbox.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.TextureResolutionEditbox"), "CEGUI::Editbox")
	_Ctrls.Tabs.Imposter.TextureResolutionEditbox.Ctrl:subscribeEvent("TextAccepted", _ScenedAnimatedMeshTextAccepted)
	
	_Ctrls.Tabs.Imposter.ParamsArea.Ctrl = CEGUIWinMgr:getWindow(
		"Scened.Scene.AnimatedMesh.Imposter.ParamsArea")
	_Ctrls.Tabs.Imposter.ParamsArea.Ctrl:hide()

	for key, value in pairs(_Ctrls.Tabs.Imposter.ColorImages) do
		value.Ctrl = CEGUIWinMgr:getWindow("Scened.Scene.AnimatedMesh.Imposter."..key)
		local cegui_tex = nil
		local imageset = nil
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		_Resources.Textures.ColorImages[key] = cegui_tex
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
		imageset = CEGUIImgsetMgr:create("Scened.Scene.AnimatedMesh.Imposter."..key, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(1,1), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.Ctrl:setProperty("Image", "set:Scened.Scene.AnimatedMesh.Imposter."..key.." image:ColorImage")
	end

	Helper.GUI.initButtons("Scened.Scene.AnimatedMesh.Imposter.",
		_Ctrls.Tabs.Imposter.Buttons, _ScenedAnimatedMeshWidgetClicked)
		
	return {
		Ctrl = _Ctrls.Root.Ctrl,
		update = _ScenedAnimatedMeshUpdate,
		updateControls = _ScenedAnimatedMeshUpdateControls,
		onSelectSceneNode = _ScenedInitAnimatedMeshOnSelectSceneNode,
		onDeselectSceneNode = nil,
		onAnimationStart = _ScenedAnimatedMeshOnAnimationBegin,
		onAnimationEnd = _ScenedAnimatedMeshOnAnimationEnd,
		}
end
