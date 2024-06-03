#pragma once
#include <sstream>

#include "AtumException.h"
#include "AtumWindows.h"
#include "DeviceConfig.h"
#include "IKeyboard.h"
#include "IMouse.h"

class window
{
private:
	// singleton
	class window_class {
	public:
		static LPCWSTR get_name() noexcept;
		static HINSTANCE get_instance() noexcept;
		window_class(const window_class&) = delete;
		window_class& operator=(const window_class&) = delete;
		window_class(const window_class&&) = delete;
		window_class& operator=(const window_class&&) = delete;
	private:
		window_class() noexcept;
		~window_class();
		static constexpr LPCWSTR window_class_name = L"Atum.D3D";
		static window_class window_class_;
		HINSTANCE instance_handle_;
	};
public:
	class exception final : public atum_exception {
	public:
		exception(int line, const char* file, HRESULT result) noexcept;
		const char* what() const noexcept override;
		const char* get_type() const noexcept override;
		static std::string translate_error_code(HRESULT result) noexcept;
		HRESULT get_error_code() const noexcept;
		std::string get_error_string() const noexcept;
	private:
		HRESULT result_;
	};
public:
	window(int width, int height, LPCWSTR name);
	~window();
	[[nodiscard]] HWND get_handle() const;
	window(const window&) = delete;
	window& operator=(const window&) = delete;
	window(const window&&) = delete;
	window& operator=(const window&&) = delete;
private:
	void set_title(const std::wstring& title) const;
	static LRESULT CALLBACK handle_msg_setup(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
	static LRESULT CALLBACK handle_msg_thunk(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
	static HWND set_active(HWND window_handle);
	static LRESULT CALLBACK handle_msg(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
private:
	inline static device_config config_;
	inline static i_keyboard* keyboard_ = &(config_.get_keyboard());
	inline static i_mouse* mouse_ = &(config_.get_mouse());
	int width_{};
	int height_{};
	HWND window_handle;
};

// Error exception helper macro
#define ATUM_WND_EXCEPT(h_result) window::exception(__LINE__, __FILE__, h_result)
#define ATUM_WND_LAST_EXCEPT() window::exception(__LINE__, __FILE__, GetLastError())
