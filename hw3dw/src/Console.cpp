#include "Console.h"

#include <thread>

#include "Logging.h"

console::console_class console::console_class::console_class_;
console::console_state console::console_state_{};

console::console_class::console_class() noexcept
	: instance_handle_(GetModuleHandle(nullptr))
{
	PLOGD << "Instantiate Console Class";
}

console::console_class::~console_class() noexcept
{
	PLOGD << "Destroy Console Class";
}

LPCWSTR console::console_class::get_name() noexcept
{
	return console_class_name;
}

HINSTANCE console::console_class::get_instance() noexcept
{
	return console_class_.instance_handle_;
}

console::console() noexcept
	:
	console_window_handle_(nullptr)
#if CONSOLE_SHUTDOWN_EVENT
	, shutdown_event_(nullptr)
#endif
{
	PLOGD << "Instantiate Console";
}

void console::initialize(const LPCWSTR name) noexcept {
	PLOGI << "Initialize Console";

	PLOGV << "Attach or Create Console";
	if (AttachConsole(ATTACH_PARENT_PROCESS) == false)
	{
		AllocConsole();
	}
	console_window_handle_ = GetConsoleWindow();

	// Save initial console state
	PLOGV << "Save initial console state";
	save_state();

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
	const auto menu = GetSystemMenu(console_window_handle_, FALSE);
	EnableMenuItem(menu, SC_CLOSE, MF_GRAYED);

	PLOGV << "Enable Console Window Handlers";
	// Enable Ctrl Handler
	SetConsoleCtrlHandler(ctrl_handler, TRUE);

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

	PLOGV << "Show the Console Window";
	ShowWindow(GetConsoleWindow(), SW_SHOW);

	// Set up Console Logger
	logging::initialize_console_logger(LOG_LEVEL_CONSOLE);

#if CONSOLE_SHUTDOWN_EVENT
	// Create a manual-reset event to block the shell
	shutdown_event_ = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (shutdown_event_ == nullptr)
	{
		PLOGE << "Failed to create shutdown event.";
		return;
	}

	// Block until shutdown_event_ is set
	std::thread([this]()
	{
		block_input();
	}).detach();
#endif
}

HWND console::get_handle() const noexcept {
	return console_window_handle_;
}

void console::save_state()
{
	save_console_state(console_state_);
}

void console::restore_state()
{
	restore_console_state(console_state_);
}

void console::save_console_state(console_state& state)
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
	state.close_button = flags & (MF_DISABLED | MF_GRAYED);

	PLOGV << "         Title: " << state.title;
	PLOGV << "          Mode: " << state.mode;
	PLOGV << "  Close Button: " << state.close_button;
}

void console::restore_console_state(const console_state& state)
{
	{
		console_state current_state;
		PLOGV << "Current Console State -";

		// Save console title
		std::vector<wchar_t> buffer(MAX_PATH);
		GetConsoleTitle(buffer.data(), static_cast<DWORD>(buffer.size()));
		current_state.title.assign(buffer.data());

		// Save console mode
		GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &current_state.mode);

		// Save system menu and close button state
		const auto flags = GetMenuState(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		current_state.close_button = flags & (MF_DISABLED | MF_GRAYED);

		PLOGV << "         Title: " << current_state.title;
		PLOGV << "          Mode: " << current_state.mode;
		PLOGV << "  Close Button: " << current_state.close_button;
	}

	// Restore console title
	SetConsoleTitle(state.title.c_str());

	// Restore console mode
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(input, state.mode);

	// Restore system menu and close button state
	if (const auto system_menu = GetSystemMenu(GetConsoleWindow(), FALSE); system_menu != nullptr) {
		EnableMenuItem(system_menu, SC_CLOSE, state.close_button);
	}

	{
		console_state restored_state;
		PLOGV << "Restored Console State -";

		// Save console title
		std::vector<wchar_t> buffer(MAX_PATH);
		GetConsoleTitle(buffer.data(), static_cast<DWORD>(buffer.size()));
		restored_state.title.assign(buffer.data());

		// Save console mode
		GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &restored_state.mode);

		// Save system menu and close button state
		const auto flags = GetMenuState(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		restored_state.close_button = flags & (MF_DISABLED | MF_GRAYED);

		PLOGV << "         Title: " << restored_state.title;
		PLOGV << "          Mode: " << restored_state.mode;
		PLOGV << "  Close Button: " << restored_state.close_button;
	}
}

#if CONSOLE_SHUTDOWN_EVENT
void console::block_input() noexcept
{
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE handles[2] = {
		input, shutdown_event_
	};

	while (true) {
		DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (result == WAIT_OBJECT_0 + 1) {
			break;
			// shutdown_event_ is signaled
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
	CloseHandle(shutdown_event_);
	shutdown_event_ = nullptr;
}
#endif

void console::shutdown()
{
	PLOGI << "Shutdown Console";
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

	PLOGI << "Restore Console State";
	restore_state();

	logging::shutdown_console_logger();

	FreeConsole();

#if CONSOLE_SHUTDOWN_EVENT
	// Signal the shutdown event
	if (shutdown_event_)
	{
		SetEvent(shutdown_event_);
	}
#endif
}

console::~console() noexcept
{
	PLOGD << "Destroy Console";
#if CONSOLE_SHUTDOWN_EVENT
	if (shutdown_event_)
	{
		CloseHandle(shutdown_event_);
		shutdown_event_ = nullptr;
	}
#endif
}

BOOL console::ctrl_handler(const DWORD ctrl_type) noexcept
{
	reinterpret_cast<console*>(GetWindowLongPtr(GetConsoleWindow(), GWLP_USERDATA));  // NOLINT(clang-diagnostic-unused-value, performance-no-int-to-ptr)
	switch (ctrl_type)
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
