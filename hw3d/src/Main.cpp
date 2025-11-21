#include <windows.h>
#include <iostream>
#include <string>

int main(const int argc, char* argv[]) {
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD processId = GetCurrentProcessId();
    std::wstring processIdStr = std::to_wstring(processId);

    // Command line to launch hw3dw.exe with the hw3d.exe process ID as a parameter
    std::wstring cmdLineStr = L"hw3dw.exe --pid " + processIdStr;

	// Convert std::wstring to wchar_t*
	wchar_t cmdLine[256];
	wcscpy_s(cmdLine, cmdLineStr.c_str());

    // Start the child process.
    if (!CreateProcess(
        nullptr,        // No module name (use command line)
        cmdLine,        // Command line
        nullptr,        // Process handle not inheritable
        nullptr,        // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        nullptr,        // Use parent's environment block
        nullptr,        // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return -1;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
