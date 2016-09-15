
local function _HelperFreeCEGUIResources(res_tbl)
	for texKey, texVal in pairs(res_tbl.Textures) do
		res_tbl.Textures[texKey] = nil
	end
	for imgsetKey, imgsetVal in pairs(res_tbl.Imagesets) do
		if imgsetVal ~= nil then
			CEGUIImgsetMgr:destroy(imgsetVal)
		end
		res_tbl.Imagesets[imgsetKey] = nil
	end
end
Helper.GUI.freeCEGUIResources = _HelperFreeCEGUIResources

function _HelperInitButtons(name_prefix, buttons, click_handler)
	if name_prefix == nil then
		name_prefix = ""
	end
	for key, value in pairs(buttons) do
		if value.Name == nil then
			value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
				name_prefix..key), "CEGUI::ButtonBase")
		else
			value.Ctrl = tolua.cast(CEGUIWinMgr:getWindow(
				name_prefix..value.Name), "CEGUI::ButtonBase")
		end
		if value.ImageName ~= nil then
			value.Ctrl:setProperty("NormalImage",	value.ImageName)
			value.Ctrl:setProperty("HoverImage",	value.ImageName)
			value.Ctrl:setProperty("PushedImage",	value.ImageName)
			value.Ctrl:setText("")
		end
		value.Ctrl:subscribeEvent("Clicked", click_handler)
	end
end
Helper.GUI.initButtons = _HelperInitButtons

function _HelperInitImages(name_prefix, images, empty_color)
	if name_prefix == nil then
		name_prefix = ""
	end
	for key, value in pairs(images) do
		local name = nil
		if value.Name == nil then
			name = name_prefix..key
		else
			name = name_prefix..value.Name
		end
		value.Ctrl = CEGUIWinMgr:getWindow(name)
		
		local cegui_tex = nil
		local imageset = nil
		
		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, empty_color)
		value.ImageEmpty = cegui_tex
		imageset = CEGUIImgsetMgr:create(name.."Empty", cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.ImageSetEmpty = imageset

		cegui_tex = CEGUIRenderer:createTexture(CEGUI.Size(1, 1))
		Helper.GUI.fillCEGUITextureWithColor(cegui_tex, empty_color)
		value.Image = cegui_tex
		imageset = CEGUIImgsetMgr:create(name, cegui_tex)
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
		value.ImageSet = imageset
		
		value.Ctrl:setProperty("Image", "set:"..value.ImageSetEmpty:getName().." image:ColorImage")
	end
end
Helper.GUI.initImages = _HelperInitImages

function _HelperSetImageTexture(image, texture)
	local imageset = image.ImageSetEmpty
	local cegui_tex = image.Image
	MyCEGUI.setTexture(cegui_tex, texture, false)
	if texture ~= nil then
		imageset = image.ImageSet
		imageset:undefineAllImages()
		imageset:defineImage("ColorImage", CEGUI.Vector2(0,0), cegui_tex:getSize(), CEGUI.Vector2(0, 0));
	end
	image.Ctrl:setProperty(
		"Image", "set:"..imageset:getName().." image:ColorImage")
end
Helper.GUI.setImageTexture = _HelperSetImageTexture
	
function _HelperSetButtonImage(button, image_name)
	button:setProperty("NormalImage",	image_name)
	button:setProperty("HoverImage",	image_name)
	button:setProperty("PushedImage",	image_name)
	button:setText("")
end
Helper.GUI.setButtonImage = _HelperSetButtonImage

function _HelperSetCenteredWindowPosition(window)
	local area = window:getArea()
	local pos_x = nil
	local pos_y = nil
	if area.min.x.offset == 0.0 and area.min.y.offset == 0.0 and
			area.max.x.offset == 0.0 and area.max.y.offset == 0.0 then
		local win_width = area.max.x.scale - area.min.x.scale
		local win_height = area.max.y.scale - area.min.y.scale
		pos_x = CEGUI.UDim((1.0 - win_width) / 2.0, 0.0)
		pos_y = CEGUI.UDim((1.0 - win_height) / 2.0, 0.0)
	else
		local scr_dim = MyDriver:getScreenSize()
		local win_width = area.max.x.offset - area.min.x.offset
		local win_height = area.max.y.offset - area.min.y.offset
		pos_x = CEGUI.UDim(0.0, (scr_dim.Width - win_width) / 2.0)
		pos_y = CEGUI.UDim(0.0, (scr_dim.Height - win_height) / 2.0)
	end
	window:setXPosition(pos_x)
	window:setYPosition(pos_y)	
end
Helper.GUI.setCenteredWindowPosition = _HelperSetCenteredWindowPosition

function _HelperAddComboboxItem(combobox, scheme_name, name, id, data)
	local new_item = CEGUI.createListboxTextItem(name, id, data, false, true)
	new_item:setSelectionBrushImage(scheme_name, "MultiLineEditboxSelectionBrush")
	new_item:setTextColours(CEGUI.colour(1.0, 1.0, 1.0, 1.0))
	combobox:addItem(new_item)
end
Helper.GUI.addComboboxItem = _HelperAddComboboxItem

function _HelperAddListboxItem(listbox, scheme_name, name, id, data)
	local new_item = CEGUI.createListboxTextItem(name, id, data, false, true)
	new_item:setSelectionBrushImage(scheme_name, "MultiLineEditboxSelectionBrush")
	new_item:setTextColours(CEGUI.colour(1.0, 1.0, 1.0, 1.0))
	listbox:addItem(new_item)
end
Helper.GUI.addListboxItem = _HelperAddListboxItem

function _HelperSetMultiColumnListHeader(multicolumnlist, flags, header_items)
	local col_cnt = multicolumnlist:getColumnCount()
	for i = 0, col_cnt - 1 do
		multicolumnlist:removeColumn(col_cnt - i - 1)
	end
	col_cnt = 0
	for k, v in pairs(header_items) do
		multicolumnlist:addColumn(v.Name, col_cnt, v.Dim)
		col_cnt = col_cnt + 1
	end
	multicolumnlist:setSelectionMode(RowSingle)
	local listHeader = multicolumnlist:getListHeader()
	listHeader:setClippedByParent(true)
	listHeader:setSortingEnabled(bit.band(flags, MULTICOLUMN_FLAGS.SORTING_ENABLED) ~= 0)
	for i = 0, listHeader:getColumnCount() - 1 do
		local headerSegment = multicolumnlist:getHeaderSegmentForColumn(i)
		headerSegment:setSizingEnabled(bit.band(flags, MULTICOLUMN_FLAGS.SIZING_DISABLED) == 0)
	end
	multicolumnlist:getHeaderSegmentForColumn(0):setSortDirection(CEGUI.ListHeaderSegment.Descending)
end
Helper.GUI.setMultiColumnListHeader = _HelperSetMultiColumnListHeader
	
function _HelperAddMultiColumnListItem(multicolumnlist, scheme_name, row_items)
	local row = multicolumnlist:getRowCount()
	multicolumnlist:addRow()
	local col = 0
	for k, v in pairs(row_items) do
		local id = -1
		if v.ID ~= nil then
			id = v.ID
		end
		local new_item = CEGUI.createListboxTextItem("", id, v.Data, false, true)
		MyCEGUI.setListboxItemTextUtf8(new_item, core.AnsiToUtf8(v.Name))
		new_item:setSelectionBrushImage(scheme_name, "MultiLineEditboxSelectionBrush")
		if v.Color ~= nil then
			new_item:setTextColours(v.Color)
		else
			new_item:setTextColours(CEGUI.colour(1.0, 1.0, 1.0, 1.0))
		end
		multicolumnlist:setItem(new_item, col, row)
		col = col + 1
	end
end
Helper.GUI.addMultiColumnListItem = _HelperAddMultiColumnListItem

function _HelperFillCEGUITextureWithColor(cegui_tex, color)
	local my_tex = MyCEGUI.getTexture(cegui_tex)
	local image = my_tex:lock()
	image:fill(color)
	my_tex:unlock()
end
Helper.GUI.fillCEGUITextureWithColor = _HelperFillCEGUITextureWithColor

function _HelperSetupCurrentScreenResolution()
	local scr_w = MyDriver:getScreenSize().Width
	local scr_h = MyDriver:getScreenSize().Height
	if scr_w > 1280 then
		scr_w = 1280
	end
	if scr_h > 1024 then
		scr_h = 1024
	end
	local scr_sz = CEGUI.Size(scr_w, scr_h)
	local fonts = CEGUIFontMgr:getIterator()
	fonts:toStart()
	while fonts:isAtEnd() == false do
		fonts:value():setNativeResolution(scr_sz)
		fonts:next()
	end
	local imagesets = CEGUIImgsetMgr:getIterator()
	imagesets:toStart()
	while imagesets:isAtEnd() == false do
		imagesets:value():setNativeResolution(scr_sz)
		imagesets:next()
	end
end
Helper.GUI.setupCurrentScreenResolution = _HelperSetupCurrentScreenResolution