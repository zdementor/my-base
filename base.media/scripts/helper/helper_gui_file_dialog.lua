
local _FileDialogMode = FILE_DIALOG_MODE.OPEN
local _FileDialogStartDir = ""
local _FileDialogWorkDir = ""
local _FileDialogFilter = {}
local _FileDialogFlags = 0
local _FileDialogFilesList = {}
local _FileDialogFilesListNames = {}
local _FileDialogAcceptCallback = nil
local _FileDialogUserData = nil

local _Ctrls =
{
	Root = {},
	Frame = {},
	Buttons =
	{
		CancelButton	= {Ctrl = nil, ImageName = nil},
		AcceptButton	= {Ctrl = nil, ImageName = nil},
		RefreshButton	= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.view-refresh"	},
		GoUpButton		= {Ctrl = nil, ImageName = "set:TangoGUIIcons image:actions.go-up"			},
	},
	FileListbox = {},
	FileNameEditbox = {},
	FileTypeCombobox = {},
	PathStaticText = {},
}

local function _FileDialogUpdate(updateFileList)
	local typeStr = ""
	local baseWorkDir = MyFileSys:getWorkingDirectory()
	_FileDialogWorkDir = string.format(
		MyFileSys:optimizeDirectoryString(_FileDialogWorkDir))
	MyFileSys:changeWorkingDirectoryTo(_FileDialogWorkDir)
	_Ctrls.FileListbox.Ctrl:resetList()
	local fileList = MyFileSys:getFileList()
	if updateFileList then
		fileList:update()
	end
	local filterIdx = _Ctrls.FileTypeCombobox.Ctrl:getItemIndex(
		_Ctrls.FileTypeCombobox.Ctrl:getSelectedItem())
	_FileDialogFilesList = {}
	_FileDialogFilesListNames = {}
	local fileIdx = 0
	for i = 0, fileList:getFileCount() - 1 do
		local fileExt = string.lower(fileList:getFileExt(i))
		local fileName = fileList:getFileName(i)
		local filtered = false
		if string.len(fileName) <= 0 then
			filtered = false
		elseif string.sub(fileName, 1, 1) == "." then
			filtered = false
		elseif fileList:isDirectory(i) then
			fileName = fileName.."/"
			filtered = true
		elseif filterIdx == 0 then
			if _FileDialogFilter[fileExt] ~= nil then
				filtered = true
			end
		elseif _FileDialogFilter[filterIdx].Value == fileExt then
			filtered = true
		end
		if filtered then
			local color = CEGUI.colour(1.0, 1.0, 0.5, 1.0)
			local type_str = "Dir"
			local size_str = ""
			if not fileList:isDirectory(i) then
				type_str = "File"
				color:set(1.0, 1.0, 1.0, 1.0)
				local fileSize = fileList:getFileSizeInBytes(i) / (1024.0 * 1024.0)
				if fileSize > 1.0 then
					size_str = string.format("%.1f MB", fileSize)
				else
					fileSize = fileList:getFileSizeInBytes(i) / (1024.0)
					if fileSize > 1.0 then
						size_str = string.format("%.1f KB", fileSize)
					else
						size_str = string.format("%d B", fileList:getFileSizeInBytes(i))
					end
				end
			end
			Helper.GUI.addMultiColumnListItem(
				_Ctrls.FileListbox.Ctrl,
				OPTIONS.HelperGUI.SchemeName, {
					[1] = {Name = fileName, ID = fileIdx, Data = nil, Color = color},
					[2] = {Name = type_str, ID = fileIdx, Data = nil, Color = color},
					[3] = {Name = size_str, ID = fileIdx, Data = nil, Color = color},})
			_FileDialogFilesList[fileIdx + 1] = {Name = fileName, Dir = fileList:isDirectory(i)}
			_FileDialogFilesListNames[fileName] = fileIdx
			fileIdx = fileIdx + 1
		end
	end
	MyCEGUI.setWindowTextUtf8(
		_Ctrls.PathStaticText.Ctrl, core.AnsiToUtf8(MyFileSys:getWorkingDirectory()))
	MyFileSys:changeWorkingDirectoryTo(baseWorkDir)
end	

local function _FileDialogClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.FileDialog.Frame" or
		name ==  "Helper.FileDialog.CancelButton" then
		_Ctrls.Root.Ctrl:setVisible(false)
	elseif name == "Helper.FileDialog.AcceptButton" then
		local filename_utf8 = _Ctrls.FileNameEditbox.Ctrl:getText()
		if string.len(filename_utf8) > 0 then
			local rel_path = core.getRelativePathBetweenDirectories(
				_FileDialogStartDir, _FileDialogWorkDir)
			local file_name_ansi = core.Utf8ToAnsi(filename_utf8)
			local file_ext = string.lower(core.extractFileExt(file_name_ansi))
			local filter_idx = _Ctrls.FileTypeCombobox.Ctrl:getItemIndex(
				_Ctrls.FileTypeCombobox.Ctrl:getSelectedItem())
			local filtered = false
			if filter_idx == 0 then
				if _FileDialogFilter[file_ext] ~= nil then
					filtered = true
				end
			elseif _FileDialogFilter[filter_idx].Value == file_ext then
				filtered = true
			end
			if filtered and _FileDialogAcceptCallback ~= nil then
				_Ctrls.Root.Ctrl:setVisible(false)
				_FileDialogAcceptCallback(
					_FileDialogWorkDir..file_name_ansi,
					rel_path..file_name_ansi,
					_FileDialogUserData)
			end
		end
	elseif name == "Helper.FileDialog.GoUpButton" then
		if _FileDialogMode == FILE_DIALOG_MODE.OPEN then
			_Ctrls.FileNameEditbox.Ctrl:setProperty("Text", "")
		end
		_FileDialogWorkDir = _FileDialogWorkDir.."../"
		_FileDialogUpdate()
	elseif name == "Helper.FileDialog.RefreshButton" then
		_FileDialogUpdate(true)
	end
end

local function _FileDialogMouseDoubleClick(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.FileDialog.FileListbox" then
		local item = _Ctrls.FileListbox.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local fileData = _FileDialogFilesList[item:getID() + 1]
			if fileData.Dir then
				if _FileDialogMode == FILE_DIALOG_MODE.OPEN then
					_Ctrls.FileNameEditbox.Ctrl:setProperty("Text", "")
				end
				_FileDialogWorkDir = _FileDialogWorkDir..fileData.Name
				_FileDialogUpdate()
			end
		end
	end
end

local function _FileDialogSelectionChanged(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.FileDialog.FileListbox" then
		local item = _Ctrls.FileListbox.Ctrl:getFirstSelectedItem()
		if item ~= nil then
			local fileData = _FileDialogFilesList[item:getID() + 1]
			if not fileData.Dir then
				MyCEGUI.setWindowTextUtf8(
					_Ctrls.FileNameEditbox.Ctrl, core.AnsiToUtf8(fileData.Name))
			else
				if _FileDialogMode == FILE_DIALOG_MODE.OPEN then
					_Ctrls.FileNameEditbox.Ctrl:setProperty("Text", "")
				end
			end
		end
	end
end

local function _FileDialogSelectionAccepted(args)
	local we = CEGUI.toWindowEventArgs(args)
	local name = we.window:getName()
	if name == "Helper.FileDialog.FileTypeCombobox" then
		_FileDialogUpdate()
	end
end

function _FileDialogInit()
	
	_Ctrls.Root.Ctrl = CEGUIWinMgr:loadWindowLayout("myengine_helper_filedialog.layout", "", "CommonLayouts")
	_Ctrls.Root.Ctrl:setVisible(false)
	CEGUISystem:getGUISheet():addChildWindow(_Ctrls.Root.Ctrl)
	
	_Ctrls.Frame.Ctrl = CEGUIWinMgr:getWindow("Helper.FileDialog.Frame")
	
	_Ctrls.FileListbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.FileDialog.FileListbox"), "CEGUI::MultiColumnList")
	_Ctrls.FileTypeCombobox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.FileDialog.FileTypeCombobox"), "CEGUI::Combobox")
	_Ctrls.FileNameEditbox.Ctrl = tolua.cast(
		CEGUIWinMgr:getWindow("Helper.FileDialog.FileNameEditbox"), "CEGUI::Editbox")
	_Ctrls.PathStaticText.Ctrl =
		CEGUIWinMgr:getWindow("Helper.FileDialog.PathStaticText")
	
	Helper.GUI.initButtons("Helper.FileDialog.",
		_Ctrls.Buttons, _FileDialogClick)

	Helper.GUI.setMultiColumnListHeader(
		_Ctrls.FileListbox.Ctrl, 0, {
			[1] = {Name = "Name", Dim = CEGUI.UDim(0.60, 0)},
			[2] = {Name = "Type", Dim = CEGUI.UDim(0.15, 0)},
			[3] = {Name = "Size", Dim = CEGUI.UDim(0.20, 0)},
			})

	_Ctrls.FileListbox.Ctrl:subscribeEvent("MouseDoubleClick",	_FileDialogMouseDoubleClick)
	_Ctrls.FileListbox.Ctrl:subscribeEvent("SelectionChanged",	_FileDialogSelectionChanged)
	_Ctrls.Frame.Ctrl:subscribeEvent("CloseClicked", _FileDialogClick)
	_Ctrls.FileTypeCombobox.Ctrl:subscribeEvent("ListSelectionAccepted", _FileDialogSelectionAccepted)
		
	Helper.GUI.setCenteredWindowPosition(_Ctrls.Frame.Ctrl)

end

local function _FileDialogShow(
	mode, flags, caption, start_dir, default_file, files_filter, accept_callback, user_data)
	
	if not _HelperInit() then
		return false
	end
	
	_FileDialogMode = mode
	_FileDialogStartDir = start_dir
	_FileDialogWorkDir = start_dir
	_FileDialogFilter = files_filter
	_FileDialogFlags = flags
	
	_Ctrls.FileNameEditbox.Ctrl:setText("")

	_Ctrls.FileTypeCombobox.Ctrl:resetList()
	local allFiles = "All files ("
	for i = 1, table.getn(_FileDialogFilter) do
		if i > 1 then
			allFiles = allFiles.."|"
		end
		allFiles = allFiles.."*.".._FileDialogFilter[i].Value
	end
	allFiles = allFiles..")"
	Helper.GUI.addListboxItem(_Ctrls.FileTypeCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName, allFiles, 0, nil)
	for i = 1, table.getn(_FileDialogFilter) do
		Helper.GUI.addListboxItem(_Ctrls.FileTypeCombobox.Ctrl, OPTIONS.HelperGUI.SchemeName,
			_FileDialogFilter[i].Descr.." (*.".._FileDialogFilter[i].Value..")", i, nil)
	end
	_Ctrls.FileTypeCombobox.Ctrl:setItemSelectState(0, true)
	
	_FileDialogUpdate()
	
	_Ctrls.Frame.Ctrl:setProperty("Text", caption)
	if _FileDialogMode == FILE_DIALOG_MODE.NEW then
		_Ctrls.Buttons.AcceptButton.Ctrl:setProperty("Text", "OK")
		_Ctrls.FileNameEditbox.Ctrl:setProperty("ReadOnly", "false")
	elseif _FileDialogMode == FILE_DIALOG_MODE.SAVE then
		_Ctrls.Buttons.AcceptButton.Ctrl:setProperty("Text", "Save")
		_Ctrls.FileNameEditbox.Ctrl:setProperty("ReadOnly", "false")
	elseif _FileDialogMode == FILE_DIALOG_MODE.OPEN then
		_Ctrls.Buttons.AcceptButton.Ctrl:setProperty("Text", "Open")
		_Ctrls.FileNameEditbox.Ctrl:setProperty("ReadOnly", "true")
	end
	
	if default_file ~= nil then	
		local file_name = core.extractFileName(default_file)
		_Ctrls.FileNameEditbox.Ctrl:setText(file_name)
		local file_idx = _FileDialogFilesListNames[file_name]
		if file_idx ~= nil then
			_Ctrls.FileListbox.Ctrl:setItemSelectState(CEGUI.MCLGridRef(file_idx, 0), true)
		end
		
	else
		_Ctrls.FileNameEditbox.Ctrl:setText("")
	end
	
	_FileDialogAcceptCallback = accept_callback
	_FileDialogUserData = user_data

	_Ctrls.Root.Ctrl:setVisible(true)
	_Ctrls.Root.Ctrl:moveToFront()
	
	return true
end
Helper.GUI.FileDialog.show = _FileDialogShow
