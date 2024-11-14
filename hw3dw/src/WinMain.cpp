#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <tchar.h>
#include <cwctype>

#include "App.h"
#include "Logging.h"

#define WAIT_FOR_DEBUGGER FALSE

// Global Variables:
HINSTANCE g_root_instance; // current instance
bool g_allow_console_logging = false;

std::vector<std::wstring> split_command_line(LPWSTR command_line);
bool is_number(const std::wstring& str);
struct process_info
{
	DWORD parent_process_id;
	std::wstring parent_process_name;
};
process_info get_parent_process_info(DWORD process_id);

int APIENTRY WinMain(
	// ReSharper disable CppInconsistentNaming
	_In_ const HINSTANCE hInstance,
	_In_opt_ const HINSTANCE hPrevInstance,
	_In_ const LPSTR lpCmdLine,
	_In_ const int nShowCmd
	// ReSharper enable CppInconsistentNaming
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	// Wait for debugger to attach
#if WAIT_FOR_DEBUGGER
	while (!IsDebuggerPresent()) {
		Sleep(100); // Sleep for 100 milliseconds
	}
#endif

	try
	{
		g_root_instance = hInstance;

		// Initialize logging
		logging::initialize(LOG_LEVEL_DEFAULT);

		// Set up Debug Output Logger
		logging::initialize_debug_output_logger(LOG_LEVEL_DEBUG_OUTPUT);

		// Split the command line into individual arguments
		std::wstring cmdLine(lpCmdLine, lpCmdLine + strlen(lpCmdLine));
		std::vector<std::wstring> args = split_command_line(&cmdLine[0]);

#if (IS_DEBUG)
		const DWORD process_id = GetCurrentProcessId();
		const auto [parent_process_id, parent_process_name] = get_parent_process_info(process_id);
		PLOGI << "Parent Process ID  (from process query info): " << parent_process_id;

		if (parent_process_name.find(L"msvsmon.exe") != std::wstring::npos)
		{
			PLOGI << "Launched from Visual Studio.";
			g_allow_console_logging = TRUE;
		}
		else if (!args.empty() && args.size() >= 2 && args[0].compare(L"--pid") == 0 && is_number(args[1]))
		{
			const DWORD cmdline_parent_process_id = std::stoul(args[1]);
			PLOGI << "Parent Process ID (passed from command line): " << cmdline_parent_process_id;

			if (parent_process_id == cmdline_parent_process_id)
			{
				PLOGI << "Launched from command line wrapper.";
				g_allow_console_logging = TRUE;
			}
		}
#endif

		app app;
		if (const int result = app.initialize() < 0)
		{
			return result;
		}

		// Start Window Message Pump
		PLOGI << "Running App";
		const int result = app.run();

		PLOGI << "Closing App";
		app.shutdown();

		return result;
	}

	//BUGBUG : Should be using MessageBox and adjusting text based on target encoding. Currently assuming ASCII to match output of exception.what().
	catch (const atum_exception& e) {
		PLOGF << e.get_type() << ":" << "\n" << e.what();
		MessageBoxA(nullptr, e.what(), e.get_type(), MB_OK | MB_ICONEXCLAMATION);
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

std::vector<std::wstring> split_command_line(LPWSTR command_line)
{
	std::wstringstream wss(command_line);
	std::wstring item;
	std::vector<std::wstring> args;
	while (wss >> item)
	{
		args.push_back(item);
	}
	return args;
}

bool is_number(const std::wstring& str)
{
	for (const wchar_t c : str)
	{
		if (!std::iswdigit(c))
		{
			return false;
		}
	} return true;
}

process_info get_parent_process_info(const DWORD process_id)
{
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &entry))
	{
		while (Process32Next(snapshot, &entry))
		{
			if (entry.th32ProcessID == process_id)
			{
				if (const HANDLE parent_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ParentProcessID)) {
					wchar_t parent_process_image_name[MAX_PATH];
					DWORD size = MAX_PATH;
					QueryFullProcessImageName(parent_handle, 0, parent_process_image_name, &size);
					const std::wstring parent_image_name(parent_process_image_name);
					const DWORD parent_process_id = GetProcessId(parent_handle);
					CloseHandle(parent_handle);
					CloseHandle(snapshot);
					return {parent_process_id, parent_image_name};
				}
			}
		}
	}
	CloseHandle(snapshot);
	PLOGW << "Failed to get parent process info";
	return {0, L""};
}