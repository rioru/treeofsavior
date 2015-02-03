#include "HookEngine/HookEngine.h"
#include "Utils/Utils.h"
#include "Win32Tools/Win32Tools.h"
#include "PacketType.h"

#include "TableNameItemsEnglish.h"

#define __DEBUG_OBJECT__ "ToSClient"
#include "dbg/dbg.h"

void dbgBuffer (BYTE *buffer, int bufferSize);


// === StringID ==========================================
typedef struct _StringID
{
    struct _StringID *self;
    DWORD unk2;
    DWORD unk3;
    int stringLen;
    char stringOrPtr[16];

}   StringID;

char * (__thiscall *StringID__getString) (StringID **this) = (void *) 0x0103B740;

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

int __thiscall Lua__LuaCmd (int this, unsigned __int8 *Src)
{
	#define OFFSET_LuaCmd (0x103B6D0 - 0x400000)
	int (__thiscall *hooked) (int this, unsigned __int8 *Src) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) Lua__LuaCmd);

    static char *lastCmd = NULL;
    if (lastCmd == NULL) lastCmd = strdup(Src);

    if (strcmp(lastCmd, Src)) {
        if (!strstr(Src, "F_bg_")) {
            free (lastCmd);
            dbg ("LuaCommand <%s> created.", Src);
            lastCmd = strdup(Src);
        }
    }

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

// ==== [ DTB TABLES ] ====================================================

/*  262 */
#pragma pack(push, 1)
typedef struct _DtbItem
{
    struct _DtbItem *next;
    struct _DtbItem **field_4;
    DWORD schrageId;

    StringID *sid;
} DtbItem;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _DtbTable
{
    int size;
    DtbItem *dtbEmpty;
    int field_8;
    int field_C;
    DtbItem **table;
    void *dwordArray[3];
    int mask;
    int tableSizeMax;
    float float_1_0;
}   DtbTable;
#pragma pack(pop)

BbQueue alreadyCalled = bb_queue_local_decl();

#define HOOK_SCHRAGE_FUNCTION(address)                                                                 \
int OFFSET_##address = (0x##address - 0x400000);                                                      \
int __thiscall sub_##address(DtbTable *this, DtbItem ** _out_, DWORD * seed)                          \
{                                                                                                     \
    int (__thiscall *hooked) (DtbTable *, DtbItem **, DWORD *) =                                      \
        (typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) sub_##address);                \
    BbQueue seeds = bb_queue_local_decl();                                                            \
                                                                                                      \
    if (!bb_queue_exists(&alreadyCalled, this)) {                                                     \
        bb_queue_add (&alreadyCalled, this);                                                          \
        int end = this->tableSizeMax * 2;                                                             \
        int start = 0;                                                                                \
        dbg("==============================");                                                        \
        dbg ("Table = %p | Seed parameter = 0x%x | Table Size = %d", this, *seed, end);                \
        for (DtbItem *curItem = this->table[start]; start < end ; curItem = curItem->next, start++) { \
            if (bb_queue_exists (&seeds, curItem))                                                    \
                break;                                                                                \
            bb_queue_add (&seeds, curItem);                                                           \
            dbg ("CurItem = %p | Seed = 0x%x", curItem, curItem->schrageId);                        \
        }                                                                                             \
        dbg("==============================");                                                        \
    }                                                                                                 \
    bb_queue_clear (&seeds);                                                                          \
                                                                                                      \
    return hooked (this, _out_, seed);                                                                \
}

HOOK_SCHRAGE_FUNCTION (65D540);
HOOK_SCHRAGE_FUNCTION (67D340);
HOOK_SCHRAGE_FUNCTION (67D400);
HOOK_SCHRAGE_FUNCTION (67EA40);
HOOK_SCHRAGE_FUNCTION (6E4D70);
HOOK_SCHRAGE_FUNCTION (6EC390);
HOOK_SCHRAGE_FUNCTION (6FA180);
HOOK_SCHRAGE_FUNCTION (709030);
HOOK_SCHRAGE_FUNCTION (7BA5E0);
HOOK_SCHRAGE_FUNCTION (7C9C60);
HOOK_SCHRAGE_FUNCTION (82D140);
HOOK_SCHRAGE_FUNCTION (838F70);
HOOK_SCHRAGE_FUNCTION (893AB0);
HOOK_SCHRAGE_FUNCTION (8FD980);
HOOK_SCHRAGE_FUNCTION (8FDA40);
HOOK_SCHRAGE_FUNCTION (92C050);
HOOK_SCHRAGE_FUNCTION (92C110);
HOOK_SCHRAGE_FUNCTION (941AC0);
HOOK_SCHRAGE_FUNCTION (9B3DA0);
HOOK_SCHRAGE_FUNCTION (9D6D70);
HOOK_SCHRAGE_FUNCTION (9D6E30);
HOOK_SCHRAGE_FUNCTION (9D8340);
HOOK_SCHRAGE_FUNCTION (A1B5F0);
HOOK_SCHRAGE_FUNCTION (A88B80);
HOOK_SCHRAGE_FUNCTION (A9BD20);
HOOK_SCHRAGE_FUNCTION (AF55C0);
HOOK_SCHRAGE_FUNCTION (AF5680);
HOOK_SCHRAGE_FUNCTION (AF6B70);
HOOK_SCHRAGE_FUNCTION (C082E0);
HOOK_SCHRAGE_FUNCTION (C08460);
HOOK_SCHRAGE_FUNCTION (C083A0);
HOOK_SCHRAGE_FUNCTION (C1FB20);
HOOK_SCHRAGE_FUNCTION (C2C2A0);
HOOK_SCHRAGE_FUNCTION (C2C360);
HOOK_SCHRAGE_FUNCTION (FED500);
HOOK_SCHRAGE_FUNCTION (FEDBD0);
HOOK_SCHRAGE_FUNCTION (1011400);
HOOK_SCHRAGE_FUNCTION (1011550);
HOOK_SCHRAGE_FUNCTION (10116A0);
HOOK_SCHRAGE_FUNCTION (1011760);
HOOK_SCHRAGE_FUNCTION (10118E0);
HOOK_SCHRAGE_FUNCTION (1011F70);
HOOK_SCHRAGE_FUNCTION (1012030);
HOOK_SCHRAGE_FUNCTION (10740B0);




// ====================================================


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

int __thiscall ItemTable__convertIESToIR (ItemTable *this, imcIESObject *object, IRItem *irItem)
{
	#define OFFSET_convertIESToIR (0x0BF1E10 - 0x400000)
	int (__thiscall *hooked) (ItemTable *, imcIESObject *, IRItem *) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) ItemTable__convertIESToIR);

    int res = hooked (this, object, irItem);

    static int curId = 0;

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
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_1,     (ULONG_PTR) logDebug_1);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_2,     (ULONG_PTR) logDebug_2);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_NetEncrypt,     (ULONG_PTR) imcCrypt__NetEncrypt);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_GetPacket,      (ULONG_PTR) ClientNet__GetPacket);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_convertIESToIR, (ULONG_PTR) ItemTable__convertIESToIR);

    #define HookEngine_hook_Shrage(address) \
        HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_##address, (ULONG_PTR) sub_##address);
    HookEngine_hook_Shrage (65D540);
    HookEngine_hook_Shrage (67D340);
    HookEngine_hook_Shrage (67D400);
    HookEngine_hook_Shrage (67EA40);
    HookEngine_hook_Shrage (6E4D70);
    HookEngine_hook_Shrage (6EC390);
    HookEngine_hook_Shrage (6FA180);
    HookEngine_hook_Shrage (709030);
    HookEngine_hook_Shrage (7BA5E0);
    HookEngine_hook_Shrage (7C9C60);
    HookEngine_hook_Shrage (82D140);
    HookEngine_hook_Shrage (838F70);
    HookEngine_hook_Shrage (893AB0);
    HookEngine_hook_Shrage (8FD980);
    HookEngine_hook_Shrage (8FDA40);
    HookEngine_hook_Shrage (92C050);
    HookEngine_hook_Shrage (92C110);
    HookEngine_hook_Shrage (941AC0);
    HookEngine_hook_Shrage (9B3DA0);
    HookEngine_hook_Shrage (9D6D70);
    HookEngine_hook_Shrage (9D6E30);
    HookEngine_hook_Shrage (9D8340);
    HookEngine_hook_Shrage (A1B5F0);
    HookEngine_hook_Shrage (A88B80);
    HookEngine_hook_Shrage (A9BD20);
    HookEngine_hook_Shrage (AF55C0);
    HookEngine_hook_Shrage (AF5680);
    HookEngine_hook_Shrage (AF6B70);
    HookEngine_hook_Shrage (C082E0);
    HookEngine_hook_Shrage (C08460);
    HookEngine_hook_Shrage (C083A0);
    HookEngine_hook_Shrage (C1FB20);
    HookEngine_hook_Shrage (C2C2A0);
    HookEngine_hook_Shrage (C2C360);
    HookEngine_hook_Shrage (FED500);
    HookEngine_hook_Shrage (FEDBD0);
    HookEngine_hook_Shrage (1011400);
    HookEngine_hook_Shrage (1011550);
    HookEngine_hook_Shrage (10116A0);
    HookEngine_hook_Shrage (1011760);
    HookEngine_hook_Shrage (10118E0);
    HookEngine_hook_Shrage (1011F70);
    HookEngine_hook_Shrage (1012030);
    HookEngine_hook_Shrage (10740B0);

	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_LuaCmd,  (ULONG_PTR) Lua__LuaCmd);
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
