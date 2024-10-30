#include <tchar.h>

#include "App.h"
#include "Logging.h"

// Global Variables:
HINSTANCE root_instance; // current instance

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
	try
	{
		root_instance = hInstance;

		// Initialize logging
		logging::initialize(LOG_LEVEL_DEFAULT);

		// Set up Debug Output Logger
		logging::initialize_debug_output_logger(LOG_LEVEL_DEBUG_OUTPUT);

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
