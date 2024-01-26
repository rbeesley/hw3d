#include "Window.h"
#include "WindowsMessageMap.h"
#include "Logging.h"

Window::WindowClass Window::WindowClass::wndClass;
const static WindowsMessageMap wmm;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::WindowClass::WindowClass()\n");
#endif // CALLTRACING
	WNDCLASSEX wcex = {
		.style = CS_OWNDC,
		.lpfnWndProc = HandleMsgSetup,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = GetInstance(),
		.hIcon = nullptr,
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = GetName(),
		.hIconSm = nullptr
	};
	wcex.cbSize = sizeof(wcex);

	RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::WindowClass::~WindowClass()\n");
#endif // CALLTRACING
	PLOGV << "Window::WindowClass::~WindowClass()";
	UnregisterClass(wndClassName, GetInstance());
}

const LPCWSTR Window::WindowClass::GetName() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::WindowClass::GetName()\n");
#endif // CALLTRACING
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::WindowClass::GetInstance()\n");
#endif // CALLTRACING
	return wndClass.hInst;
}

Window::Window(int width, int height, const LPCWSTR name) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::Window()\n");
#endif // CALLTRACING
	RECT wr {
		.left = 100,
		.top = 100,
		.right = width + wr.left,
		.bottom = height + wr.top
	};

	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	// Show Window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

HWND Window::GetHandle() const
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::GetHandle()\n");
#endif // CALLTRACING
	return hWnd;
}

Window::~Window()
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::~Window()\n");
#endif // CALLTRACING
	Logging::GetLogger()->RemoveDebugOutput();
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::HandleMsgSetup()\n");
#endif // CALLTRACING
	//PLOGV_(Logging::kConsole) << wmm(msg, lParam, wParam).c_str();
	PLOGV << wmm(msg, lParam, wParam).c_str();

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::HandleMsgThunk()\n");
#endif // CALLTRACING
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#ifdef CALLTRACING
	OutputDebugString(L" ** Window::HandleMsg()\n");
#endif // CALLTRACING
	int wmId, wmEvent;

	//PLOGV_(Logging::kConsole) << wmm(msg, lParam, wParam).c_str();
	PLOGV << wmm(msg, lParam, wParam).c_str();

	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case 0:
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		break;
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}