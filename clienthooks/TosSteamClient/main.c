#include <windows.h>
#define DLL_EXPORT __declspec(dllexport)

int DLL_EXPORT Steam_Init_DLL (int a1, int a2, int a3) {
    return 1;
}

int DLL_EXPORT GetSteamID_DLL (int a1, int a2, int a3) {
    return 1;
}

int DLL_EXPORT Steam_GetLoginInfo_DLL (int a1, int a2, int a3) {
    return 1;
}

DLL_EXPORT BOOL APIENTRY DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return TRUE;
}
