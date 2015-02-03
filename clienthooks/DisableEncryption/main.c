#include <windows.h>
#include <tlhelp32.h>
#include <time.h>
#include <psapi.h>
#include <wincon.h>
#include <subauth.h>
#include <ntdef.h>

#define __DEBUG_OBJECT__ "DisableEncryption"
#include "dbg/dbg.h"


#define NetEncrypt_OFFSET 0xBC0210

DWORD
get_pid_by_name (char *proc_name)
{
	DWORD dwPID = 0;

	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	if (!Process32First(hSnapshot, &pe32))
		return 0;

	while (Process32Next(hSnapshot, &pe32))
	{
		if (!stricmp(proc_name, pe32.szExeFile))
		{
			dwPID = pe32.th32ProcessID;
			break;
		}
	}

	CloseHandle(hSnapshot);

	return dwPID;
}


HANDLE
get_handle_from_pid (DWORD pid)
{
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	while (hHandle == INVALID_HANDLE_VALUE)
	{
		hHandle = OpenProcess (
			PROCESS_ALL_ACCESS,
			FALSE, pid
		);

		Sleep(1);
	}

	return hHandle;
}

HANDLE
get_handle_by_name (char *proc_name)
{
	int pid = get_pid_by_name(proc_name);

	return get_handle_from_pid (pid);
}


DWORD
get_baseaddr (char *module_name)
{
	MODULEENTRY32 module_entry = {0};
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, get_pid_by_name(module_name));

	if (!snapshot)
		return 0;

	module_entry.dwSize = sizeof(module_entry);
	BOOL bModule = Module32First(snapshot, &module_entry);

	while (bModule)
	{
		if (!stricmp(module_entry.szModule, module_name))
		{
			CloseHandle(snapshot);
			return (DWORD) module_entry.modBaseAddr;
		}

		bModule = Module32Next(snapshot, &module_entry);
	}

	CloseHandle(snapshot);

	return 0;
}

int main (int argc, char ** argv)
{
    HANDLE process = get_handle_by_name ("Client_tos.exe");
    if (!process) {
        printf ("Process not found\n");
        return 0;
    }

    int baseAddress = get_baseaddr ("Client_tos.exe");
    printf ("BaseAddress = %x\n", baseAddress);

    static unsigned char patchCode [29] = {
        /*  55              push ebp                                      ; Client_tos.imcCrypt__NetEncrypt(guessed Arg1,Arg2,Arg3)
            8BEC            mov ebp, esp
            8B45 10         mov eax, [dword ss:ebp+10]                    ; &encryptedpacket->size
            8D58 02         lea ebx, [eax+2]                              ; &encryptedpacket->data
            8B7D 08         mov edi, [dword ss:ebp+8]                     ; plaintextSize
            66:8908         mov [word ds:eax], cx                         ; encryptedpacket->size = plaintextSize
            57              push edi                                      ; size
            FF75 0C         push [dword ss:ebp+0C]                        ; src
            53              push ebx                                      ; dest
            E8 9B3E5BFF     call Client_tos.memcpy                        ; Jump to MSVCR100.memcpy
            89F8            mov eax, edi
            C9              leave
            C3              retn
        */
        0x55,
        0x8B, 0xEC,
        0x8B, 0x45, 0x10,
        0x8D, 0x58, 0x02,
        0x8B, 0x7D, 0x08,
        0x66, 0x89, 0x08,
        0x57,
        0xFF, 0x75, 0x0C,
        0x53,
        0xE8, 0x9B, 0x3E, 0x5B, 0xFF,
        0x89, 0xF8,
        0xC9,
        0xC3
    };

    static unsigned char originalCode [29] = {
        0x55, 0x8B, 0xEC, 0x83, 0x3D, 0xB4, 0x3C, 0xD3, 0x02, 0x00, 0x75, 0x20, 0x68, 0xAC, 0x0F, 0x52,
        0x01, 0x6A, 0x49, 0x68, 0x68, 0x05, 0x4D, 0x01, 0x68, 0x54, 0x05, 0x4D, 0x01
    };

    unsigned char opcode;
    ReadProcessMemory (process, (LPVOID) baseAddress + NetEncrypt_OFFSET + 3, &opcode, sizeof(opcode), NULL);
    printf ("baseAddress + NetEncrypt_OFFSET = %x => opcode found : %x\n", baseAddress + NetEncrypt_OFFSET, opcode);

    switch (opcode) {
        case 0x83: // Already patched : unpatch
            WriteProcessMemory (process, (LPVOID) baseAddress + NetEncrypt_OFFSET, patchCode, sizeof(patchCode), NULL);
            printf ("Process patched !\n");
        break;

        case 0x8B: // Unpatched : patch
            WriteProcessMemory (process, (LPVOID) baseAddress + NetEncrypt_OFFSET, originalCode, sizeof(originalCode), NULL);
            printf ("Process unpatched !\n");
        break;

        default :
            printf ("Error, unexpected code found.\n");
        break;
    }

    system("pause");
}
