#include "HookEngine/HookEngine.h"
#include "Utils/Utils.h"
#include "Win32Tools/Win32Tools.h"

#define __DEBUG_OBJECT__ "LuaSavior"
#include "dbg/dbg.h"

#include "luajit.h"
#include "lauxlib.h"

// Global Lua handler
lua_State * hLua = NULL;

// Lua loaded functions prototypes
int (* _lua_gettop) ();
int (* _lua_isstring) ();
char * (* _lua_tolstring) ();
int (* _lua_getfield) ();
int (* _luaL_register) ();
int (* _lua_settop) ();
int (* _lua_pcall) ();
int (* _luaL_loadfile) ();
#define _lua_pop(L,n) _lua_settop(L, -(n)-1)
#define _lua_getglobal(L,s) _lua_getfield(L, LUA_GLOBALSINDEX, (s))


// Hook for retrieve the global Lua handler in imcIESThread
typedef struct _imcIESThread {
    byte data[0x984];
    lua_State * luaJITHandler;
} imcIESThread;

char __thiscall imcIESThread__Run (imcIESThread *this, int stringID, int a3)
{
	#define OFFSET_imcIESThread__Run (0xFE2130 - 0x400000)
	char (__thiscall * hooked) (imcIESThread *this, int stringID, int a3) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) imcIESThread__Run);

    hLua = this->luaJITHandler;

    return hooked (this, stringID, a3);
}


// Lua loader
bool loadLua (char *tosDllPath)
{
    HMODULE lua51 = LoadLibrary(str_dup_printf("%s/luajit/lua51.dll", tosDllPath));
    if (!lua51) {
        MessageBox (NULL, "ERROR LOADING DLL", "ERROR", 0);
        return 0;
    }

    #define LuaGPA(x) \
    do { \
        _##x = (void *) GetProcAddress (lua51, STRINGIFY(x)); \
        if (!_##x) { \
            DWORD code = GetLastError(); \
            char *errorCode = get_error_message(code); \
            char *errorMsg = str_dup_printf("ERROR LOADING " STRINGIFY(x) " : %s (%x) !\n", errorCode, code); \
            MessageBox(NULL, errorMsg, "ERROR", 0); return 0; \
        } \
    } while(0);

    LuaGPA (lua_gettop);
    LuaGPA (lua_isstring);
    LuaGPA (lua_tolstring);
    LuaGPA (lua_getfield);
    LuaGPA (luaL_register);
    LuaGPA (lua_settop);
    LuaGPA (lua_pcall);
    LuaGPA (luaL_loadfile);

    // Wait for the handler to be read in the imcIESThread::Run hook
    while (!hLua) {
        Sleep (1);
    }

    return 1;
}

// "print" hook
int Lua_print (lua_State * L)
{
    int nargs = _lua_gettop(L);

    for (int i = 1; i <= nargs; i++)
    {
        if (_lua_isstring(L, i)) {
            char *arg = _lua_tolstring (L, i, NULL);
            dbg ("msg = <%s>", arg);
        }
        else {
            /* Do something with non-strings */
        }
    }

    return 0;
}

void luaSetHooks (lua_State *L)
{
    struct luaL_Reg hookedFunctions [] = {
        {"print", Lua_print},

        {NULL, NULL} /* end of array */
    };


    _lua_getglobal(L, "_G");
    _luaL_register(L, NULL, hookedFunctions);
    _lua_pop(L, 1);
}


/** ============== MAIN ==================== */

void startInjection (void)
{
	char *tosDllPath = get_module_path ("LuaSavior.dll");
	dbg_set_output (fopen (str_dup_printf("%s/log.txt", tosDllPath), "w+"));

	if (!HookEngine_new (str_dup_printf ("%s/NtHookEngine.dll", tosDllPath))) {
		MessageBox (NULL, "Error", "HookEngine loading error", 0);
		return;
	}

	DWORD baseAddr = get_baseaddr ("Client_tos.exe");
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_imcIESThread__Run, (ULONG_PTR) imcIESThread__Run);

    // Load our own luaJIT and retrieve addresses to the functions needed
    if (!loadLua (tosDllPath)) {
        return;
    }

    // Set optionnal hooks
    luaSetHooks (hLua);

    // Load custom script
    if (_luaL_loadfile (hLua, str_dup_printf("%s/script.lua", tosDllPath))) {
        MessageBox (NULL, "Cannot load script.lua", "ERROR", 0);
        return;
    }

    if (_lua_pcall (hLua, 0, 0, 0)) {
        MessageBox (NULL, "Error when loading script.lua", "ERROR", 0);
        return;
    }

}

void endInjection (void)
{
	HookEngine_unhook_all ();
}


bool WINAPI DllMain (HMODULE dll, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) startInjection, NULL, 0, NULL);
		break;

		case DLL_PROCESS_DETACH:
			endInjection ();
		break;
	}

	return true;
}
