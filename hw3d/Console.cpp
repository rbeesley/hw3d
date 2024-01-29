#include "AtumWindows.h"
#include "Console.h"
#include "Logging.h"

Console::ConsoleClass Console::ConsoleClass::conClass;

Console::ConsoleClass::ConsoleClass() noexcept
	:hInst(GetModuleHandle(nullptr))
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::ConsoleClass()\n");
#endif // CALLTRACING
}

Console::ConsoleClass::~ConsoleClass()
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::~ConsoleClass()\n");
#endif // CALLTRACING
	FreeConsole();
}

const LPCWSTR Console::ConsoleClass::GetName() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::GetName()\n");
#endif // CALLTRACING
	return conClassName;
}

HINSTANCE Console::ConsoleClass::GetInstance() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::ConsoleClass::GetInstance()\n");
#endif // CALLTRACING
	return conClass.hInst;
}

Console::Console(const LPCWSTR name) noexcept {
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::Console()\n");
#endif // CALLTRACING
	AllocConsole();
	hWnd = GetConsoleWindow();

	PLOGV << "Initialize STD File Streams";
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	// Set Console Window Title
	PLOGV << "Set Console Window Title";
	SetConsoleTitle(name);

	// Disable System Close Button and Menu Item
	PLOGV << "Disable Console Window System Close Button and Menu Item";
	auto hmenu = GetSystemMenu(hWnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);


	PLOGV << "Enable Console Window Handlers";
	// Enable Ctrl Handler
	SetConsoleCtrlHandler(CtrlHandler, TRUE);

	// Enable mouse input
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hInput, &dwMode);
	SetConsoleMode(hInput, dwMode | ENABLE_MOUSE_INPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);

	PLOGV << "Show the Console Window";
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

Console::~Console() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::~Console()\n");
#endif // CALLTRACING
	Logging::GetLogger()->RemoveConsole();
}

HWND Console::GetWindow() noexcept {
	return this->hWnd;
}

BOOL Console::CtrlHandler(DWORD fdwCtrlType) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Console::CtrlHandler()\n");
#endif // CALLTRACING
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
