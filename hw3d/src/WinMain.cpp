#include "Window.h"
#include "Console.h"
#include "Logging.h"
#include <tchar.h>

// Global Variables:
HINSTANCE root_instance; // current instance

int APIENTRY WinMain(
	_In_ const HINSTANCE instance,
	_In_opt_ const HINSTANCE previous_instance,
	_In_ const LPSTR command_line,
	_In_ const int show_flags)
{
	UNREFERENCED_PARAMETER(previous_instance);
	UNREFERENCED_PARAMETER(command_line);
	UNREFERENCED_PARAMETER(show_flags);
	try
	{
		root_instance = instance;

		// Initialize logging
		//logging log(plog::info);
		logging log(plog::debug);
		//logging log(plog::verbose);

		// Set up DebugOutput Logger
		//logging::init_debug_output(plog::fatal);
		//logging::init_debug_output(plog::error);
		logging::init_debug_output(plog::warning);
		//logging::init_debug_output(plog::info);
		//logging::init_debug_output(plog::debug);
		//logging::init_debug_output(plog::verbose);

		// Create Window
		PLOGI << "Creating Window";
		const window window(640, 360, TEXT("Atum D3D Window"));
		if (!window.get_handle())
		{
			PLOGF << "Failed to create Window";
			return -2;
		}

#ifdef _DEBUG
		// Create Console
		PLOGI << "Creating Debug Console";
		const console console(TEXT("Debug Console"));
		if (!console.get_window_handle())
		{
			PLOGE << "Failed to create Debug Console";
		}

		// Set up Console Logger
		//logging::init_console(plog::info);
		logging::init_console(plog::debug);
		//logging::init_console(plog::verbose);

		// Check Logging
		PLOG_VERBOSE << "This is a VERBOSE message";
		PLOG_DEBUG << "This is a DEBUG message";
		PLOG_INFO << "This is an INFO message";
		PLOG_WARNING << "This is a WARNING message";
		PLOG_ERROR << "This is an ERROR message";
		PLOG_FATAL << "This is a FATAL message";
		//PLOG_NONE << "This is a NONE message";
#endif // _DEBUG

		// Start Window Message Pump
		PLOGI << "Starting Message Pump";
		MSG msg;
		BOOL result;
		while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (result == -1)
		{
			return -1;
		}

		PLOGI << "Closing application";
		return static_cast<int>(msg.wParam);
	}
	//BUGBUG : Should be using MessageBox and adjusting text based on target encoding. Currently assuming ASCII to match output of exception.what().
	catch (const atum_exception& e) {
		PLOGF << e.get_type() << ":";
		PLOGF << "    " << e.what();
		MessageBoxA(nullptr, e.what(), e.get_type(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		PLOGF << "Standard Exception:";
		PLOGF << "    " << e.what();
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		PLOGF << "Unknown Exception:";
		PLOGF << "No further details about the exception are available.";
		MessageBox(nullptr, TEXT("No details available"), TEXT("Unknown Exception"), MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
