#pragma once
#include <optional>
#include <sstream>
#include <memory>

#include "AtumException.h"

#define WINDOW_
#include "AtumWindows.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

class window
{
private:
	// singleton
	class window_class {
	public:
		window_class();
		~window_class();
		window_class(const window_class&) = delete;
		window_class& operator=(const window_class&) = delete;
		window_class(const window_class&&) = delete;
		window_class& operator=(const window_class&&) = delete;
		void initialize() const noexcept;
		void shutdown() const noexcept;
		static LPCWSTR get_name() noexcept;
		HINSTANCE get_instance() const noexcept;
	private:
		static constexpr LPCWSTR window_class_name = L"Atum.D3D";
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
	window() noexcept = default;
	~window();

	window(const window&) = delete;
	window& operator=(const window&) = delete;
	window(const window&&) = delete;
	window& operator=(const window&&) = delete;

	void initialize(int width, int height, LPCWSTR name);
	[[nodiscard]] static HWND get_handle();
	void shutdown() const;
	static std::optional<int> process_messages();
	[[nodiscard]] static mouse& get_mouse();
	[[nodiscard]] static keyboard& get_keyboard();
	[[nodiscard]] static graphics& get_graphics();
	static void set_title(const std::wstring& title);
private:
	static LRESULT CALLBACK handle_msg_setup(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
	static LRESULT CALLBACK handle_msg_thunk(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
	static HWND set_active(HWND window_handle);
	static LRESULT CALLBACK handle_msg(HWND window_handle, UINT msg, WPARAM w_param, LPARAM l_param) noexcept;
private:
	inline static std::unique_ptr<mouse> p_mouse_;
	inline static std::unique_ptr<keyboard> p_keyboard_;
	inline static std::unique_ptr<graphics> p_graphics_;
	inline static int x_{};
	inline static int y_{};
	int width_{};
	int height_{};
	std::unique_ptr<window_class> window_class_;
	static inline HWND window_handle_;
};
