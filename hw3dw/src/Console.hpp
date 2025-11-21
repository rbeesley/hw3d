#pragma once
#include <cstdio>
#include <string>

#include "AtumWindows.hpp"

#define CONSOLE_SHUTDOWN_EVENT TRUE

class Console {
public:
	// singleton
	class ConsoleClass {
	public:
		static LPCWSTR getName() noexcept;
		static HINSTANCE getInstance() noexcept;
		ConsoleClass(const ConsoleClass&) = delete;
		ConsoleClass& operator=(const ConsoleClass&) = delete;
		ConsoleClass(const ConsoleClass&&) = delete;
		ConsoleClass& operator=(const ConsoleClass&&) = delete;
	private:
		ConsoleClass() noexcept;
		~ConsoleClass() noexcept;
		static constexpr LPCWSTR consoleClassName_ = L"Atum.D3D.Console";
		static ConsoleClass consoleClass_;
		HINSTANCE instanceHandle_;
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
	static bool freeConsole_;
	static ConsoleState consoleState_;
	static BOOL CALLBACK ctrlHandler(DWORD ctrl_type) noexcept;
	static void saveConsoleState(ConsoleState& state);
	static void restoreConsoleState(const ConsoleState& state);
};
