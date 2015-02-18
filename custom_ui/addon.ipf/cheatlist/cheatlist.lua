
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

