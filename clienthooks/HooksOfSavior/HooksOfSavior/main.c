#include "HookEngine/HookEngine.h"
#include "Utils/Utils.h"
#include "Win32Tools/Win32Tools.h"
#include "PacketType.h"

#include "TableNameItemsEnglish.h"

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

typedef struct _StringID
{
    struct _StringID *self;
    DWORD unk2;
    DWORD unk3;
    int stringLen;
    char stringOrPtr[16];

}   StringID;

typedef struct _ItemTable {
    char data[0x230];
}   ItemTable;

typedef struct _imcIESObject {
    char data[0x5C];
    StringID *type;
    DWORD classList;
}   imcIESObject;

typedef struct _IRItem {
    int seed;
    StringID *itemName;
    StringID *className;
    StringID *itemType;
    StringID *GroupName;
    StringID *classType;
    StringID *EqpType;
    StringID *AttackType;
    StringID *Attribute;
    int ItemLv;
    StringID *HandSide;
    char data[0x114];
}   IRItem;


int __thiscall ItemTable__InitItem (ItemTable *this, imcIESObject *object, IRItem *irItem)
{
	#define OFFSET_InitItem (0x0BF1E10 - 0x400000)
	int (__thiscall *hooked) (ItemTable *, imcIESObject *, IRItem *) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) ItemTable__InitItem);

    int res = hooked (this, object, irItem);

    static int curId = 0;
    char * (__thiscall *StringID__getString) (StringID **this) = (void *) 0x0103B740;

    // Korean item name :
    // char *itemName   = StringID__getString (&irItem->itemName);

    int seed         = irItem->seed;
    int ItemLv       = irItem->ItemLv;

    char *className  = StringID__getString (&irItem->className);
    char *itemType   = StringID__getString (&irItem->itemType);
    char *GroupName  = StringID__getString (&irItem->GroupName);
    char *classType  = StringID__getString (&irItem->classType);
    char *EqpType    = StringID__getString (&irItem->EqpType);
    char *AttackType = StringID__getString (&irItem->AttackType);
    char *Attribute  = StringID__getString (&irItem->Attribute);
    char *HandSide   = StringID__getString (&irItem->HandSide);

    dbg ("Seed = 0x%08X | ItemLv = %0.3d | ClassName = <%s> | itemType = <%s> | GroupName = <%s> | classType = <%s> | "
         "EqpType = <%s> | AttackType = <%s> | Attribute = <%s> | HandSide = <%s> | Name = <%s>",
         seed, ItemLv, className, itemType, GroupName, classType, EqpType, AttackType, Attribute, HandSide, tableNameItemsEnglish[curId++]);

    return res;
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
	char *tosDllPath = get_module_path ("HooksOfSavior.dll");
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
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_InitItem,  (ULONG_PTR) ItemTable__InitItem);
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
