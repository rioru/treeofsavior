
cheatFrame = nil;
dbgMsgCount = 0;

function CHEATLIST_ON_INIT(addon, frame)
	cheatFrame = frame;
end

function EMUDBG (textDbg)
	if cheatFrame == nil then
		return;
	end
	local grid = GET_CHILD(cheatFrame, 'r1emu_grid', 'ui::CGrid');
	ATTACH_TEXT_TO_OBJECT(grid, 'DBGMSG_' .. dbgMsgCount, textDbg, 1, (dbgMsgCount * 35), 1000, 35, ui.LEFT, ui.TOP, 1, "left", "top");
	dbgMsgCount = dbgMsgCount + 1;
end

function CHEATLIST_UI_CLOSE(frame, obj, argStr, argNum)
	ui.CloseFrame('cheatlist');
	cheatFrame = nil;
end

