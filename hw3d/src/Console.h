#pragma once
#include <cstdio>

#include "AtumWindows.h"

class console {
public:
	// singleton
	class console_class {
	public:
		static LPCWSTR get_name() noexcept;
		static HINSTANCE get_instance() noexcept;
		console_class(const console_class&) = delete;
		console_class& operator=(const console_class&) = delete;
		console_class(const console_class&&) = delete;
		console_class& operator=(const console_class&&) = delete;
	private:
		console_class() noexcept;
		~console_class();
		static constexpr LPCWSTR console_class_name = L"Atum.D3D.Console";
		static console_class console_class_;
		HINSTANCE instance_handle_;
	};

public:
	HWND get_window_handle() const noexcept;
	explicit console(LPCWSTR name) noexcept;
	~console() noexcept = default;
	console(const console&) = delete;
	console& operator=(const console&) = delete;
	console(const console&&) = delete;
	console& operator=(const console&&) = delete;
private:
	HWND window_handle;
	FILE* p_cin_ = nullptr;
	FILE* p_cout_ = nullptr;
	FILE* p_cerr_ = nullptr;
	static WNDPROC console_window_proc_;
	static BOOL CALLBACK ctrl_handler(DWORD ctrl_type) noexcept;
};
