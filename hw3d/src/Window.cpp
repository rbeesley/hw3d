#include "DefinesConfig.h"
#include "Window.h"
#include "Logging.h"
#include "Resources/resource.h"
#include <system_error>

#ifdef LOG_WINDOW_MESSAGES // defined in DefinesConfig.h
#include "WindowsMessageMap.h"

const static windows_message_map windows_message_map;
#endif

window::window_class window::window_class::window_class_;

window::window_class::window_class() noexcept
	:
	instance_handle_(GetModuleHandle(nullptr))
{
	WNDCLASSEX wcex = {
		.style = CS_OWNDC,
		.lpfnWndProc = handle_msg_setup,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = get_instance(),
		.hIcon = static_cast<HICON>(LoadImage(instance_handle_, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), 0)),
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = get_name(),
		.hIconSm = static_cast<HICON>(LoadImage(instance_handle_, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0))
	};
	wcex.cbSize = sizeof(wcex);

	RegisterClassEx(&wcex);
}

window::window_class::~window_class()
{
	UnregisterClass(window_class_name, get_instance());
}

LPCWSTR window::window_class::get_name() noexcept
{
	return window_class_name;
}

HINSTANCE window::window_class::get_instance() noexcept
{
	return window_class_.instance_handle_;
}

window::window(const int width, const int height, const LPCWSTR name)
	:
	width_(width),
	height_(height)
{
	RECT window_rect{};
	window_rect.left = 100;
	window_rect.top = 100;
	window_rect.right = width_ + window_rect.left;
	window_rect.bottom = height_ + window_rect.top;

	if (!AdjustWindowRect(&window_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	window_handle = CreateWindowEx(
		0,
		window_class::get_name(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		nullptr,
		nullptr,
		window_class::get_instance(),
		this
	);

	if (nullptr == window_handle)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// Show Window
	ShowWindow(window_handle, SW_SHOWDEFAULT);
}

HWND window::get_handle() const
{
	return window_handle;
}

window::~window()
{
	DestroyWindow(window_handle);
}

LRESULT CALLBACK window::handle_msg_setup(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in DefinesConfig.h
	PLOGV << windows_message_map(msg, l_param, w_param).c_str();
#endif

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const p_create = reinterpret_cast<CREATESTRUCTW*>(l_param);
		window* const p_wnd = static_cast<window*>(p_create->lpCreateParams);
		SetWindowLongPtr(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_wnd));
		SetWindowLongPtr(window_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handle_msg_thunk));
		return handle_msg(window_handle, msg, w_param, l_param);
	}
	return DefWindowProc(window_handle, msg, w_param, l_param);
}

LRESULT CALLBACK window::handle_msg_thunk(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
	reinterpret_cast<window*>(GetWindowLongPtr(window_handle, GWLP_USERDATA));
	return handle_msg(window_handle, msg, w_param, l_param);
}

LRESULT CALLBACK window::handle_msg(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
	int window_message_id, window_message_event;

#ifdef LOG_WINDOW_MESSAGES // defined in DefinesConfig.h
	PLOGV << windows_message_map(msg, l_param, w_param).c_str();
#endif

	switch (msg)
	{
	/* Window */
	case WM_CREATE:
		break;
	case WM_COMMAND:
		window_message_id = LOWORD(w_param);
		window_message_event = HIWORD(w_param);
		switch (window_message_id)
		{
		case 0:
		default:
			return DefWindowProc(window_handle, msg, w_param, l_param);
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KILLFOCUS:
		keyboard_.clear_state();
		break;
	/* Keyboard */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		// Filter keyboard autorepeat
		if (!(l_param & 0x40000000 /* previous key state */) || keyboard_.is_autorepeat_enabled())
		{
			keyboard_.on_key_pressed(static_cast<unsigned char>(w_param));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard_.on_key_released(static_cast<unsigned char>(w_param));
		break;
	case WM_CHAR:
		keyboard_.on_char(static_cast<unsigned char>(w_param));
		break;
	/* Default */
	default:
		return DefWindowProc(window_handle, msg, w_param, l_param);
	}
	return 0;
}

window::exception::exception(const int line, const char* file, const HRESULT result) noexcept
	:
	atum_exception(line, file),
	result_(result)
{}

const char* window::exception::what() const noexcept
{
	std::ostringstream out;
	out << get_type() << "\n"
		<< "[Error Code] " << get_error_code() << "\n"
		<< "[Description] " << get_error_string() << "\n"
		<< get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* window::exception::get_type() const noexcept
{
	return "Atum Window Exception";
}

std::string window::exception::translate_error_code(const HRESULT result) noexcept
{
	std::string message = std::system_category().message(result);
	return message;
}

HRESULT window::exception::get_error_code() const noexcept
{
	return result_;
}

std::string window::exception::get_error_string() const noexcept
{
	return translate_error_code(result_);
}
