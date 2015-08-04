#pragma once

// Camera
#define OFFSET_SetViewTM      (0x0AEE0A0 - 0x400000) // ICBT3

// Items list
#define OFFSET_convertIESToIR (0x08DDF40 - 0x400000)
#define OFFSET_LuaGetObject   (0x103B6D0 - 0x400000)

// Debug functions
#define OFFSET_logDebug_1     (0x0CB0C40 - 0x400000)
#define OFFSET_logDebug_2     (0x0CA8E80 - 0x400000)

// Packets
#define OFFSET_SendPacket     (0x0659510 - 0x400000) // ICBT3
#define OFFSET_GetPacket      (0x0659910 - 0x400000) // ICBT3
#define OFFSET_NetEncrypt     (0x0CFDC20 - 0x400000) // ICBT3
#define OFFSET_CNetUsr__Recv  (0x0658AF0 - 0x400000) // ICBT3

// Lua
#define OFFSET_lua_gettop     (0x0CD93C0 - 0x400000)