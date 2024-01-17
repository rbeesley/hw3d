#if FALSE
#include <iostream>
#include <Windows.h>
#include <TraceLoggingProvider.h> // Provide TraceLogging
//#include <Support/TraceLoggingDynamic.h>
#include <winmeta.h> // Define constants e.g. WINEVENT_LEVEL_VERBOSE
#include "WindowsMessageMap.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Formatters/TxtFormatter.h>

TRACELOGGING_DEFINE_PROVIDER(
	g_hEtwProvider,
	"Atum.Hw3d",
	// {f227de66-2e26-5050-d767-9295693d2343}
	(0xf227de66, 0x2e26, 0x5050, 0xd7, 0x67, 0x92, 0x95, 0x69, 0x3d, 0x23, 0x43));

enum
{
	kDebugOutput = 1,
	kConsole
};

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap wmm;
	PLOGV_(kConsole) << wmm(msg, lParam, wParam).c_str();

	std::ostringstream oss;

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		TraceLoggingWrite(
			g_hEtwProvider,
			"WndProc",
			TraceLoggingLevel(WINEVENT_LEVEL_INFO),
			TraceLoggingString("Quitting", "Message"));
		return 0;
		break;
	//case WM_KEYUP:
	//case WM_KEYDOWN:
	//	const int keyboardScanCode = (lParam >> 16) & 0x00ff;
	//	const int virtualKey = wParam;
	//	break;
	case WM_CHAR:
		//const int character = wParam & 0xff;
		SetWindowText(hWnd, std::to_wstring(wParam).c_str());
		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		oss << "(" << pt.x << ", " << pt.y << ")";
		PLOGV_(kConsole) << oss.str().c_str();
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR lpCmdLine,
	_In_     int nCmdShow)
{
	// Create a console window
	AllocConsole();
	FILE* pCout;
	freopen_s(&pCout, "CONOUT$", "w", stdout);

	// Use the ShowWindow() function to show the console window
	ShowWindow(GetConsoleWindow(), SW_SHOW);

	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	plog::init<kDebugOutput>(plog::info, &debugOutputAppender);

	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
	plog::init<kConsole>(plog::verbose, &consoleAppender);

	plog::init(plog::debug, plog::get<kDebugOutput>()).addAppender(plog::get<kConsole>());

	PLOGV << "verbose";
	PLOGD << "debug";
	PLOGI << "info";
	PLOGW << "warning";
	PLOGE << "error";
	PLOGF << "fatal";
	PLOGN << "none";

	//std::cout << "Hello, World!" << std::endl;

	TraceLoggingRegister(g_hEtwProvider);

	//TraceLoggingWrite(
	//	g_hEtwProvider,
	//	"WinMain",
	//	TraceLoggingLevel(WINEVENT_LEVEL_INFO), // Levels defined in <winmeta.h>
	//	TraceLoggingString(lpCmdLine, "lpCmdLine"), // field name is "lpCmdLine"
	//	TraceLoggingInt32(nCmdShow)); // field name is implicitly "nCmdShow"

	//tld::Provider provider("ATUM.hw3d", );
	//if (provider.IsEnabled(eventLevel, eventKeyword))
	//{
	//	Event event(provider, szEventName, eventLevel, eventKeyword);
	//	(add fields definitions and values);
	//	event.Write();
	//}

	// Register a Win32 window
	const auto pClassName = L"ATUM.hw3d";
	const auto pWinMainName = L"HW3D";

	WNDCLASSEX wc =
	{
		.style = CS_OWNDC,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszClassName = pClassName,
		.hIconSm = nullptr
	};
	wc.cbSize = sizeof(wc);
	RegisterClassEx(&wc);

	// Create a Win32 window
	HWND hWnd = CreateWindowEx(
		0,
		pClassName,
		pWinMainName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (hWnd == nullptr)
	{
		std::cout << "Failed to create a window." << std::endl;
		return 1;
	}

	ShowWindow(hWnd, SW_SHOW);

	// Use the UpdateWindow() function to update the window
	UpdateWindow(hWnd);

	// Use the GetMessage() function to retrieve messages from the message queue
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		// Use the TranslateMessage() function to translate virtual-key messages into character messages
		TranslateMessage(&msg);

		// Use the DispatchMessage() function to dispatch a message to a window procedure
		DispatchMessage(&msg);
	}

	UnregisterClass(pClassName, hInstance);
	TraceLoggingUnregister(g_hEtwProvider);

	if (-1 == gResult) {
		return gResult;
	}
	else {
		return static_cast<int>(msg.wParam);
	}
}
#endif