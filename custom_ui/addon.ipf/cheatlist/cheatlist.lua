
cheatFrame = nil;

function CHEATLIST_ON_INIT(addon, frame)
	cheatFrame = frame;
end

function CHEATLIST_DBG ()
	local dbgmsg = GET_CHILD_RECURSIVELY(cheatFrame, 'dbgmsg', 'ui::CRichText');
	dbgmsg:SetText("{@st41b}".. "DEBUG TEST")
end

function CHEATLIST_UI_CLOSE(frame, obj, argStr, argNum)
	ui.CloseFrame('cheatlist');
end

