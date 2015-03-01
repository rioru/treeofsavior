#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Win32Tools/Win32Tools.h"

#pragma pack(push, 1)
typedef struct _StringID
{
  struct _StringID *self;
  DWORD unk2;
  DWORD unk3;
  int stringLen;
  char stringOrPtr[32];

}   StringID;
#pragma pack(pop)


int main (int argc, char **argv)
{
    DWORD baseAddr;

    if (argc < 3) {
        printf("Usage : <Array base address (HEX)> <Number of StringID (DECIMAL)>\n\n");
        printf("Array base address (HEX) = ");
        scanf ("%x", (int *) &baseAddr);
    } else {
        baseAddr = strtol(argv[1], NULL, 16);
    }

    Ztring *outputz = ztring_new ();

    for (DWORD curAddr = baseAddr; ; curAddr += 4) {
        HANDLE process = get_handle_by_name ("Client_tos.exe");
        DWORD pStringID = read_memory_as_int (process, curAddr);

        if (!pStringID) {
            break;
        }

        StringID sid;
        read_from_memory (process, &sid, pStringID, sizeof(StringID));
        ztring_concat (outputz, str_dup_printf("MakeNameForce (0x%08X, \"SID_%.32s\");\n", curAddr, sid.stringOrPtr));
    }

    const char* output = ztring_get_text (outputz);
    const size_t len = strlen(output) + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), output, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    printf("\n\n%s\n", output);

    system("pause");

    return 0;
}
