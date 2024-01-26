#include "AtumWindows.h"
#include "Console.h"
#include "Logging.h"

#ifndef NOMSGHANDLER
#include "WindowsMessageMap.h"
#endif // NOMSGHANDLER

Console::ConsoleClass Console::ConsoleClass::conClass;
#ifndef NOMSGHANDLER
const static WindowsMessageMap wmm;
#endif // NOMSGHANDLER

Console::ConsoleClass::ConsoleClass() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::ConsoleClass()\n");
#endif // CALLTRACING
	PLOGV << "Console::ConsoleClass::ConsoleClass()";
	AllocConsole();
	hWnd = GetConsoleWindow();
}

Console::ConsoleClass::~ConsoleClass()
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::~ConsoleClass()\n");
#endif // CALLTRACING
	PLOGV << "Console::ConsoleClass::~ConsoleClass()";
	FreeConsole();
}

const LPCWSTR Console::ConsoleClass::GetName() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::GetName()\n");
#endif // CALLTRACING
	PLOGV << "Console::ConsoleClass::GetName()";
	return conClassName;
}

HWND Console::ConsoleClass::GetInstance() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::GetInstance()\n");
#endif // CALLTRACING
	PLOGV << "Console::ConsoleClass::GetInstance()";
	return conClass.hWnd;
}

Console::Console(const LPCWSTR name) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::Console()\n");
#endif // CALLTRACING
	PLOGV << "Console::Console()";
	//CreateConsole();
	//ShowWindow(ConsoleClass::GetInstance(), SW_SHOW);
	InitializeStreams();
	InitializeConsole();
	InitializeHandlers();
	ShowConsoleWindow();
}

Console::~Console() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::~Console()\n");
#endif // CALLTRACING
	PLOGV << "Console::~Console()";
	Logging::GetLogger()->RemoveConsole();
	try
	{
		if (pCin != nullptr) fclose(pCin);
	}
	catch (const std::exception&)
	{
	}
	try
	{
		if (pCout != nullptr) fclose(pCout);
	}
	catch (const std::exception&)
	{
	}
	try
	{
		if (pCerr != nullptr) fclose(pCerr);
	}
	catch (const std::exception&)
	{
	}
}

void Console::InitializeConsole() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::InitializeConsole()\n");
#endif // CALLTRACING
	PLOGV << "Console::InitializeConsole()";
	// Create a console window
	auto hmenu = GetSystemMenu(ConsoleClass::GetInstance(), FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

void Console::CreateConsole() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::CreateConsole()\n");
#endif // CALLTRACING
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

void Console::InitializeStreams() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::InitializeStreams()\n");
#endif // CALLTRACING
	PLOGV << "Console::InitializeStreams()";
	freopen_s(&pCin, "CONIN$", "r", stdin);
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	freopen_s(&pCerr, "CONOUT$", "w", stderr);
	//freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	//freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
}

void Console::InitializeHandlers() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::InitializeHandlers()\n");
#endif // CALLTRACING
	PLOGV << "Console::InitializeHandlers()";

	// Enable Ctrl Handler
	SetConsoleCtrlHandler(CtrlHandler, TRUE);

#ifndef NOMSGHANDLER
	/*
	// Enable Signals (Conflicts with ConsoleCtrlHandler although more standard)
	signal(SIGILL, &Console::Sig_Handler);
	signal(SIGFPE, &Console::Sig_Handler);
	signal(SIGSEGV, &Console::Sig_Handler);
	signal(SIGTERM, &Console::Sig_Handler);
	signal(SIGBREAK, &Console::Sig_Handler);
	signal(SIGABRT, &Console::Sig_Handler);
	*/

	// Enable Msg Handler
	/*
	// Method 1 (Initially recommended way, but obsolete and causes access denied (0x5) errors)
	SetLastError(0);
	LONG_PTR glpfnConsoleWindow = SetWindowLongPtr(hConWnd, GWLP_WNDPROC, (LONG_PTR)Console::HandleMsg);
	if (glpfnConsoleWindow == 0)
	{
		DWORD err = GetLastError();
		std::cout << "Failed to set window procedure. Error code: " << err << std::endl;
		//return 1;
	}
	*/

	/*
	// Method 2 (Newer API which is cleaner, but still doesn't work although error isn't indicated)
	PLOGV << "Setting Console Message Handler";
	auto hConWnd = GetConsoleWindow();
	auto result = SetWindowSubclass(hConWnd, Console::HandleMsg, 1, 0);
	PLOGV << "SetWindowSubclass result: " << result;
	*/
#endif // NOMSGHANDLER

//	/*
	// Enable mouse input
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hInput, &dwMode);
	SetConsoleMode(hInput, dwMode | ENABLE_MOUSE_INPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
//	*/
}

void Console::ShowConsoleWindow() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ShowConsoleWindow()\n");
#endif // CALLTRACING
	PLOGV << "Console::ShowConsoleWindow()";
	// Use the ShowWindow() function to show the console window for logging
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

#ifndef NOMSGHANDLER
void Console::RunMessagePump() {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::RunMessagePump()\n")
#endif // CALLTRACING
	PLOGV << "Console::RunMessagePump()";
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		PLOGV << wmm(msg.message, msg.lParam, msg.wParam).c_str();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
#endif NOMSGHANDLER

BOOL Console::CtrlHandler(DWORD fdwCtrlType) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::CtrlHandler()\n");
#endif // CALLTRACING
	PLOGV << "Console::CtrlHandler()";
	Console* conptr = (Console*)GetWindowLongPtr(GetConsoleWindow(), GWLP_USERDATA);
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
		PLOGV << "Ctrl-C event";
		Beep(750, 300);
		return TRUE;

	case CTRL_CLOSE_EVENT:
		PLOGV << "Ctrl-Close event";
		Beep(600, 200);
		return TRUE;

	case CTRL_BREAK_EVENT:
		PLOGV << "Ctrl-Break event";
		Beep(900, 200);
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		PLOGV << "Ctrl-Logoff event";
		Beep(1000, 200);
		return TRUE;

	case CTRL_SHUTDOWN_EVENT:
		PLOGV << "Ctrl-Shutdown event";
		Beep(750, 500);
		return TRUE;

	default:
		PLOGV << " Unknown CtrlHandler event";
		return FALSE;
	}
}

#ifndef NOMSGHANDLER
LRESULT CALLBACK Console::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::HandleMsg()\n");
#endif // CALLTRACING
	int wmId, wmEvent;

	//PLOGV_(Logging::kConsole) << wmm(msg, lParam, wParam).c_str();
	PLOGV << wmm(msg, lParam, wParam).c_str();

	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case 0:
		default:
			//return CallWindowProc(glpfnConsoleWindow, hWnd, msg, wParam, lParam);
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		//return CallWindowProc(glpfnConsoleWindow, hWnd, msg, wParam, lParam);
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void Console::Sig_Handler(int n_signal) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::Sig_Handler()\n");
#endif // CALLTRACING
	PLOGV << "Signal: " << n_signal;
}
#endif NOMSGHANDLER