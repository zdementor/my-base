
function _HelperCopyToClipboard()
	local count = 0
	local windows = CEGUIWinMgr:getIterator()
	windows:toStart()
	while windows:isAtEnd() == false do
		local w = windows:value()
		local eb = nil
		if w:testClassName("Editbox") then
			eb = tolua.cast(w, "CEGUI::Editbox")
		elseif w:testClassName("MultiLineEditbox") then
			eb = tolua.cast(w, "CEGUI::MultiLineEditbox")
		elseif w:testClassName("Combobox") then
			eb = tolua.cast(w, "CEGUI::Combobox")
		end
		if eb ~= nil then
			if eb:hasInputFocus() then
				os.copyToClipboard(string.sub(eb:getText(),
					eb:getSelectionStartIndex() + 1,
					eb:getSelectionEndIndex()))
				count = count + 1
			end
		end
		windows:next()
	end
	if count > 0 then
		return true
	end
	return false
end
Helper.copyToClipboard = _HelperCopyToClipboard

function _HelperPasteFromClipboard()
	local count = 0
	local windows = CEGUIWinMgr:getIterator()
	windows:toStart()
	while windows:isAtEnd() == false do
		local w = windows:value()
		local eb = nil
		if w:testClassName("Editbox") then
			eb = tolua.cast(w, "CEGUI::Editbox")
		elseif w:testClassName("MultiLineEditbox") then
			eb = tolua.cast(w, "CEGUI::MultiLineEditbox")
		elseif w:testClassName("Combobox") then
			eb = tolua.cast(w, "CEGUI::Combobox")
		end
		if eb ~= nil then
			if eb:hasInputFocus() then
				local start = eb:getSelectionStartIndex()
				local clipBoardText = os.getTextFromClipboard()
				eb:insertText(clipBoardText, start)
				eb:setCaratIndex(start + string.len(clipBoardText))
				count = count + 1
			end
		end
		windows:next()
	end
	if count > 0 then
		return true
	end
	return false
end
Helper.pasteFromClipboard = _HelperPasteFromClipboard

function _HelperGetClampedValue(value, minval, maxval)
	if value < minval then
		return minval
	elseif value > maxval then
		return maxval		
	end
	return value
end
Helper.getClampedValue = _HelperGetClampedValue

function _HelperGetRelFileName(fullfname, basepath)
	local fname = core.extractFileName(fullfname)
	local fpath = core.extractFilePath(fullfname);
	local relpath = core.getRelativePathBetweenDirectories(basepath, fpath)
	return relpath..fname
end
Helper.getRelFileName = _HelperGetRelFileName

local function _HelperGetPassLayerIndex(pass, layer)
	local units_max = MyDriver:getMaximalTextureUnitsAmount()
	local idx = 0
	for i = 0, units_max - 1 do
		if core.isPtrsEq(layer, pass.Layers[i]) then
			idx = i
			break
		end
	end
	return idx
end
Helper.getPassLayerIndex = _HelperGetPassLayerIndex

local function _HelperGetMaterialPassIndex(material, pass)
	local pass_cnt = material:getPassesCount()
	local idx = 0
	for i = 0, pass_cnt - 1 do
		if core.isPtrsEq(pass, material:getPass(i)) then
			idx = i
			break
		end
	end
	return idx
end
Helper.getMaterialPassIndex = _HelperGetMaterialPassIndex

local function _HelperGetSelectedMaterial()
	local sel_nodes = MyScnMgr:getSelectedSceneNodesCount()
	local scene_node = nil
	if sel_nodes >= 1 then
		scene_node = MyScnMgr:getSelectedSceneNode(0)
	end
	if scene_node == nil then
		return nil
	end
	local material = scene_node:getMaterial(0)
	return material
end
Helper.getSelectedMaterial = _HelperGetSelectedMaterial

local function _HelperGetSceneNodeFileText(scene_node, scene_only)
	local file_name = scene_node:getFileName()
	local text = ""
	local game_node = scene_node:getGameNode()
	local core_props =
	{
		scene_props = {
			tag = scn.getSceneNodeCorePropertiesTagValue(),
			xml = scene_node:getSceneCorePropertiesXMLString(),},
		game_props = {
			tag = game.getGameNodeCorePropertiesTagValue(),
			xml = nil,},
		dynamic_props = {
			tag = dyn.getDynamicObjectCorePropertiesTagValue(),
			xml = nil,},
	}
	if game_node ~= nil then
		core_props.game_props.xml = game_node:getGameCorePropertiesXMLString()
		core_props.dynamic_props.xml = game_node:getDynamicCorePropertiesXMLString()
	end
	local file = nil
	if scene_only == nil then
		scene_only = false
	end
	if not scene_only then
		if file_ext == "xml" then
			local file_ext = core.extractFileExt(file_name)
			scene_only = true
		end
	end	
	if not scene_only then
		file = io.open (file_name, "r")
	end
	if file ~= nil then
		text = file:read("*a")
		file:close()
	else
		text = text.."<?xml version=\"1.0\"?>\n"
		text = text.."<!--Saved at "..os.getCurrentTimeString().."-->\n"
		text = text.."\n"
		text = text.."<Scene scene_node_type=\""..
			scn.getSceneNodeTypeName(scene_node:getSceneNodeType()).."\" />\n"
		text = text.."<"..core_props.scene_props.tag..">\n"
		text = text.."</"..core_props.scene_props.tag..">\n"
		text = text.."</Scene>\n"
		text = text.."\n"
		if game_node ~= nil then
			text = text.."<Game game_node_type=\""..
				game.getGameNodeTypeName(game_node:getGameNodeType()).."\" "..
				"class_name=\""..game_node:getGameClassName().."\" />\n"
			text = text.."\n"
			text = text.."<"..core_props.game_props.tag..">\n"
			text = text.."</"..core_props.game_props.tag..">\n"
			text = text.."\n"
			text = text.."</Game>\n"
			text = text.."\n"
		end
	end
	for key, value in pairs(core_props) do
		local start_mask = "<"..value.tag..">"
		local end_mask = "</"..value.tag..">"
		local start_mask_len = string.len(start_mask)
		local end_mask_len = string.len(end_mask)
		local start_tag = string.find(text, start_mask)
		local end_tag = string.find(text, end_mask)
		if start_tag ~= nil and end_tag ~= nil then
			start_tag = start_tag + start_mask_len
			end_tag = end_tag
			if end_tag > start_tag then
				local text_left = string.sub(text, 1, start_tag)
				local text_core = value.xml
				local text_right = string.sub(text, end_tag, string.len(text))
				text = text_left..text_core..text_right
			end
		end
	end
	return text
end
Helper.getSceneNodeFileText = _HelperGetSceneNodeFileText

local function _HelperMakeScreenShot()
	LOG_INFO("Make screenshot...")
	local image = MyDriver:makeScreenShotImage()
	if image ~= nil then
		local scrPath = MyResMgr:getMediaDirFull(res.EMT_TEMP_DATA).."screenshots/"
		if not os.isdir(scrPath) then
			os.mkdir(scrPath)
		end
		local scrName = nil
		local maxScr = 1000
		for i = 0, maxScr-1 do
			local fname = scrPath..string.format("screenshot_%03d.jpg", i)
			if not os.isfile(fname) then
				scrName = fname
				break
			end
		end
		if scrName ~= nil then
			MyImgLib:saveImageToFile(image, scrName)
		else
			LOG_WARN("Can not save screenshot to "
				..scrPath.." (directory full, max screenshots count is "..maxScr..").")
		end
		MyImgLib:removeImage(image)
	end
end
Helper.makeScreenShot = _HelperMakeScreenShot
