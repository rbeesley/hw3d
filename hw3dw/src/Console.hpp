#pragma once
#include <cstdio>
#include <string>

#define CONSOLE_
#include "AtumWindows.hpp"

#define CONSOLE_SHUTDOWN_EVENT TRUE

class Console {
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
		static constexpr LPCWSTR s_console_class_name_ = L"Atum.D3D.Console";
		static console_class s_console_class_;
		HINSTANCE instance_handle_;
	};

public:
	explicit Console(HWND appWindowHandle, LPCWSTR name) noexcept;
	~Console() noexcept;
	Console(const Console&) = delete;
	Console& operator=(const Console&) = delete;
	Console(const Console&&) = delete;
	Console& operator=(const Console&&) = delete;

	HWND getHandle() const noexcept;
	static void saveState();
	static void restoreState();
	void shutdown() const;
private:
	struct ConsoleState {
		std::wstring title;
		DWORD mode{};
		UINT closeButton{};
	};

	static HWND appWindowHandle_;
	HWND consoleWindowHandle_;
	FILE* cin_ = nullptr;
	FILE* cout_ = nullptr;
	FILE* cerr_ = nullptr;
#if CONSOLE_SHUTDOWN_EVENT
	HANDLE shutdownEvent_;
	void blockInput() noexcept;
#endif
	static ConsoleState s_consoleState_;
	static BOOL CALLBACK ctrlHandler(DWORD ctrl_type) noexcept;
	static void saveConsoleState(ConsoleState& state);
	static void restoreConsoleState(const ConsoleState& state);
};
