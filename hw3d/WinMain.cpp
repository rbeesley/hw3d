#include "Window.h"
#include "Console.h"
#include "Logging.h"

// Global Variables:
HINSTANCE g_hInst; // current instance

int APIENTRY WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR lpCmdLine,
	_In_     int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	g_hInst = hInstance;

	// Initialize logging
	Logging log(plog::verbose);
	//log.InitDebugOutput(plog::info);
	log.InitDebugOutput(plog::verbose);

	// Create Console
	PLOGI << "Creating Debug Console";
	Console console(L"Atum D3D Debug Console");
	//log.InitConsole(plog::debug);
	log.InitConsole(plog::verbose);

	// Check Logging
	PLOG_VERBOSE << "This is a VERBOSE message";
	PLOG_DEBUG << "This is a DEBUG message";
	PLOG_INFO << "This is an INFO message";
	PLOG_WARNING << "This is a WARNING message";
	PLOG_ERROR << "This is an ERROR message";
	PLOG_FATAL << "This is a FATAL message";

	// Create Window
	PLOGI << "Creating Window";
	Window wnd(800, 300, L"Atum D3D Window");
	if (!wnd.GetHandle())
	{
		PLOGF << "Failed to create Window";
		return -2;
	}

	// Start Window Message Pump
	PLOGI << "Starting Message Pump";
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//console.RunMessagePump();
	}

	if (gResult == -1)
	{
		return -1;
	}

	PLOGI << "Closing application";
	return static_cast<int>(msg.wParam);
}
