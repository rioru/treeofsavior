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

#define OFFSET_logDebug_1 (0x0CB0C40 - 0x400000)
// #define OFFSET_LuaGetObject (0x103B6D0 - 0x400000)
// #define OFFSET_logDebug_2 (0xCA8E80 - 0x400000)
#define OFFSET_GetPacket (0x645DC0 - 0x400000)
#define OFFSET_NetEncrypt (0xCBE530 - 0x400000)

char * (__thiscall *StringID__c_str) (StringID **this) = (void *) 0xD394E0;

/** =================== HOOKS ================= */
void __cdecl logDebug_1 (int a1, LPCSTR lpOutputString, char *message)
{
	void (__cdecl *hooked) (int, LPCSTR, char *) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) logDebug_1);

	char buffer[4096];
	sprintf (buffer, message);

	dbg ("%s", buffer);

	return hooked (a1, lpOutputString, message);
}

int __thiscall Lua__LuaGetObject (int this, unsigned __int8 *Src)
{
	int (__thiscall *hooked) (int this, unsigned __int8 *Src) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) Lua__LuaGetObject);

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
	char OutputString[24096];
	va_list va;

	va_start(va, Format);
	vsprintf(OutputString, Format, va);

	dbg ("%s", OutputString);
}

bool __thiscall ClientNet__GetPacket (void *clientNet, BYTE *outPacket, DWORD *outSize, DWORD *outType)
{
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

#define HOOK_SCHRAGE_FUNCTION(name, address)                                                                 \
int OFFSET_##name = (address - 0x400000);                                                                    \
int __thiscall sub_##name(DtbTable *this, DtbItem ** _out_, DWORD * seed)                                    \
{                                                                                                            \
    int (__thiscall *hooked) (DtbTable *, DtbItem **, DWORD *) =                                             \
        (typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) sub_##name);                          \
    BbQueue seeds = bb_queue_local_decl();                                                                   \
                                                                                                             \
    /* if (!bb_queue_exists(&alreadyCalled, this)) {                                                       */\
        bb_queue_add (&alreadyCalled, this);                                                                 \
        int end = this->tableSizeMax * 2;                                                                    \
        int start = 0;                                                                                       \
        dbg("==============================");                                                               \
        dbg ("Table = %p | Seed parameter = 0x%x | Table Size = %d", this, *seed, end);                      \
        for (DtbItem *curItem = this->table[start]; start < end ; curItem = curItem->next, start++) {        \
            if (bb_queue_exists (&seeds, curItem))                                                           \
                break;                                                                                       \
            bb_queue_add (&seeds, curItem);                                                                  \
            dbg ("CurItem = %p | Seed = 0x%x", curItem, curItem->schrageId);                                 \
        }                                                                                                    \
        dbg("==============================");                                                               \
    /*}*/                                                                                                    \
    bb_queue_clear (&seeds);                                                                                 \
                                                                                                             \
    return hooked (this, _out_, seed);                                                                       \
}

// TODO : Redefine the list of Shrage functions
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_1, 0x5A4F30);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_2, 0x5C42F0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_3, 0x5C43B0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_4, 0x5C5A80);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_5, 0x5C8BB0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_6, 0x62E560);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_7, 0x635B30);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_8, 0x6435C0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_9, 0x6531A0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_10, 0x711B30);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_11, 0x722010);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_12, 0x78D870);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_13, 0x79E1C0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_14, 0x84ECD0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_15, 0x84ED90);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_16, 0x87E210);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_17, 0x87E2D0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_18, 0x88D570);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_19, 0x8A0A60);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_20, 0x8F5A30);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_21, 0x8F5AF0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_22, 0x8F5BB0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_23, 0x903BF0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_24, 0x916F20);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_25, 0x916FE0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_26, 0xCE2CE0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_27, 0xCE33B0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_28, 0xD0F410);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_29, 0xD0F560);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_30, 0xD0F6B0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_31, 0xD0F770);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_32, 0xD0FF80);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_33, 0xD10040);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_34, 0xD627C0);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_35, 0x730F70);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_36, 0xD0F830);
HOOK_SCHRAGE_FUNCTION(DtbTable__getObject_37, 0xD0F8F0);




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
	#define OFFSET_convertIESToIR (0x08DDF40 - 0x400000)
	int (__thiscall *hooked) (ItemTable *, imcIESObject *, IRItem *) =
		(typeof(hooked)) HookEngine_get_original_function ((ULONG_PTR) ItemTable__convertIESToIR);

    int res = hooked (this, object, irItem);

    // static int curId = 0;

    // Korean item name :
    char *itemName   = StringID__c_str (&irItem->itemName);

    int seed         = irItem->seed;
    int ItemLv       = irItem->ItemLv;

    char *className  = StringID__c_str (&irItem->className);
    char *itemType   = StringID__c_str (&irItem->itemType);
    char *GroupName  = StringID__c_str (&irItem->GroupName);
    char *classType  = StringID__c_str (&irItem->classType);
    char *EqpType    = StringID__c_str (&irItem->EqpType);
    char *AttackType = StringID__c_str (&irItem->AttackType);
    char *Attribute  = StringID__c_str (&irItem->Attribute);
    char *HandSide   = StringID__c_str (&irItem->HandSide);

    dbg ("Seed = 0x%08X | ItemLv = %0.3d | ClassName = <%s> | itemType = <%s> | GroupName = <%s> | classType = <%s> | "
         "EqpType = <%s> | AttackType = <%s> | Attribute = <%s> | HandSide = <%s> | Name = <%s>",
         seed, ItemLv, className, itemType, GroupName, classType, EqpType, AttackType, Attribute, HandSide, itemName);

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
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_1,     (ULONG_PTR) logDebug_1);
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_logDebug_2,     (ULONG_PTR) logDebug_2);
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_NetEncrypt,     (ULONG_PTR) imcCrypt__NetEncrypt);
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_GetPacket,      (ULONG_PTR) ClientNet__GetPacket);
	// HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_LuaGetObject,  (ULONG_PTR) Lua__LuaGetObject);
	HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_convertIESToIR, (ULONG_PTR) ItemTable__convertIESToIR);

    #define HookEngine_hook_Shrage(name) \
        HookEngine_hook ((ULONG_PTR) baseAddr + OFFSET_##name, (ULONG_PTR) sub_##name);

    /*
    HookEngine_hook_Shrage(DtbTable__getObject_1);
    HookEngine_hook_Shrage(DtbTable__getObject_2);
    HookEngine_hook_Shrage(DtbTable__getObject_3);
    HookEngine_hook_Shrage(DtbTable__getObject_4);
    HookEngine_hook_Shrage(DtbTable__getObject_5);
    HookEngine_hook_Shrage(DtbTable__getObject_6);
    HookEngine_hook_Shrage(DtbTable__getObject_7);
    HookEngine_hook_Shrage(DtbTable__getObject_8);
    HookEngine_hook_Shrage(DtbTable__getObject_9);
    HookEngine_hook_Shrage(DtbTable__getObject_10);
    HookEngine_hook_Shrage(DtbTable__getObject_11);
    HookEngine_hook_Shrage(DtbTable__getObject_12);
    HookEngine_hook_Shrage(DtbTable__getObject_13);
    HookEngine_hook_Shrage(DtbTable__getObject_14);
    HookEngine_hook_Shrage(DtbTable__getObject_15);
    HookEngine_hook_Shrage(DtbTable__getObject_16);
    HookEngine_hook_Shrage(DtbTable__getObject_17);
    HookEngine_hook_Shrage(DtbTable__getObject_18);
    HookEngine_hook_Shrage(DtbTable__getObject_19);
    HookEngine_hook_Shrage(DtbTable__getObject_20);
    HookEngine_hook_Shrage(DtbTable__getObject_22);
    HookEngine_hook_Shrage(DtbTable__getObject_23);
    HookEngine_hook_Shrage(DtbTable__getObject_24);
    HookEngine_hook_Shrage(DtbTable__getObject_25);
    HookEngine_hook_Shrage(DtbTable__getObject_26);
    HookEngine_hook_Shrage(DtbTable__getObject_27);
    HookEngine_hook_Shrage(DtbTable__getObject_28);
    HookEngine_hook_Shrage(DtbTable__getObject_29);
    HookEngine_hook_Shrage(DtbTable__getObject_30);
    HookEngine_hook_Shrage(DtbTable__getObject_31);
    HookEngine_hook_Shrage(DtbTable__getObject_32);
    HookEngine_hook_Shrage(DtbTable__getObject_33);
    HookEngine_hook_Shrage(DtbTable__getObject_34);
    HookEngine_hook_Shrage(DtbTable__getObject_35);
    HookEngine_hook_Shrage(DtbTable__getObject_36);
    HookEngine_hook_Shrage(DtbTable__getObject_37);
    */
    // HookEngine_hook_Shrage(DtbTable__getObject_21);
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
