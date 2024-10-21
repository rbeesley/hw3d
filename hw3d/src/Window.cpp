#include "Window.h"

#include <system_error>

#include "AtumWindows.h"
#include "LoggingConfig.h"
#include "Logging.h"
#include "Resources/resource.h"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
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
	PLOGI << "Initializing Window";

	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	RECT window_rect{};
	window_rect.left = 100;
	window_rect.top = 100;
	window_rect.right = width_ + window_rect.left;
	window_rect.bottom = height_ + window_rect.top;

	if (!AdjustWindowRect(&window_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	window_handle_ = CreateWindowEx(
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

	// Check for an error
	if (nullptr == window_handle_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// Newly created windows start off as hidden
	ShowWindow(window_handle_, SW_SHOWDEFAULT);

	// Get the mouse and keyboard
	p_mouse_ = std::make_shared<mouse>();
	p_keyboard_ = std::make_shared<keyboard>();

	// Create the graphics object
	p_graphics_ = std::make_shared<graphics>(window_handle_, width_, height_);

	// Check for an error
	if (nullptr == p_graphics_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}
}

HWND window::get_handle() const
{
	return window_handle_;
}

window::~window()
{
	DestroyWindow(window_handle_);
}

void window::set_title(const std::wstring& title) const
{
	if(!SetWindowText(window_handle_, title.c_str()))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}
}

std::optional<int> window::process_messages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return static_cast<int>(msg.wParam);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

std::shared_ptr<mouse> window::get_mouse()
{
	return p_mouse_;
}

std::shared_ptr<keyboard> window::get_keyboard()
{
	return p_keyboard_;
}

std::shared_ptr<graphics> window::get_graphics()
{
	return p_graphics_;
}

LRESULT CALLBACK window::handle_msg_setup(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windows_message_map(msg, l_param, w_param).c_str();
#endif

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const p_create = reinterpret_cast<CREATESTRUCTW*>(l_param);  // NOLINT(performance-no-int-to-ptr)
		window* const p_wnd = static_cast<window*>(p_create->lpCreateParams);
		SetWindowLongPtr(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_wnd));
		SetWindowLongPtr(window_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handle_msg_thunk));
		return handle_msg(window_handle, msg, w_param, l_param);
	}
	return DefWindowProc(window_handle, msg, w_param, l_param);
}

LRESULT CALLBACK window::handle_msg_thunk(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
	return handle_msg(window_handle, msg, w_param, l_param);
}

HWND window::set_active(const HWND window_handle)
{
	if (p_mouse_->is_in_window())
	{
		if (const HWND active_window = GetActiveWindow(); active_window != window_handle)
		{
			return SetActiveWindow(window_handle);
		}
	}

	return nullptr;
}

LRESULT CALLBACK window::handle_msg(const HWND window_handle, const UINT msg, const WPARAM w_param, const LPARAM l_param) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windows_message_map(msg, l_param, w_param).c_str();
#endif
#ifdef LOG_WINDOW_MOUSE_MESSAGES
	if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
	{
		PLOGV << windows_message_map(msg, l_param, w_param).c_str();
	}
#endif

	switch (msg)
	{
	/* Window */
	case WM_CREATE:
		{
			RECT rect;
			GetWindowRect(window_handle, &rect);
			x_ = rect.left;
			y_ = rect.top;
			break;
		}
	case WM_COMMAND:
		{
			const int window_message_id = LOWORD(w_param);
			const int window_message_event = HIWORD(w_param);
			switch (window_message_id)
			{
				case 0:
				default:
				return DefWindowProc(window_handle, msg, w_param, l_param);
			}
			break;
		}
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KILLFOCUS:
		p_keyboard_->clear_state();
		break;
	case WM_SHOWWINDOW:
		{
			if (w_param == TRUE) // The window is being shown
			{
				RECT rect;
				GetWindowRect(window_handle, &rect);
				x_ = rect.left;
				y_ = rect.top;
			}
			break;
		}
	case WM_MOVE:
		{
			const int x_pos = static_cast<short>(LOWORD(l_param)); // Horizontal position
			const int y_pos = static_cast<short>(HIWORD(l_param)); // Vertical position

			// Calculate how far the window has moved
			if (p_mouse_)
			{
				const int x_delta = x_pos - x_;
				const int y_delta = y_pos - y_;

#ifdef LOG_WINDOW_MOVEMENT_MESSAGES
				PLOGD << "Window moved: " << x_delta << ", " << y_delta;
#endif

				// Update the mouse position so that the elements in the window which are dependent on the mouse position, are updated to appear stationary
				p_mouse_->on_mouse_move(p_mouse_->pos().x - x_delta, p_mouse_->pos().y - y_delta);
			}

			// Store the new position for future calculations
			x_ = x_pos;
			y_ = y_pos;

			break;
		}

	/* Keyboard */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		// Filter keyboard autorepeat
		if (!(l_param & 0x40000000 /* previous key state */) || p_keyboard_->is_autorepeat_enabled())
		{
			p_keyboard_->on_key_pressed(static_cast<unsigned char>(w_param));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		p_keyboard_->on_key_released(static_cast<unsigned char>(w_param));
		break;
	case WM_CHAR:
		p_keyboard_->on_char(static_cast<unsigned char>(w_param));
		break;
	/* Mouse */
	case WM_MOUSEMOVE:
		{
			if (const auto win = reinterpret_cast<window*>(GetWindowLongPtr(window_handle, GWLP_USERDATA)))  // NOLINT(performance-no-int-to-ptr)
			{
				const auto [x, y] = MAKEPOINTS(l_param);

				// mouse is inside client region
				if (x >= 0 && x < win->width_ && y >= 0 && y < win->height_)
				{
					// but internal state is still outside client region
					if (!p_mouse_->is_in_window())
					{
						// fix the state
						p_mouse_->on_mouse_enter(x, y);
						SetCapture(window_handle);
					}
					else
					{
						p_mouse_->on_mouse_move(x, y);
					}
				}
				// mouse is outside client region and internal state places it inside client region
				else if(p_mouse_->is_in_window())
				{
					// because a button is being held down we want to keep track of the mouse position outside the client region boundaries
					if (p_mouse_->is_left_pressed() || p_mouse_->is_right_pressed() || p_mouse_->is_middle_pressed() || p_mouse_->is_x1_pressed() || p_mouse_->is_x2_pressed())
					{
						p_mouse_->on_mouse_move(x, y);
					}
					// but no buttons ARE being held down
					else
					{
						// fix the state
						ReleaseCapture();
						p_mouse_->on_mouse_leave();
					}
				}
			}
			break;
		}
	case WM_LBUTTONDOWN:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			set_active(window_handle);
			p_mouse_->on_left_pressed(x, y);
			break;
		}
	case WM_LBUTTONUP:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			p_mouse_->on_left_released(x, y);
			break;
		}
	case WM_RBUTTONDOWN:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			set_active(window_handle);
			p_mouse_->on_right_pressed(x, y);
			break;
		}
	case WM_RBUTTONUP:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			p_mouse_->on_right_released(x, y);
			break;
		}
	case WM_MBUTTONDOWN:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			set_active(window_handle);
			p_mouse_->on_middle_pressed(x, y);
			break;
		}
	case WM_MBUTTONUP:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			p_mouse_->on_middle_released(x, y);
			break;
		}
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			const int wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param);
			switch (msg)
			{
				case WM_MOUSEWHEEL: // Vertical mouse scroll wheel
					p_mouse_->on_v_wheel_delta(x, y, wheel_delta);
					break;
				case WM_MOUSEHWHEEL: // Horizontal mouse scroll wheel
					p_mouse_->on_h_wheel_delta(x, y, wheel_delta);
					break;
				default:
					break;
			}
			break;
		}
	case WM_XBUTTONDOWN:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			set_active(window_handle);
			switch GET_XBUTTON_WPARAM(w_param)
			{
				case XBUTTON1:
					p_mouse_->on_x1_pressed(x, y);
					break;
				case XBUTTON2:
					p_mouse_->on_x2_pressed(x, y);
					break;
				default:
					break;
			}
			break;
		}
	case WM_XBUTTONUP:
		{
			const auto [x, y] = MAKEPOINTS(l_param);
			switch GET_XBUTTON_WPARAM(w_param)
			{
				case XBUTTON1:
					p_mouse_->on_x1_released(x, y);
					break;
				case XBUTTON2:
					p_mouse_->on_x2_released(x, y);
					break;
				default:
					break;
			}
			break;
		}

	/* Default */
	default:
		return DefWindowProc(window_handle, msg, w_param, l_param);
	}
	return 0;
}

window::hresult_exception::hresult_exception(int line, const char* file, HRESULT hresult) noexcept
	:
	exception(line, file),
	hresult_(hresult)
{}

const char* window::hresult_exception::what() const noexcept
{
	std::ostringstream out;
	out	<< "[Error Code] " << get_error_code() << "\n"
		<< "[Description] " << get_error_description() << "\n"
		<< get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* window::hresult_exception::get_type() const noexcept
{
	return "Atum Window Exception";
}

std::string window::exception::translate_error_code(const HRESULT result) noexcept
{
	std::string message = std::system_category().message(result);
	return message;
}

HRESULT window::hresult_exception::get_error_code() const noexcept
{
	return hresult_;
}

std::string window::hresult_exception::get_error_description() const noexcept
{
	return translate_error_code(hresult_);
}

const char* window::no_gfx_exception::get_type() const noexcept
{
	return "Atum Window Exception [No Graphics]";
}
