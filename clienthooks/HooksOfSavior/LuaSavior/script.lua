-- http://www.troubleshooters.com/codecorn/lua/lua_c_calls_lua.htm

account = session.barrack.GetMyAccount();

print ("BPC ID = 1"); -- Get the first slot in the commander list
bpc = account:GetBySlot(1);

actorPC = bpc:GetApc();
print ("actorPC.mapID = " .. actorPC.mapID);
print ("actorPC.channelID = " .. actorPC.channelID);