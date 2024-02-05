#include "Window.h"
#include "WindowsMessageMap.h"
#include "Logging.h"
#include "resource.h"

#define USE_SYSTEMERROR
#ifdef USE_SYSTEMERROR
#include <system_error>
#endif // USE_SYSTEMERROR

//#define USE_FORMATMESSAGE_H
#ifdef USE_FORMATMESSAGE_H
#include <FormatMessage.h>
#endif // USE_FORMATMESSAGE_H

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
		.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), 0)),
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = GetName(),
		.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0))
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

Window::Window(int width, int height, const LPCWSTR name)
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

	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	};

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

	if (nullptr == hWnd)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

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
	Logging::get()->RemoveDebugOutput();
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

Window::Exception::Exception(int line, const char* file, HRESULT hresult) noexcept
	:
	AtumException(line, file),
	hresult(hresult)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream out;
	out << GetType() << "\n"
		<< "[Error Code] " << GetErrorCode() << "\n"
		<< "[Description] " << GetErrorString() << "\n"
		<< GetOriginString();
	whatBuffer = out.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Atum Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hresult) noexcept
{
#ifdef USE_SYSTEMERROR
	std::string message = std::system_category().message(hresult);
	return message;
#endif // USE_SYSTEMERROR

#ifdef USE_FORMATMESSAGE_H
	std::string s;
#ifdef UNICODE
	std::wstring w;
	w = CFormatMessage(hresult).MessageText();
	s = std::string(w.begin(), w.end());
#else
	s = CFormatMessage(hresult).MessageText();
#endif
	return s;
#endif // USE_FORMATMESSAGE_H

#if !(defined(USE_SYSTEMERROR) || defined(USE_FORMATMESSAGE_H))
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		// use system message tables to retrieve error text
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		// allocate buffer on local heap for error text
		FORMAT_MESSAGE_FROM_SYSTEM |
		// Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
		FORMAT_MESSAGE_IGNORE_INSERTS |
		// ignore regular line breaks in the message definition text
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		nullptr,
		hresult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(pMsgBuf),
		0,
		nullptr
	);
	if (nMsgLen == 0) {
		return "Unidetified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
#endif // !defined(USE_SYSTEMERROR) || !defined(USE_FORMATMESSAGE_H)
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hresult;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hresult);
}
