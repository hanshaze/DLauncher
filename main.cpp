#include <windows.h>
#include <iostream>
#include <fstream>
using namespace std;
char *dll;
char *process;
bool loadConfigFile() {
	if (ifstream(".\\dlauncher.ini")) {
		GetPrivateProfileString("DLAUNCHER", "dll", NULL, dll, 256, ".\\dlauncher.ini");
		if (strcmp(dll, "") == 0) {
			return false;
		}
		GetPrivateProfileString("DLAUNCHER", "process", NULL, process, 256, ".\\dlauncher.ini");
		if (strcmp(process, "") == 0) {
			return false;
		}
		return true;
	}
	else {
		return false;
	}
}
int inject(HANDLE Proc)
{
	if (!ifstream(dll))
	{
		return 1;
	}
	char buf[50] = { 0 };
	if (!Proc)
	{
		return 1;
	}
	LPVOID LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	LPVOID RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(dll), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(Proc, (LPVOID)RemoteString, dll, strlen(dll), NULL);
	CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);
	CloseHandle(Proc);
	return 0;
}
int main() {
	if (!loadConfigFile()) {
		return 0;
	}
	system("title DLauncher");
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	DWORD oldProtect;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo;
	if (ifstream(process)) {
		cout << "Starting the process \"" << process << "\"...\n";
		if (CreateProcess(process, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo)) {
			SuspendThread(ProcessInfo.hThread);
			cout << "Injecting the dll \"" << dll << "...\n";
			if (inject(ProcessInfo.hProcess) != 0) {
				DWORD exitCode;
				GetExitCodeProcess(ProcessInfo.hProcess, &exitCode);
				TerminateProcess(ProcessInfo.hProcess, exitCode);
				return 0;
			}
			cout << "DLL Injected.";
			ResumeThread(ProcessInfo.hThread);
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}
