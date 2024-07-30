#pragma once
#include <optional>
#include <sstream>
#include <memory>

#include "AtumException.h"
#include "AtumWindows.h"
#include "DeviceConfig.h"
#include "IKeyboard.h"
#include "IMouse.h"
#include "Graphics.h"

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
	class exception : public atum_exception {
		using atum_exception::atum_exception;
	public:
		static std::string translate_error_code(HRESULT result) noexcept;
	private:
		HRESULT result_;
	};
public:
	class hresult_exception : public exception
	{
	public:
		hresult_exception(int line, const char* file, HRESULT hresult) noexcept;
		const char* what() const noexcept override;
		const char* get_type() const noexcept override;
		HRESULT get_error_code() const noexcept;
		std::string get_error_description() const noexcept;
	private:
		HRESULT hresult_;
	};
	class no_gfx_exception : public exception
	{
	public:
		using exception::exception;
		const char* get_type() const noexcept override;

	};
public:
	window(int width, int height, LPCWSTR name);
	~window();
	[[nodiscard]] HWND get_handle() const;
	static std::optional<int> process_messages();
	graphics& get_graphics() const;
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
	HWND window_handle_;
	std::unique_ptr<graphics> p_graphics_;
};

// Error exception helper macro
#define ATUM_WND_EXCEPT(h_result) window::hresult_exception(__LINE__, __FILE__, h_result)
#define ATUM_WND_LAST_EXCEPT() window::hresult_exception(__LINE__, __FILE__, GetLastError())
#define ATUM_WND_NO_GFX_EXCEPT() window::no_gfx_exception(__LINE__, __FILE__)