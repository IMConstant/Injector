#ifndef Header_h
#define Header_h

#include <Windows.h>
#include <TlHelp32.h>

DWORD FindProcessId(char *);
HMODULE FindDllModule(DWORD, char *);

namespace virtlib
{
	BOOL WINAPI InjectDynamicLibrary(DWORD, LPSTR);
	BOOL WINAPI EjectDynamicLibrary(DWORD, LPSTR);
}

#endif