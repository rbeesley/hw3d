#pragma once
#include <cstdio>
#include <string>

#define CONSOLE_
#include "AtumWindows.h"

#define CONSOLE_SHUTDOWN_EVENT FALSE

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
		~console_class() noexcept;
		static constexpr LPCWSTR console_class_name = L"Atum.D3D.Console";
		static console_class console_class_;
		HINSTANCE instance_handle_;
	};

public:
	console() noexcept;
	~console() noexcept;
	console(const console&) = delete;
	console& operator=(const console&) = delete;
	console(const console&&) = delete;
	console& operator=(const console&&) = delete;

	void initialize(LPCWSTR name) noexcept;
	HWND get_handle() const noexcept;
	static void save_state();
	static void restore_state();
	static void shutdown();
private:
	struct console_state {
		std::wstring title;
		DWORD mode;
		UINT close_button;
	};

	HWND console_window_handle_;
	FILE* p_cin_ = nullptr;
	FILE* p_cout_ = nullptr;
	FILE* p_cerr_ = nullptr;
#if CONSOLE_SHUTDOWN_EVENT
	HANDLE shutdown_event_;
	void block_input() noexcept;
#endif
	static BOOL CALLBACK ctrl_handler(DWORD ctrl_type) noexcept;
	static console_state console_state_;
	static void save_console_state(console_state& state);
	static void restore_console_state(const console_state& state);
};
