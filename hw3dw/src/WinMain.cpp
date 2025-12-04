#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include <cwctype>

#include "App.hpp"
#include "Logging.hpp"

#define WAIT_FOR_DEBUGGER FALSE

// Global Variables:
HINSTANCE g_rootInstance; // current instance

std::vector<std::wstring> splitCommandLine(LPWSTR commandLine);
bool isNumber(const std::wstring& str);
struct ProcessInfo
{
	DWORD parentProcessId;
	std::wstring parentProcessName;
};
ProcessInfo getParentProcessInfo(DWORD processId);

int APIENTRY WinMain(
	// ReSharper disable CppInconsistentNaming
	_In_ const HINSTANCE hInstance,
	_In_opt_ const HINSTANCE hPrevInstance,
	_In_ const LPSTR lpCommandLine,
	_In_ const int nShowCmd
	// ReSharper enable CppInconsistentNaming
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCommandLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	// Wait for debugger to attach
#if WAIT_FOR_DEBUGGER
	while (!IsDebuggerPresent()) {
		Sleep(100); // Sleep for 100 milliseconds
	}
#endif

	try
	{
		g_rootInstance = hInstance;

		// Initialize logging
		Logging::initialize(LOG_LEVEL_DEFAULT);
		Logging::initializeDebugOutputLogger(LOG_LEVEL_DEBUG_OUTPUT);

		// Detect parent process for console logging
		std::wstring commandLine(lpCommandLine, lpCommandLine + strlen(lpCommandLine));
		std::vector<std::wstring> args = splitCommandLine(&commandLine[0]);

#if (IS_DEBUG)
		const DWORD processId = GetCurrentProcessId();
		const auto [parentProcessId, parentProcessName] = getParentProcessInfo(processId);
		PLOGI << "Parent Process ID  (from process query info): " << parentProcessId;

		bool allowConsoleLogging = false;
		if (parentProcessName.find(L"msvsmon.exe") != std::wstring::npos)
		{
			PLOGI << "Launched from Visual Studio.";
			allowConsoleLogging = true;
		}
		else if (!args.empty() && args.size() >= 2 && args[0].compare(L"--pid") == 0 && isNumber(args[1]))
		{
			const DWORD commandLineParentProcessId = std::stoul(args[1]);
			PLOGI << "Parent Process ID (passed from command line): " << commandLineParentProcessId;

			if (parentProcessId == commandLineParentProcessId)
			{
				PLOGI << "Launched from command line wrapper.";
				allowConsoleLogging = true;
			}
		}
#endif

		App app(allowConsoleLogging);
		PLOGI << "Running App";
		return app.run();

		PLOGI << "Closing App";
	}
	//BUGBUG : Should be using MessageBox and adjusting text based on target encoding. Currently assuming ASCII to match output of Exception.what().
	catch (const AtumException& e) {
		PLOGF << e.getType() << ":" << "\n" << e.what();
		MessageBoxA(nullptr, e.what(), e.getType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		PLOGF << "Standard Exception:" << "\n" << e.what();
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		PLOGF << "Unknown Exception:" << "\n" << "No further details about the exception are available.";
		MessageBox(nullptr, TEXT("No details available"), TEXT("Unknown Exception"), MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}

std::vector<std::wstring> splitCommandLine(const LPWSTR commandLine)
{
	std::wstringstream wss(commandLine);
	std::wstring item;
	std::vector<std::wstring> args;
	while (wss >> item)
	{
		args.push_back(item);
	}
	return args;
}

bool isNumber(const std::wstring& str)
{
	for (const wchar_t c : str)
	{
		if (!std::iswdigit(c))
		{
			return false;
		}
	} return true;
}

ProcessInfo getParentProcessInfo(const DWORD processId)
{
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (entry.th32ProcessID == processId)
			{
				if (const HANDLE parentHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ParentProcessID)) {
					wchar_t parentProcessImageName[MAX_PATH];
					DWORD size = MAX_PATH;
					QueryFullProcessImageName(parentHandle, 0, parentProcessImageName, &size);
					const std::wstring parentImageName(parentProcessImageName);
					const DWORD parentProcessId = GetProcessId(parentHandle);
					CloseHandle(parentHandle);
					CloseHandle(snapshot);
					return {parentProcessId, parentImageName};
				}
			}
		}
	}
	CloseHandle(snapshot);
	PLOGW << "Failed to get parent process info";
	return {0, L""};
}