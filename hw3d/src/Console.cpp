#include "Console.h"

#include "Logging.h"

console::console_class console::console_class::console_class_;

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
{
	PLOGD << "Instantiate Console";
}


void console::initialize(const LPCWSTR name) noexcept {
	PLOGI << "Initialize Console";

	AllocConsole();
	console_window_handle_ = GetConsoleWindow();

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
	const HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(input, &mode);
	SetConsoleMode(input, mode | ENABLE_MOUSE_INPUT | ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);

	PLOGV << "Show the Console Window";
	ShowWindow(GetConsoleWindow(), SW_SHOW);

	// Set up Console Logger
	logging::initialize_console_logger(LOG_LEVEL_CONSOLE);
}

void console::shutdown()
{
	PLOGI << "Shutdown Console";
	logging::shutdown_console_logger();
	FreeConsole();
}

console::~console() noexcept
{
	PLOGD << "Destroy Console";
}

HWND console::get_handle() const noexcept {
	return console_window_handle_;
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
