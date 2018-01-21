#include <iostream>

#include "Header.h"

DWORD FindProcessId(char *line)
{
	HANDLE hSnap;
	PROCESSENTRY32 pe;

	BOOL flag = 0;

	pe.dwSize = sizeof(PROCESSENTRY32);

	std::cout << "Waiting for start process : ";

	while (!flag)
	{
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(hSnap, &pe))
		{
			do
			{
				if (strcmp(pe.szExeFile, line) == 0)
				{
					flag = 1;

					std::cout << line << std::endl;

					break;
				}

			} while (Process32Next(hSnap, &pe));
		}
	}

	return pe.th32ProcessID;
}

HMODULE FindDllModule(DWORD processID, char *DllPath)
{
	MODULEENTRY32 me;
	HANDLE hSnap;
	
	me.dwSize = sizeof(MODULEENTRY32);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);

	if (Module32First(hSnap, &me))
	{
		do
		{
			if (strstr(DllPath, me.szModule) != NULL)
				break;

		} while (Module32Next(hSnap, &me));
	}

	return me.hModule;
}

BOOL WINAPI virtlib::InjectDynamicLibrary(DWORD ProcessID, LPSTR DllPath)
{
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessID);

	if (hTargetProcess == INVALID_HANDLE_VALUE || hTargetProcess == NULL)
		return FALSE;

	LPVOID LpadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	LPVOID MemoryAddress = (LPVOID)VirtualAllocEx(hTargetProcess, NULL,
		strlen(DllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!WriteProcessMemory(hTargetProcess, MemoryAddress, DllPath, strlen(DllPath) + 1, NULL))
		return FALSE;

	HANDLE hThread = CreateRemoteThread(hTargetProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)LpadLibAddr, MemoryAddress, NULL, NULL);

	if (hThread == NULL || hThread == INVALID_HANDLE_VALUE)
	{
		std::cout << "We can't create remote thread\n";
		std::cout << GetLastError();

		std::cin.get();
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hTargetProcess, MemoryAddress, strlen(DllPath) + 1, MEM_FREE);

	return TRUE;
}

BOOL WINAPI virtlib::EjectDynamicLibrary(DWORD processID, LPSTR DllPath)
{
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

	if (hTargetProcess == INVALID_HANDLE_VALUE)
		return FALSE;

	HMODULE hDllModule = FindDllModule(processID, DllPath);

	if (hDllModule == INVALID_HANDLE_VALUE)
		return FALSE;

	LPVOID Address = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");

	HANDLE hThread = CreateRemoteThread(hTargetProcess, NULL,
		0, (LPTHREAD_START_ROUTINE)Address, hDllModule, NULL, NULL);

	if (hThread == INVALID_HANDLE_VALUE)
		return FALSE;

	WaitForSingleObject(hThread, INFINITE);

	if (GetLastError() != 0)
		return FALSE;

	return TRUE;
}