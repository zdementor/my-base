
local _Ctrls =
{
	Root = {},
	Frame = {},
	Image = {Name="Helper.ImageEditor.Image"},
	Buttons =
	{
		SaveButton			= {Name="Helper.ImageEditor.SaveButton", ImageName = "set:TangoGUIIcons image:actions.document-save"				},
		OpenButton			= {Name="Helper.ImageEditor.OpenButton", ImageName = "set:TangoGUIIcons image:actions.document-open"				},
		ReloadButton		= {Name="Helper.ImageEditor.ReloadButton", ImageName = "set:TangoGUIIcons image:actions.view-refresh"				},
		IncScaleButton		= {Name="Helper.ImageEditor.IncScaleButton", ImageName = "set:TangoGUIIcons image:actions.image-zoomin"				},
		DecScaleButton		= {Name="Helper.ImageEditor.DecScaleButton", ImageName = "set:TangoGUIIcons image:actions.image-zoomout"				},
		FullscreenButton	= {Name="Helper.ImageEditor.FullscreenButton", ImageName = "set:TangoGUIIcons image:actions.view-fullscreen"			},
		WindowedButton		= {Name="Helper.ImageEditor.WindowedButton", ImageName = "set:TangoGUIIcons image:actions.view-windowed"				},
		BottomAlignButton	= {Name="Helper.ImageEditor.BottomAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-bottom-aligned"		},
		TopAlignButton		= {Name="Helper.ImageEditor.TopAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-top-aligned"			},
		LeftAlignButton		= {Name="Helper.ImageEditor.LeftAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-left-aligned"			},
		RightAlignButton	= {Name="Helper.ImageEditor.RightAlignButton", ImageName = "set:TangoGUIIcons image:actions.view-right-aligned"			},
		IdentScaleButton	= {Name="Helper.ImageEditor.IdentScaleButton", ImageName = nil },
	},
	ScaleScroll = {Name="Helper.ImageEditor.ScaleScroll"},
	AlphaCheckbox = {Name="Helper.ImageEditor.AlphaCheckbox"},
	ScrollArea = {Name="Helper.ImageEditor.ScrollArea"}
}

local _Callbacks =
{
	Open	= nil,
	Reload	= nil,
	Save	= nil,
	Close	= nil,
}

local _Resources =
{
	Textures =
	{
		ImageEmpty = nil,
		Image = nil,
	},
	Imagesets =
	{
		ImageEmpty = nil,
		Image = nil,
	},
}

local IMAGE_EDITOR_VIEW_SCROLL_IDENT = 3.0
local IMAGE_EDITOR_VIEW_SCROLL_STEP = 1.0

local _ImageEditorCurScale = 1.0
local IMAGE_EDITOR_VIEW_SCALE_MIN  = math.pow(2.0, -IMAGE_EDITOR_VIEW_SCROLL_IDENT)
local IMAGE_EDITOR_VIEW_SCALE_MAX  = math.pow(2.0, IMAGE_EDITOR_VIEW_SCROLL_IDENT)

local _ImageEditorCaption = "Image Editor:"
local _ImageEditorFileName = ""

local IMAGE_EDITOR_STATE_FULLSCREEN		= 0
local IMAGE_EDITOR_STATE_WINDOWED		= 1
local IMAGE_EDITOR_STATE_BOTTOM_ALIGNED	= 2
local IMAGE_EDITOR_STATE_TOP_ALIGNED	= 3
local IMAGE_EDITOR_STATE_LEFT_ALIGNED	= 4
local IMAGE_EDITOR_STATE_RIGHT_ALIGNED	= 5

local _ImageEditorState = IMAGE_EDITOR_STATE_WINDOWED
local _ImageEditorFlags = 0
local _ImageSize = core.dimension2di(255, 255)
local _ImageEditorUserData = nil

local _ImageFrameWindowedAreaRectProperty = nil

local _ImageEditorImageName = nil
local _ImageEditorImageSaveFileName = nil

local function _ImageEditorSetScrollScale(scale)
	if scale <= IMAGE_EDITOR_VIEW_SCALE_MIN then
		scale = IMAGE_EDITOR_VIEW_SCALE_MIN
	end
	if scale >= IMAGE_EDITOR_VIEW_SCALE_MAX then
		scale = IMAGE_EDITOR_VIEW_SCALE_MAX
	end
	_ImageEditorCurScale = scale
end

local function _ImageEditorUpdateScaleControls()
	if _ImageEditorCurScale <= IMAGE_EDITOR_VIEW_SCALE_MIN then
		_Ctrls.Buttons.DecScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.DecScaleButton.Ctrl:enable()
	end
	if _ImageEditorCurScale >= IMAGE_EDITOR_VIEW_SCALE_MAX then
		_Ctrls.Buttons.IncScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.IncScaleButton.Ctrl:enable()
	end
	if _ImageEditorCurScale == 1.0 then
		_Ctrls.Buttons.IdentScaleButton.Ctrl:disable()
	else
		_Ctrls.Buttons.IdentScaleButton.Ctrl:enable()
	end
	
	local img_sz =  CEGUI.UVector2(
		CEGUI.UDim(0.0, _ImageSize.Width * _ImageEditorCurScale),
		CEGUI.UDim(0.0, _ImageSize.Height * _ImageEditorCurScale))

	local img_area = CEGUI.URect(_Ctrls.Image.Ctrl:getArea().min, _Ctrls.Image.Ctrl:getArea().max)
	img_area.max.x.offset = img_area.min.x.offset + img_sz.x.offset
	img_area.max.y.offset = img_area.min.y.offset + img_sz.y.offset

	local vertScroll = _Ctrls.ScrollArea.Ctrl:getVertScrollbar()
	local horzScroll = _Ctrls.ScrollArea.Ctrl:getHorzScrollbar()

	local vertProgress = Helper.scrollbarGetProgress(vertScroll)
	local horzProgress = Helper.scrollbarGetProgress(horzScroll)

	_Ctrls.Image.Ctrl:setArea(img_area)
	_Ctrls.Image.Ctrl:setMaxSize(img_sz)

	Helper.scrollbarSetProgress(vertScroll, vertProgress)
	Helper.scrollbarSetProgress(horzScroll, horzProgress)
end

local function _ImageEditorUpdateControls()

	for key, value in pairs(_Ctrls.Buttons) do
		value.Ctrl:enable()
	end
	_Ctrls.Buttons.SaveButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.SAVE) ~= 0)
	_Ctrls.Buttons.OpenButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.OPEN) ~= 0)
	_Ctrls.Buttons.ReloadButton.Ctrl:setEnabled(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.RELOAD) ~= 0)
	
	_Ctrls.Frame.Ctrl:setSizingEnabled(false)
	
	if _ImageEditorState == IMAGE_EDITOR_STATE_FULLSCREEN then
		_Ctrls.Buttons.FullscreenButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_WINDOWED then
		_Ctrls.Buttons.WindowedButton.Ctrl:disable()
		_Ctrls.Frame.Ctrl:setSizingEnabled(true)
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_BOTTOM_ALIGNED then
		_Ctrls.Buttons.BottomAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_TOP_ALIGNED then
		_Ctrls.Buttons.TopAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_LEFT_ALIGNED then
		_Ctrls.Buttons.LeftAlignButton.Ctrl:disable()
	elseif _ImageEditorState == IMAGE_EDITOR_STATE_RIGHT_ALIGNED then
		_Ctrls.Buttons.RightAlignButton.Ctrl:disable()
	end

	_ImageEditorUpdateScaleControls()
end

local function _ImageEditorSetTexture(my_tex)
	local cegui_tex = _Resources.Textures.Image
	local useAlpha = false
	if bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.USE_IMAGE_ALPHA) ~= 0 then
		useAlpha = true
	end
	MyCEGUI.setTexture(cegui_tex, my_tex, useAlpha)
	if my_tex ~= nil then
		local imageset = _Resources.Imagesets.Image
		imageset:undefineAllImages()
		imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0))
		_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.Image image:Image")
		_ImageSize:set(cegui_tex:getSize().width, cegui_tex:getSize().height)
	else
		_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.ImageEmpty image:Image")
		_ImageSize:set(256, 256)
	end
end

local function _ImageEditorOpenImage(full_file_name)

	local my_tex = nil
	local image = nil

	if full_file_name ~= nil then
		my_tex = MyDriver:findTexture(full_file_name)
	end

	if my_tex == nil then
		if full_file_name ~= nil then
			image = MyImgLib:getImage(full_file_name)
		end
	end

	if image ~= nil then
   		my_tex = MyDriver:addTexture(full_file_name, image)
		MyImgLib:removeImage(image)
	end

	_ImageEditorSetTexture(my_tex)

	if my_tex == nil then
		if full_file_name ~= nil then
			LOG_ERR("Can't open image from file '"..tostring(full_file_name).."'")
		end
	end

	_ImageEditorFileName = full_file_name
	_Ctrls.Frame.Ctrl:setText(
		_ImageEditorCaption..": "..core.extractFileName(full_file_name))

	_ImageEditorUpdateScaleControls()

	_ImageEditorImageName = full_file_name
	_ImageEditorImageSaveFileName = full_file_name

	return (my_tex ~= nil)
end

local function _ImageEditorOpenImageAccepted(full_file_name, rel_full_file_name, user_data)
	local my_tex = _ImageEditorOpenImage(full_file_name)
	if my_tex ~= nil then	
		if _Callbacks.Open ~= nil then
			_Callbacks.Open(full_file_name, rel_full_file_name, user_data)
		end
	end
end

local function _ImageEditorSaveImageAccepted(full_file_name, rel_full_file_name, user_data)
	local cegui_tex = _Resources.Textures.Image
	local imageset = _Resources.Imagesets.Image
	local my_tex = MyCEGUI.getTexture(cegui_tex)
	local image = MyImgLib:findImage(_ImageEditorImageName)
	if image == nil then
		local tex_file_name = MyDriver:findTextureName(my_tex)
		if tex_file_name ~= nil then
			image = MyImgLib:getImage(tex_file_name)
		end
	end
	if image ~= nil then
		MyImgLib:saveImageToFile(image, full_file_name)
		if _Callbacks.Save ~= nil then
			_Callbacks.Save(full_file_name, rel_full_file_name, user_data)
		end
	else
		LOG_INFO("Can't save image to "..full_file_name)
	end
end

local function _ImageEditorSetFileName(file_name)
	if not _HelperInit() then
		return false
	end
	_ImageEditorOpenImage(file_name)
	_ImageEditorUpdateControls()
	return true
end

local function _ImageEditorSetState(state)
	if _ImageEditorState == IMAGE_EDITOR_STATE_WINDOWED and state ~= IMAGE_EDITOR_STATE_WINDOWED then
		_ImageFrameWindowedAreaRectProperty = _Ctrls.Frame.Ctrl:getProperty("UnifiedAreaRect")
	end
	if state == IMAGE_EDITOR_STATE_FULLSCREEN then
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{1.0,0},{1.0,0}}")
	elseif state == IMAGE_EDITOR_STATE_WINDOWED then
		if _ImageFrameWindowedAreaRectProperty == nil then
			_ImageFrameWindowedAreaRectProperty = "{{0.1,0},{0.1,0},{0.9,0},{0.9,0}}"
		end
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", _ImageFrameWindowedAreaRectProperty)
	elseif state == IMAGE_EDITOR_STATE_BOTTOM_ALIGNED then
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.5,0},{1.0,0},{1.0,0}}")
	elseif state == IMAGE_EDITOR_STATE_TOP_ALIGNED then
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.,0},{0.0,0},{1.0,0},{0.5,0}}")
	elseif state == IMAGE_EDITOR_STATE_LEFT_ALIGNED then
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.0,0},{0.0,0},{0.66,0},{1.0,0}}")
	elseif state == IMAGE_EDITOR_STATE_RIGHT_ALIGNED then
		_Ctrls.Frame.Ctrl:setProperty("UnifiedAreaRect", "{{0.33,0},{0.0,0},{1.0,0},{1.0,0}}")
	end
	_ImageEditorState = state
end

local _ImageEditorDragging = false
local _ImageEditorStartDragPosX = -1
local _ImageEditorStartDragPosY = -1

local function _ImageEditorDrag(x, y, reset)
	if not _ImageEditorDragging or (reset ~= nil and reset) then
		CEGUICursor:setImage(OPTIONS.ScenedGUI.SchemeName, "MouseMoveCursor")
		_ImageEditorDragging = true
	end
	_ImageEditorStartDragPosX = x
	_ImageEditorStartDragPosY = y
end

local function _ImageEditorDragStop()
	if _ImageEditorDragging then
		CEGUICursor:setImage(OPTIONS.ScenedGUI.SchemeName, "MouseArrow")
		_ImageEditorStartDragPosX = -1
		_ImageEditorStartDragPosY = -1
		_ImageEditorDragging = false
	end
end

local function _ImageEditorImageMouseEnter(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
		if _ImageEditorDragging then
			if me.sysKeys == CEGUI.RightMouse then
				_ImageEditorDrag(me.position.x, me.position.y, true)
			else
				_ImageEditorDragStop()
			end
		end
	end
end

local function _ImageEditorImageMouseLeave(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
	end
end

local function _ImageEditorImageMouseMove(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
		if _ImageEditorDragging then
			local vertScroll = _Ctrls.ScrollArea.Ctrl:getVertScrollbar()
			local horzScroll = _Ctrls.ScrollArea.Ctrl:getHorzScrollbar()
			if horzScroll:isVisible() then
				horzScroll:setScrollPosition(horzScroll:getScrollPosition() +
					_ImageEditorStartDragPosX - me.position.x)
			end
			if vertScroll:isVisible() then
				vertScroll:setScrollPosition(vertScroll:getScrollPosition() +
					_ImageEditorStartDragPosY - me.position.y)
			end
			_ImageEditorDrag(me.position.x, me.position.y)
		end
	end
end

local function _ImageEditorImageMouseWhell(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
		local win = we.window
		local horzScroll = _Ctrls.ScrollArea.Ctrl:getHorzScrollbar()
		local vertScroll = _Ctrls.ScrollArea.Ctrl:getVertScrollbar()

		local rect = win:getUnclippedOuterRect()
		local rWidth  = rect:getWidth()
		local rHeight = rect:getHeight()
		local imgX = (me.position.x - rect.left) / rWidth
		local imgY = (me.position.y - rect.top) / rHeight
	
		local scaleScroll = _Ctrls.ScaleScroll.Ctrl
		local scaleStep = scaleScroll:getStepSize()
		local scalePos = scaleScroll:getScrollPosition()
		scaleScroll:setScrollPosition(scalePos + me.wheelChange * scaleStep);

		rect = win:getUnclippedOuterRect()
		rWidth  = rect:getWidth()
		rHeight = rect:getHeight()

		local rect4 = win:getClipRect()
		local imgX4 = (me.position.x - rect4.left) / rWidth
		local imgY4= (me.position.y - rect4.top) / rHeight

		local progrX = (horzScroll:getDocumentSize() * (imgX-imgX4))
		if progrX > (horzScroll:getDocumentSize() - horzScroll:getPageSize()) then
			progrX = 1.
		else
			progrX = progrX / (horzScroll:getDocumentSize() - horzScroll:getPageSize())
		end
		local progrY = (vertScroll:getDocumentSize() * (imgY-imgY4))
		if progrY > (vertScroll:getDocumentSize() - vertScroll:getPageSize()) then
			progrY = 1.
		else
			progrY = progrY / (vertScroll:getDocumentSize() - vertScroll:getPageSize())
		end

		Helper.scrollbarSetProgress(horzScroll, progrX)
		Helper.scrollbarSetProgress(vertScroll, progrY)
	end
end

local function _ImageEditorImageMouseButton(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
		if me.button == CEGUI.RightButton and me.sysKeys == CEGUI.RightMouse then
			_ImageEditorDrag(me.position.x, me.position.y)
		else
			_ImageEditorDragStop()
		end
	end
end

local function _ImageEditorImageMouseClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
		if me.button == CEGUI.RightButton then
			_ImageEditorDragStop()
		end
	end
end

local function _ImageEditorImageMouseDoubleClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.Image.Ctrl:getName() then
		local me = CEGUI.toMouseEventArgs(args)
	end
end

local function _ImageEditorClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.ImageEditor.Frame" then
		_ImageEditorSetFileName(nil) -- to release texture
		_Ctrls.Root.Ctrl:setVisible(false)
		if _Callbacks.Close ~= nil then
			_Callbacks.Close()
		end
	elseif name == _Ctrls.Buttons.DecScaleButton.Name then
		local scroll = _Ctrls.ScaleScroll.Ctrl:getScrollPosition() - IMAGE_EDITOR_VIEW_SCROLL_STEP
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(scroll)
	elseif name == _Ctrls.Buttons.IncScaleButton.Name then		
		local scroll = _Ctrls.ScaleScroll.Ctrl:getScrollPosition() + IMAGE_EDITOR_VIEW_SCROLL_STEP
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(scroll)
	elseif name == _Ctrls.Buttons.IdentScaleButton.Name then		
		 _Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)
	elseif name == _Ctrls.Buttons.SaveButton.Name then
		local save_file_name = _ImageEditorImageSaveFileName
		if save_file_name == nil then
			local cegui_tex = _Resources.Textures.Image
			local my_tex = MyCEGUI.getTexture(cegui_tex)
			local tex_file_name = MyDriver:findTextureName(my_tex)
			save_file_name = tex_file_name
		end
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.SAVE, 0, "Save image...",
			MyResMgr:getMediaDirFull(res.EMT_TEXTURES),
			save_file_name,
			SCENED_IMAGE_SAVE_FILE_FILTER, _ImageEditorSaveImageAccepted, _ImageEditorUserData)
	elseif name == _Ctrls.Buttons.OpenButton.Name then
		local cegui_tex = _Resources.Textures.Image
		local my_tex = MyCEGUI.getTexture(cegui_tex)
		local tex_file_name = nil
		local start_dir = MyResMgr:getMediaDirFull(res.EMT_TEXTURES)
		if my_tex ~= nil then
			tex_file_name = MyDriver:findTextureName(my_tex)
			if string.len(tex_file_name) > 0 and string.sub(tex_file_name, 0, 1) == "#" then
				tex_file_name = nil
			else
				start_dir = core.extractFilePath(tex_file_name)
			end
		end						
		Helper.GUI.FileDialog.show(FILE_DIALOG_MODE.OPEN, 0, "Open image...",
			start_dir, tex_file_name,
			SCENED_IMAGE_OPEN_FILE_FILTER, _ImageEditorOpenImageAccepted, _ImageEditorUserData)
	elseif name == _Ctrls.Buttons.ReloadButton.Name then
		_ImageEditorSetFileName(_ImageEditorFileName)
		_Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)
		if _Callbacks.Reload ~= nil then
			_Callbacks.Reload(_ImageEditorFileName, nil, _ImageEditorUserData)
		end
	elseif name == _Ctrls.Buttons.FullscreenButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_FULLSCREEN)
	elseif name == _Ctrls.Buttons.WindowedButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_WINDOWED)
	elseif name == _Ctrls.Buttons.BottomAlignButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_BOTTOM_ALIGNED)
	elseif name == _Ctrls.Buttons.TopAlignButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_TOP_ALIGNED)
	elseif name == _Ctrls.Buttons.LeftAlignButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_LEFT_ALIGNED)
	elseif name == _Ctrls.Buttons.RightAlignButton.Name then
		_ImageEditorSetState(IMAGE_EDITOR_STATE_RIGHT_ALIGNED)
	end
	_ImageEditorUpdateControls()
end

local function _ImageEditorScrollPosChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.ScaleScroll.Name then
		local scale = math.pow(2, _Ctrls.ScaleScroll.Ctrl:getScrollPosition() - IMAGE_EDITOR_VIEW_SCROLL_IDENT)
		_ImageEditorSetScrollScale(scale)
		_ImageEditorUpdateScaleControls()
	end
end

local function _ImageEditorCheckStateChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == _Ctrls.AlphaCheckbox.Name then
		local cegui_tex = _Resources.Textures.Image
		local my_tex = MyCEGUI.getTexture(cegui_tex)
		local useAlpha = _Ctrls.AlphaCheckbox.Ctrl:isSelected()
		local alphaFlag = IMAGE_EDITOR_FLAGS.USE_IMAGE_ALPHA
		if not useAlpha then
			alphaFlag = bit.bnot(alphaFlag)
			_ImageEditorFlags = bit.band(_ImageEditorFlags, alphaFlag)
		else
			_ImageEditorFlags = bit.bor(_ImageEditorFlags, alphaFlag)
		end
		_ImageEditorSetTexture(my_tex)
	end
end

function _ImageEditorInit()

	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout(
		"myengine_helper_ImageEditor.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)

	_Ctrls.Frame.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.ImageEditor.Frame"), "CEGUI::FrameWindow")

	_Ctrls.ScrollArea.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow(_Ctrls.ScrollArea.Name), "CEGUI::ScrollablePane")

	local cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 40, 40, 40))
	_Resources.Textures.ImageEmpty = cegui_tex
	imageset = CEGUIImgsetMgr:create("Helper.ImageEditor.ImageEmpty", cegui_tex)
	imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.ImageEmpty = imageset
	
	cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
	Helper.GUI.fillCEGUITextureWithColor(cegui_tex, img.SColor(255, 0, 0, 0))
	_Resources.Textures.Image = cegui_tex
	imageset = CEGUIImgsetMgr:create("Helper.ImageEditor.Image", cegui_tex)
	imageset:defineImage("Image", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	_Resources.Imagesets.Image = imageset

	_Ctrls.Image.Ctrl = CEGUIWinMgr:getWindow(_Ctrls.Image.Name)
	_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.ImageEmpty image:Image")
	_Ctrls.Image.Ctrl:subscribeEvent("MouseEnter",       _ImageEditorImageMouseEnter)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseLeave",       _ImageEditorImageMouseLeave)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseMove",        _ImageEditorImageMouseMove)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseWheel",       _ImageEditorImageMouseWhell)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseButtonDown",  _ImageEditorImageMouseButton)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseButtonUp",    _ImageEditorImageMouseButton)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseClick",       _ImageEditorImageMouseClick)
	_Ctrls.Image.Ctrl:subscribeEvent("MouseDoubleClick", _ImageEditorImageMouseDoubleClick)
	
	Helper.GUI.initButtons("", _Ctrls.Buttons, _ImageEditorClick)

	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _ImageEditorClick)

	_Ctrls.ScaleScroll.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow(_Ctrls.ScaleScroll.Name), "CEGUI::Scrollbar")
	_Ctrls.ScaleScroll.Ctrl:setDocumentSize(IMAGE_EDITOR_VIEW_SCROLL_IDENT * 2. + 1.)
	_Ctrls.ScaleScroll.Ctrl:setPageSize(IMAGE_EDITOR_VIEW_SCROLL_STEP)
	_Ctrls.ScaleScroll.Ctrl:setStepSize(IMAGE_EDITOR_VIEW_SCROLL_STEP)
	_Ctrls.ScaleScroll.Ctrl:subscribeEvent("ScrollPosChanged", _ImageEditorScrollPosChanged)
	_Ctrls.ScaleScroll.Ctrl:setScrollPosition(IMAGE_EDITOR_VIEW_SCROLL_IDENT)

	_ImageEditorSetState(IMAGE_EDITOR_STATE_WINDOWED)
	_Ctrls.Frame.Ctrl:setProperty("UnifiedMinSize", "{{0.0,600},{0.0,400}}")

	_Ctrls.AlphaCheckbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow(_Ctrls.AlphaCheckbox.Name), "CEGUI::Checkbox")
	_Ctrls.AlphaCheckbox.Ctrl:subscribeEvent("CheckStateChanged", _ImageEditorCheckStateChanged)

	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)
end

local function _ImageEditorSetImage(image_name, save_file_name)

	if not _Ctrls.Root.Ctrl:isVisible() then
		return false
	end	
	_Ctrls.Image.Ctrl:setProperty("Image", "set:Helper.ImageEditor.ImageEmpty image:Image")
	_ImageSize:set(256, 256)

	_ImageEditorImageName = nil
	_ImageEditorImageSaveFileName = save_file_name

	local my_tex = nil
	local image = MyImgLib:findImage(image_name)

	if image == nil then
		LOG_WARN("Can't find image '"..tostring(image_name).."'")
		return false
	end	

	my_tex = MyDriver:findTexture(image_name, image)
	if my_tex ~= nil then
		MyDriver:removeTexture(my_tex)
		my_tex = MyDriver:findTexture(image_name, image)
	end
	if my_tex ~= nil then
		LOG_ERR("Can not individualy access to the texture '"..image_name.."'")
		return false
	end

	my_tex = MyDriver:addTexture(image_name, image)

	_ImageEditorSetTexture(my_tex)

	if my_tex == nil then
		LOG_WARN("Can't find image '"..tostring(image_name).."'")
	end

	return true
end
Helper.GUI.ImageEditor.setImage = _ImageEditorSetImage

local function _ImageEditorShow(
	mode,
	flags,
	caption,
	image_name,
	file_open_callback,
	file_reload_callback,
	file_save_callback,
	close_callback,
	user_data)
	
	if not _HelperInit() then
		return false
	end
	
	_ImageEditorCaption = caption

	_ImageEditorFlags = flags

	_Ctrls.AlphaCheckbox.Ctrl:setSelected(bit.band(_ImageEditorFlags, IMAGE_EDITOR_FLAGS.USE_IMAGE_ALPHA) ~= 0)

	_Callbacks.Open		= file_open_callback
	_Callbacks.Reload	= file_reload_callback
	_Callbacks.Save		= file_save_callback
	_Callbacks.Close	= close_callback
	
	_ImageEditorSetFileName(nil) -- to release old texture
	if image_name ~= nil then
		_ImageEditorSetFileName(image_name)
	end

	_ImageEditorUserData = user_data

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()

	return true
end
Helper.GUI.ImageEditor.show = _ImageEditorShow
