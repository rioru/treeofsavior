
cheatFrame = nil;
dbgMsgCount = 0;
notDisplayed = {}

function CHEATLIST_ON_INIT(addon, frame)
	cheatFrame = frame;
end

function addMsgConsole (grid, text)
	ATTACH_TEXT_TO_OBJECT(grid, 'DBGMSG_' .. dbgMsgCount, text, 1, (dbgMsgCount * 25), 1000, 25, ui.LEFT, ui.TOP, 1, "left", "top");
	dbgMsgCount = dbgMsgCount + 1;
end

function UI_INFO ()
	local list = session.GetUIInfoList();
	local cnt = list:Count();
	EMUDBG (" --- UI_INFO --- ");
	for i = 0 , cnt - 1 do
		local info = list:Element(i);
		EMUDBG ("=====[UiFrameInfo element n = " .. i .. "]==========");
		EMUDBG ("groupID = " .. info.groupID);
		EMUDBG ("uiindex = " .. info.uiindex);
		EMUDBG ("x/y = " .. info.x .. " / " .. info.y);
		EMUDBG ("width/height = " .. info.width .. " / " .. info.height);
		-- EMUDBG ("title = " .. info.title); -- Not possible to access to "title" attribute directly
		EMUDBG ("numArg1 = " .. info.numArg1);
		EMUDBG ("numArg2 = " .. info.numArg2);
	end
end

function EMUDBG (textDbg)

	if cheatFrame == nil then
		table.insert(notDisplayed, textDbg);
		return;
	end

	local grid = GET_CHILD(cheatFrame, 'r1emu_grid', 'ui::CGrid');
	
	-- Some messages are still waiting to be displayed
	for k,msg in pairs (notDisplayed) do
		addMsgConsole(grid, msg);
	end
	notDisplayed =  {};
	
	addMsgConsole (grid, textDbg);
end

function CHEATLIST_UI_CLOSE(frame, obj, argStr, argNum)
	ui.CloseFrame('cheatlist');
end

