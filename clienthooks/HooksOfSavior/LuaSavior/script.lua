function hookOfSavior(actor)
	local name = actor:GetName();
	print ("actorName = " .. name);
	
	local brk = GetBarrackSystem(actor);
	local key = name;
	key = brk:GetCID();
	
	print ("CID = " .. key);
	
	local bpc = barrack.GetBarrackPCInfoByCID(key);
end