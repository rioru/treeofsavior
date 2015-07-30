#!/usr/bin/python
"""
Tree of Savior IDAPython Script
Automic rename of Lua related functions
"""

import idaapi
import idc

LuaExtern__declGlobalFunction = 0x0CD9800;

def MakeNameForce (address, name):
    x = 2;
    newName = name;
    while (MakeNameEx (address, newName, SN_NOWARN) == 0):
        newName = "%s_%d" % (name, x);
        x = x + 1;
    return newName;

# Rename all functions declared with LuaExtern__declGlobalFunction
occ = RfirstB (LuaExtern__declGlobalFunction);
while occ != BADADDR:
    routineAddress = Dword (occ - 5 - 4);
    routineName = GetString (Dword (occ - 4));
    occ = RnextB (LuaExtern__declGlobalFunction, occ);
    name = MakeNameForce (routineAddress, routineName);
    SetType (routineAddress, "int __cdecl %s (lua_State * luaState)" % name);