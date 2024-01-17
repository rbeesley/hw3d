#include "Window.h"
#include "Logging.h"

int WINAPI WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR lpCmdLine,
	_In_     int nCmdShow)
{
	// Initialize logging
	Logging::initLog();

	// Create Window
	PLOGI << "Creating Window";
	Window wnd(800, 300, L"Atum D3D Window");

	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1)
	{
		return -1;
	}

	PLOGI << "Closing application";
	return static_cast<int>(msg.wParam);
}
