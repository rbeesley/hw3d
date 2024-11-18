#include "Window.h"

#include <system_error>

#include "Console.h"
#include "imgui.h"
#include "Logging.h"
#include "Resources/resource.h"
#include "WindowsThrowMacros.h"
#include "backends/imgui_impl_win32.h"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include "WindowsMessageMap.h"

const static class windows_message_map windows_message_map;
#endif

// Definition of static member variables
unsigned int window::target_width_ = 0;
unsigned int window::target_height_ = 0;
bool window::in_sizemove_ = false;
bool window::minimized_ = false;

window::window_class::window_class()
	: instance_handle_(GetModuleHandle(nullptr))
{
	PLOGV << "Instantiate Window Class";
}

void window::window_class::initialize() const noexcept
{
	PLOGD << "Initialize Window Class";

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

void window::window_class::shutdown() const noexcept
{
	PLOGD << "Shutdown Window Class";
	UnregisterClass(window_class_name, get_instance());
}

window::window_class::~window_class()
{
	PLOGV << "Destroy Window Class";
}

LPCWSTR window::window_class::get_name() noexcept
{
	return window_class_name;
}

HINSTANCE window::window_class::get_instance() const noexcept
{
	return instance_handle_;
}

void window::initialize(const int width, const int height, const LPCWSTR name)
{
	PLOGI << "Initialize Window";

	width_ = width;
	height_ = height;

	window_class_ = std::make_unique<window_class>();
	window_class_->initialize();

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
		window_class_->get_instance(),
		this
	);

	// Check for an error
	if (nullptr == window_handle_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// Newly created windows start off as hidden
	PLOGI << "Show the Window";
	ShowWindow(window_handle_, SW_SHOWDEFAULT);
	UpdateWindow(window_handle_);

	// Configure Dear ImGui
	PLOGI << "Configure Dear ImGui";

	// Setup Dear ImGui context
	PLOGV << "IMGUI_CHECKVERSION();";
	IMGUI_CHECKVERSION();
	PLOGV << "ImGui::CreateContext();";
	ImGui::CreateContext();
	PLOGD << "Set Dear ImGui flags";
	ImGuiIO& im_gui_io = ImGui::GetIO();
	im_gui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	im_gui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_DOCKING
	im_gui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	im_gui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.
#endif

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

#ifdef IMGUI_DOCKING
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (im_gui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
#endif

	PLOGI << "Setup Dear ImGui Platform backend";
	PLOGV << "ImGui_ImplWin32_Init()";
	ImGui_ImplWin32_Init(window_handle_);

	// Get the mouse and keyboard
	PLOGD << "Create references to the mouse and keyboard";
	p_mouse_ = std::make_unique<mouse>();
	p_keyboard_ = std::make_unique<keyboard>();

	// Create the graphics object
	PLOGD << "Create the DirectX graphics object";
	PLOGV << "p_graphics_ = std::make_unique<graphics>(window_handle_, width_, height_);";
	p_graphics_ = std::make_unique<graphics>(window_handle_, width_, height_);

	// Check for an error
	if (nullptr == p_graphics_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	if(target_width_ == width_ && target_height_ == height_)
	{
		set_target_dimensions(0, 0);
	}
}

HWND window::get_handle()
{
	return window_handle_;
}

void window::shutdown() const
{
	PLOGI << "Shutdown Window";
	PLOGV << "p_graphics_->shutdown();";
	p_graphics_->shutdown();
	PLOGV << "ImGui_ImplWin32_Shutdown();";
	ImGui_ImplWin32_Shutdown();
	window_class_->shutdown();
}

window::~window()
{
	PLOGD << "Destroy Window";
	PLOGV << "ImGui::DestroyContext()";
	ImGui::DestroyContext();
	DestroyWindow(window_handle_);
}

void window::set_title(const std::wstring& title)
{
	if (!SetWindowText(window_handle_, title.c_str()))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}
}

void window::set_target_dimensions(const unsigned int width, const unsigned int height)
{
	PLOGV << "set_target_dimensions(width: " << width << ", height: " << height << ");";
	target_width_ = width;
	target_height_ = height;
}

window::window_dimensions window::get_target_dimensions()
{
	PLOGV << "get_target_dimensions() : width: " << target_width_ << ", height: " << target_height_ << "";
	return {target_width_, target_height_};
}

std::optional<unsigned int> window::process_messages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.message;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

mouse* window::get_mouse()
{
	if (!p_mouse_)
	{
		throw ATUM_WND_NO_GFX_EXCEPT();
	}
	return p_mouse_.get();
}

keyboard* window::get_keyboard()
{
	if (!p_keyboard_)
	{
		throw ATUM_WND_NO_GFX_EXCEPT();
	}
	return p_keyboard_.get();
}

graphics* window::get_graphics()
{
	if (!p_graphics_)
	{
		throw ATUM_WND_NO_GFX_EXCEPT();
	}
	return p_graphics_.get();
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param);
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
	//PLOGV << "ImGui_ImplWin32_WndProcHandler";
	if(ImGui_ImplWin32_WndProcHandler(window_handle, msg, w_param, l_param))
	{
		return true;
	}

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
		// ReSharper disable once CppDeclaratorNeverUsed
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
	case WM_SIZE:
		if (w_param == SIZE_MINIMIZED)
		{
			// The window was minimized (you should probably suspend the application)
			if (!minimized_)
			{
				minimized_ = true;
			}
		}
		else if (minimized_)
		{
			// The window was minimized and is now restored (resume from suspend)
			minimized_ = false;
		}
		else if (!in_sizemove_)
		{
			// Handle the swapchain resize for maximize or unmaximize
			set_target_dimensions(LOWORD(l_param), HIWORD(l_param));
		}
		break;
	case WM_ENTERSIZEMOVE:
		// We want to avoid trying to resizing the swapchain as the user does the 'rubber band' resize
		in_sizemove_ = true;
		break;
	case WM_EXITSIZEMOVE:
		// Here is the other place where you handle the swapchain resize after the user stops using the 'rubber-band'
		in_sizemove_ = false;
		break;
	case WM_GETMINMAXINFO:
	{
		// We want to prevent the window from being set too tiny
		const auto info = reinterpret_cast<MINMAXINFO*>(l_param);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
	}
	break;
	case WM_SYSCOMMAND:
		if ((w_param & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
#ifdef IMGUI_DOCKING
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const int dpi = HIWORD(w_param);
			PLOGI << "WM_DPICHANGED to " << dpi << "(" << static_cast<float>(dpi) / 96.0f * 100.0f << ")";
			const RECT* suggested_rect = reinterpret_cast<RECT*>(l_param);
			::SetWindowPos(window_handle_, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
#endif

		/* Keyboard */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (const auto& io = ImGui::GetIO(); io.WantCaptureKeyboard)
		{
			return 0;
		}
		// Filter keyboard autorepeat
		if (!(l_param & 0x40000000 /* previous key state */) || p_keyboard_->is_autorepeat_enabled())
		{
			p_keyboard_->on_key_pressed(static_cast<unsigned char>(w_param));
		}
		break;
	}
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
			else if (p_mouse_->is_in_window())
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
	default: break;
	}
	return DefWindowProc(window_handle, msg, w_param, l_param);
}

window::hresult_exception::hresult_exception(int line, const char* file, HRESULT hresult) noexcept
	:
	exception(line, file),
	hresult_(hresult)
{}

const char* window::hresult_exception::what() const noexcept
{
	std::ostringstream out;
	out << "[Error Code] " << get_error_code() << "\n"
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
