#pragma once
#include "AtumWindows.h"
#include <cstdio>
//#include <memory>
//#include <signal.h>

class Console {
public:
	// singleton
	class ConsoleClass {
	public:
		static const LPCWSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		ConsoleClass() noexcept;
		~ConsoleClass();
		ConsoleClass(const ConsoleClass&) = delete;
		ConsoleClass& operator=(const ConsoleClass&) = delete;
		static constexpr LPCWSTR conClassName = L"Atum.D3D.Console";
		static ConsoleClass conClass;
		HINSTANCE hInst;
	};

public:
	Console(const LPCWSTR name) noexcept;
	~Console() noexcept;
	Console(const Console&) = delete;
	Console& operator=(const Console&) = delete;
	HWND GetWindow() const noexcept;
private:
	HWND hWnd;
	FILE* pCin = nullptr;
	FILE* pCout = nullptr;
	FILE* pCerr = nullptr;
	static WNDPROC glpfnConsoleWindow;
	static BOOL CALLBACK CtrlHandler(DWORD fdwCtrlType) noexcept;
};
