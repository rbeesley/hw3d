#include "Console.hpp"

#include <thread>

#include "Logging.hpp"

Console::ConsoleClass Console::ConsoleClass::consoleClass_;
Console::ConsoleState Console::consoleState_{};

Console::ConsoleClass::ConsoleClass() noexcept
	: instanceHandle_(GetModuleHandle(nullptr))
{
	PLOGD << "Instantiate Console Class";
}

Console::ConsoleClass::~ConsoleClass() noexcept
{
	PLOGD << "Destroy Console Class";
	PLOGD << "Console Class Destroyed";
}

LPCWSTR Console::ConsoleClass::getName() noexcept
{
	return consoleClassName_;
}

HINSTANCE Console::ConsoleClass::getInstance() noexcept
{
	return consoleClass_.instanceHandle_;
}


extern bool g_allowConsoleLogging;

bool Console::freeConsole_ = true;
HWND Console::appWindowHandle_;

Console::Console(const HWND appWindowHandle, const LPCWSTR name) noexcept
	: consoleWindowHandle_(nullptr)
#if CONSOLE_SHUTDOWN_EVENT
	, shutdownEvent_(nullptr)
#endif
{
	PLOGD << "Instantiate Console";
	appWindowHandle_ = appWindowHandle;

	PLOGV << "Attach or Create Console";
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		PLOGV << "Creating Console";
		AllocConsole();
	}
	consoleWindowHandle_ = GetConsoleWindow();

	// Save initial console state
	PLOGV << "Save initial console state";
	saveState();

	PLOGV << "Initialize STD File Streams";
	errno_t result = 0;
	// ReSharper disable StringLiteralTypo
	result |= freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	result |= freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	result |= freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
	// ReSharper restore StringLiteralTypo

	if (result)
	{
		PLOGW << "Error initializing STD File Streams: " << result;
	}

	// Set Console Window Title
	PLOGV << "Set Console Window Title";
	SetConsoleTitle(name);

	// Disable System Close Button and Menu Item
	PLOGV << "Disable Console Window System Close Button and Menu Item";
	const auto menu = GetSystemMenu(consoleWindowHandle_, FALSE);
	EnableMenuItem(menu, SC_CLOSE, MF_GRAYED);

	// Enable mouse input
	PLOGV << "Enable Console mouse input";
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(input, &mode);
	mode &= ~ENABLE_LINE_INPUT; // Disable line input
	mode &= ~ENABLE_ECHO_INPUT; // Disable echo input
	mode |= ENABLE_EXTENDED_FLAGS; // Ensure extended flags are enabled
	mode |= ENABLE_MOUSE_INPUT | ENABLE_QUICK_EDIT_MODE; // Enable mouse and quick edit modes
	SetConsoleMode(input, mode);

	PLOGV << "Enable Console Window Ctrl Handler";
	// Enable Ctrl Handler
	if (SetConsoleCtrlHandler(ctrlHandler, TRUE)) { PLOGV << "Success"; }
	else { PLOGV << "Failure"; }

	PLOGV << "Show the Console Window";
	ShowWindow(GetConsoleWindow(), SW_SHOW);

	// Set up Console Logger
	Logging::initializeConsoleLogger(LOG_LEVEL_CONSOLE);

#if CONSOLE_SHUTDOWN_EVENT
	PLOGV << "Enable Console Shutdown Event";
	// Create a manual-reset event to block the shell
	shutdownEvent_ = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (shutdownEvent_ == nullptr)
	{
		PLOGE << "Failed to create shutdown event.";
		return;
	}

	// Block until shutdownEvent_ is set
	std::thread([this]()
		{
			blockInput();
		}).detach();
#endif
}

HWND Console::getHandle() const noexcept {
	return consoleWindowHandle_;
}

void Console::saveState()
{
	saveConsoleState(consoleState_);
}

void Console::restoreState()
{
	restoreConsoleState(consoleState_);
}

void Console::saveConsoleState(ConsoleState& state)
{
	PLOGV << "Save Console State -";

	// Save console title
	std::vector<wchar_t> buffer(MAX_PATH);
	GetConsoleTitle(buffer.data(), static_cast<DWORD>(buffer.size()));
	state.title.assign(buffer.data());

	// Save console mode
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &state.mode);

	// Save system menu and close button state
	const auto flags = GetMenuState(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
	state.closeButton = flags & (MF_DISABLED | MF_GRAYED);

	PLOGV << "         Title: " << state.title;
	PLOGV << "          Mode: " << state.mode;
	PLOGV << "  Close Button: " << state.closeButton;
}

void Console::restoreConsoleState(const ConsoleState& state)
{
	{
		ConsoleState currentState;
		PLOGV << "Current Console State -";

		// Get current console title
		std::vector<wchar_t> buffer(MAX_PATH);
		GetConsoleTitle(buffer.data(), static_cast<DWORD>(buffer.size()));
		currentState.title.assign(buffer.data());

		// Get current console mode
		GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &currentState.mode);

		// Get current system menu and close button state
		const auto flags = GetMenuState(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		currentState.closeButton = flags & (MF_DISABLED | MF_GRAYED);

		PLOGV << "         Title: " << currentState.title;
		PLOGV << "          Mode: " << currentState.mode;
		PLOGV << "  Close Button: " << currentState.closeButton;
	}

	// Restore console title
	SetConsoleTitle(state.title.c_str());

	// Restore console mode
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(input, state.mode);

	// Restore system menu and close button state
	if (const auto system_menu = GetSystemMenu(GetConsoleWindow(), FALSE); system_menu != nullptr) {
		EnableMenuItem(system_menu, SC_CLOSE, state.closeButton);
	}

	{
		ConsoleState restored_state;
		PLOGV << "Restored Console State -";

		// Save console title
		std::vector<wchar_t> buffer(MAX_PATH);
		GetConsoleTitle(buffer.data(), static_cast<DWORD>(buffer.size()));
		restored_state.title.assign(buffer.data());

		// Save console mode
		GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &restored_state.mode);

		// Save system menu and close button state
		const auto flags = GetMenuState(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		restored_state.closeButton = flags & (MF_DISABLED | MF_GRAYED);

		PLOGV << "         Title: " << restored_state.title;
		PLOGV << "          Mode: " << restored_state.mode;
		PLOGV << "  Close Button: " << restored_state.closeButton;
	}
}

#if CONSOLE_SHUTDOWN_EVENT
void Console::blockInput() noexcept
{
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE handles[2] = {
		input, shutdownEvent_
	};

	while (true) {
		DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (result == WAIT_OBJECT_0 + 1) {
			PLOGW << "Shutdown Event";
			break;
			// shutdownEvent_ is signaled
		} if (result == WAIT_OBJECT_0) {
			// Process input event, keep blocking the shell
			INPUT_RECORD input_record;
			DWORD events;
			ReadConsoleInput(input, &input_record, 1, &events);
			//PLOGI << "Input Event: " << input_record.EventType;
			if (input_record.EventType == KEY_EVENT) {
				// Optionally handle specific input events here
			}
		}
	}
	CloseHandle(shutdownEvent_);
	shutdownEvent_ = nullptr;
}
#endif

void Console::shutdown() const
{
	PLOGI << "Shutdown Console";

#if CONSOLE_SHUTDOWN_EVENT
	// Signal the shutdown event
	if (shutdownEvent_)
	{
		PLOGI << "Signal the Console Shutdown Event";
		SetEvent(shutdownEvent_);
	}
#endif

	Logging::shutdownConsoleLogger();
}

Console::~Console() noexcept
{
	PLOGD << "Destroy Console";
#if CONSOLE_SHUTDOWN_EVENT
	if (shutdownEvent_)
	{
		CloseHandle(shutdownEvent_);
		shutdownEvent_ = nullptr;
	}
#endif

	PLOGI << "Restore Console State";
	restoreState();

	if (freeConsole_)
	{
		FreeConsole();
	}

	consoleWindowHandle_ = HWND();
	PLOGD << "Console Destroyed";
}

BOOL Console::ctrlHandler(const DWORD ctrl_type) noexcept
{
	reinterpret_cast<Console*>(GetWindowLongPtr(GetConsoleWindow(), GWLP_USERDATA));  // NOLINT(clang-diagnostic-unused-value, performance-no-int-to-ptr)
	switch (ctrl_type)
	{
	case CTRL_C_EVENT:
		PLOGW << "Ctrl-C event";
		Beep(750, 300);
		return TRUE;

	case CTRL_BREAK_EVENT:
		PLOGW << "Ctrl-Break event";
		Beep(900, 200);
		return TRUE;

	case CTRL_CLOSE_EVENT:
		PLOGW << "Ctrl-Close event";
		Beep(600, 200);
		freeConsole_ = false;
		if (appWindowHandle_ != nullptr) { SendMessage(appWindowHandle_, WM_APP_CONSOLE_CLOSE, 0, 0); }
		Sleep(INFINITE); // Block this thread from processing further so that the App can close gracefully
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		PLOGW << "Ctrl-Logoff event";
		Beep(1000, 200);
		return TRUE;

	case CTRL_SHUTDOWN_EVENT:
		PLOGW << "Ctrl-Shutdown event";
		Beep(750, 500);
		return TRUE;

	default:
		PLOGW << " Unknown CtrlHandler event: 0x" << std::hex << ctrl_type;
		return FALSE;
	}
}
