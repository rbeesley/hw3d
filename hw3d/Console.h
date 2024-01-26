#pragma once
#include <cstdio>
#include <memory>
#include <signal.h>
#include "AtumWindows.h"

#define NOMSGHANDLER

class Console {
public:
	class ConsoleClass {
	public:
		static const LPCWSTR GetName() noexcept;
		static HWND GetInstance() noexcept;
	private:
		ConsoleClass() noexcept;
		~ConsoleClass();
		ConsoleClass(const ConsoleClass&) = delete;
		ConsoleClass& operator=(const ConsoleClass&) = delete;
		static constexpr LPCWSTR conClassName = L"Atum.D3D.Console";
		static ConsoleClass conClass;
		HWND hWnd;
	};

	Console(const LPCWSTR name) noexcept;
	~Console() noexcept;
	Console(const Console&) = delete;
	Console& operator=(const Console&) = delete;
#ifndef NOMSGHANDLER
	void RunMessagePump();
#endif NOMSGHANDLER
private:
	FILE* pCin = nullptr;
	FILE* pCout = nullptr;
	FILE* pCerr = nullptr;
	static WNDPROC glpfnConsoleWindow;

	void CreateConsole();
	void InitializeConsole();
	void InitializeStreams();
	void InitializeHandlers();
	void ShowConsoleWindow();
	static BOOL CALLBACK CtrlHandler(DWORD fdwCtrlType) noexcept;
#ifndef NOMSGHANDLER
	static LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept;
	static void Sig_Handler(int n_signal) noexcept;
#endif NOMSGHANDLER
};
