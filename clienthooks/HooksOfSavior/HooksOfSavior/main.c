#include "HookEngine/HookEngine.h"
#include "Utils/Utils.h"
#include "Win32Tools/Win32Tools.h"
#include "PacketType.h"

#define __DEBUG_OBJECT__ "ToSClient"
#include "dbg/dbg.h"

void dbgBuffer (BYTE *buffer, int bufferSize);


/** =================== HOOKS ================= */
void __cdecl logDebug_1 (int a1, LPCSTR lpOutputString, char *message)
{
	#define OFFSET_logDebug_1 (0xFB2920 - 0x400000)
	void (__cdecl *hooked) (int, LPCSTR, char *) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) logDebug_1);

	char buffer[4096];
	sprintf (buffer, message);

	dbg ("%s", buffer);

	return hooked (a1, lpOutputString, message);
}

int __thiscall unknown__createCmd (int this, unsigned __int8 *Src)
{
	#define OFFSET_createCmd (0x103B6D0 - 0x400000)
	int (__thiscall *hooked) (int this, unsigned __int8 *Src) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) unknown__createCmd);

    /*
    static char *lastCmd = NULL;
    if (lastCmd == NULL) lastCmd = strdup(Src);

    if (strcmp(lastCmd, Src)) {
        if (!strstr(Src, "F_bg_")) {
            free (lastCmd);
            dbg ("Command <%s> created.", Src);
            lastCmd = strdup(Src);
        }
    }
    */

    return hooked (this, Src);
}

void logDebug_2 (const char *Format, ...)
{
	#define OFFSET_logDebug_2 (0xFB2800 - 0x400000)
	char OutputString[24096];
	va_list va;

	va_start(va, Format);
	vsprintf(OutputString, Format, va);

	dbg ("%s", OutputString);
}

bool __thiscall ClientNet__GetPacket (void *clientNet, BYTE *outPacket, DWORD *outSize, DWORD *outType)
{
    #define OFFSET_GetPacket (0x6FCA80 - 0x400000)
    bool (__thiscall *hooked) (void *clientNet, BYTE *outPacket, DWORD *outSize, DWORD *outType) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) ClientNet__GetPacket);

    bool res = hooked (clientNet, outPacket, outSize, outType);

    if (res & 0xFF) {
        WORD type = *outType;
        dbg ("[CLIENT RECEIVE] Packet type : <%s>", PacketType_to_string (type));
        dbgBuffer (outPacket, *outSize);
    }

    return res;
}

int __cdecl imcCrypt__NetEncrypt (signed int plaintextSize, BYTE *plaintextIn, void *ciphertextOut)
{
	#define OFFSET_NetEncrypt (0x0FC0210 - 0x400000)
	int (__cdecl *hooked) (signed int plaintextSize, BYTE *plaintextIn, void *ciphertextOut) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) imcCrypt__NetEncrypt);

    WORD type = *(WORD *) (plaintextIn);
    dbg ("[CLIENT SEND] Packet type : <%s>", PacketType_to_string (type));
    dbgBuffer (plaintextIn, plaintextSize);

    return hooked (plaintextSize, plaintextIn, ciphertextOut);
}


/** ============== UTILS =================== */

void dbgBuffer (BYTE *buffer, int bufferSize)
{
    int curPos = 0;
    char tmpbuf[1024] = {0};
    char hexBuf[4] = {0};

    dbg ("=================================================");
    while (curPos < bufferSize) {
        int offset;
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            sprintf (hexBuf, " %02X", buffer[curPos]);
            strcat (tmpbuf, hexBuf);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                strcat(tmpbuf, "   ");
            }
        }
        strcat (tmpbuf, " | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            unsigned char c = buffer[curPos];
            sprintf (hexBuf, "%c", isprint(c) ? c : '.');
            strcat (tmpbuf, hexBuf);
        }

        dbg ("%s", tmpbuf);
        tmpbuf[0] = 0;
    }
    dbg ("=================================================");
}

/** ============== MAIN ==================== */

void startInjection (void)
{
	char *tosDllPath = get_module_path ("ToSClient.dll");
	dbg_set_output (fopen (str_dup_printf("%s/log.txt", tosDllPath), "w+"));

	if (!HookEngine_new (str_dup_printf ("%s/NtHookEngine.dll", tosDllPath))) {
		MessageBox (NULL, "Error", "HookEngine loading error", 0);
		return;
	}

	DWORD baseAddr = get_baseaddr ("Client_tos.exe");
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_1, (ULONG_PTR) logDebug_1);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_2, (ULONG_PTR) logDebug_2);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_NetEncrypt, (ULONG_PTR) imcCrypt__NetEncrypt);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_GetPacket,  (ULONG_PTR) ClientNet__GetPacket);
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_createCmd,  (ULONG_PTR) unknown__createCmd);
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
