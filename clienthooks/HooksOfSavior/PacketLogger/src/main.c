#include "HookEngine/HookEngine.h"
#include "Utils/Utils.h"
#include "Win32Tools/Win32Tools.h"
#include "PacketType.h"
#include "dbg/dbg.h"

// Offsets
#define OFFSET_GetPacket  (0x645DC0 - 0x400000)
#define OFFSET_NetEncrypt (0xCBE530 - 0x400000)

// Globals
char *loggerPath = NULL;
HANDLE mutex;
FILE *defaultOutput = NULL;
bool enablePacketEncryption = false;

/** =================== Utils ================= */
void writePacketToFile (
    char *packetType,
    byte *packet,
    size_t packetSize
) {
    WaitForSingleObject (mutex, INFINITE);
    static char filename[10000];
    sprintf (filename, "%s/packets/%s.txt", loggerPath, packetType);
	FILE *packetFile = fopen (filename, "a+");

    dbg_set_output (packetFile);
    buffer_print (packet, packetSize, NULL);
    dbg_set_output (defaultOutput);
    fclose (packetFile);

    ReleaseMutex (mutex);
}

/** =================== HOOKS ================= */
bool __thiscall ClientNet__GetPacket (void *clientNet, BYTE *outPacket, DWORD *outSize, DWORD *outType)
{
    bool (__thiscall *hooked) (void *clientNet, BYTE *outPacket, DWORD *outSize, DWORD *outType) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) ClientNet__GetPacket);

    bool res = hooked (clientNet, outPacket, outSize, outType);

    if (res & 0xFF) {
        WORD type = *outType;
        char *packetType = PacketType_to_string (type);
        dbg ("[CLIENT RECEIVE] Packet type : <%s>", packetType);
        buffer_print (outPacket, *outSize, NULL);
        writePacketToFile (packetType, outPacket, *outSize);
    }

    return res;
}

int __cdecl imcCrypt__NetEncrypt (size_t plaintextSize, BYTE *plaintextIn, BYTE *ciphertextOut)
{
	int (__cdecl *hooked) (size_t plaintextSize, BYTE *plaintextIn, BYTE *ciphertextOut) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) imcCrypt__NetEncrypt);

    WORD type = *(WORD *) (plaintextIn);
    char *packetType = PacketType_to_string (type);
    dbg ("[CLIENT SEND] Packet type : <%s>", packetType);
    buffer_print (plaintextIn, plaintextSize, NULL);
    writePacketToFile (packetType, plaintextIn, plaintextSize);

    if (enablePacketEncryption) {
        return hooked (plaintextSize, plaintextIn, ciphertextOut);
    } else {
        *(WORD *) ciphertextOut = plaintextSize;
        memcpy (&ciphertextOut[2], plaintextIn, plaintextSize);
        return plaintextSize+2;
    }
}

/** ============== MAIN ==================== */

void startInjection (void)
{
    mutex = CreateMutex (NULL, FALSE, NULL);
	loggerPath = get_module_path ("PacketLogger.dll");
	defaultOutput = fopen (str_dup_printf("%s/log.txt", loggerPath), "w+");

	dbg_set_output (defaultOutput);

	if (!HookEngine_new (str_dup_printf ("%s/NtHookEngine.dll", loggerPath))) {
		MessageBox (NULL, "Error", "HookEngine loading error", 0);
		return;
	}

	DWORD baseAddr = get_baseaddr ("Client_tos.exe");
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_NetEncrypt, (ULONG_PTR) imcCrypt__NetEncrypt);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_GetPacket,  (ULONG_PTR) ClientNet__GetPacket);
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
