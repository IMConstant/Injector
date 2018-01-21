#include <iostream>
#include <conio.h>
#include <string.h>

#include "Header.h"

int main(int argc, char *argv[])
{
	char DllName[MAX_PATH];
	char ProcessName[MAX_PATH];

	std::cout << "Dynamic Library : ";
	std::cin.getline(DllName, 256, '\n');
	std::cout << "Process Name : ";
	std::cin.getline(ProcessName, 256, '\n');

	if (!virtlib::InjectDynamicLibrary(FindProcessId(ProcessName), DllName))
		std::cout << "Error : " << GetLastError() << std::endl;
	else
		std::cout << "Dll successfuly injected\n";

	std::cout << "Press 1 to eject Dll from a process\n";

	if (_getch() == '1')
	if (virtlib::EjectDynamicLibrary(FindProcessId(ProcessName), DllName))
			std::cout << "Dll successfuly unloaded.\n";
		else
			std::cout << "Dll was not unloaded\n";

	std::cin.get();
	return 0;
}