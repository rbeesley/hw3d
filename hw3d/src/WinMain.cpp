#include <tchar.h>

#include "App.h"
#include "Logging.h"
#include "LoggingConfig.h"

// Global Variables:
HINSTANCE root_instance; // current instance

int APIENTRY WinMain(
	_In_ const HINSTANCE hInstance,
	_In_opt_ const HINSTANCE hPrevInstance,
	_In_ const LPSTR lpCmdLine,
	_In_ const int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);
	try
	{
		root_instance = hInstance;

#ifndef LOG_LEVEL_FULL // defined in LoggingConfig.h
		// Initialize logging
		//logging log(plog::none); // Effectively turns off logging
		//logging log(plog::fatal);
		//logging log(plog::error);
		//logging log(plog::warning);
		logging log(plog::info); // default
		//logging log(plog::debug);
		//logging log(plog::verbose);

		// Set up DebugOutput Logger
		//logging::init_debug_output(plog::fatal);
		//logging::init_debug_output(plog::error);
		//logging::init_debug_output(plog::warning);
		logging::init_debug_output(plog::info); // default
		//logging::init_debug_output(plog::debug);
		//logging::init_debug_output(plog::verbose);
#else
		logging log(plog::verbose);
		logging::init_debug_output(plog::verbose);
		#endif

		app app;
#ifdef _DEBUG


#ifndef LOG_LEVEL_FULL // defined in LoggingConfig.h
		// Set up Console Logger
		//logging::init_console(plog::info);
		logging::init_console(plog::debug); // default
		//logging::init_console(plog::verbose);
#else
		logging::init_console(plog::verbose);
#endif

		// Check Logging
		PLOG_VERBOSE << "This is a VERBOSE message";
		PLOG_DEBUG << "This is a DEBUG message";
		PLOG_INFO << "This is an INFO message";
		PLOG_WARNING << "This is a WARNING message";
		PLOG_ERROR << "This is an ERROR message";
		PLOG_FATAL << "This is a FATAL message";
		//PLOG_NONE << "This is a NONE message";
#endif

		if(const int result = app.init() < 0)
		{
			return result;
		}

		// Start Window Message Pump
		PLOGI << "Starting Message Pump";
		PLOGI << "Running App";
		const int result = app.run();

		PLOGI << "Closing App";
		return result;
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
