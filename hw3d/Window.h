#pragma once
#include "AtumWindows.h"
#include "AtumException.h"
#include <sstream>

class Window
{
private:
	// singleton
	class WindowClass {
	public:
		static const LPCWSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr LPCWSTR wndClassName = L"Atum.D3D";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	class Exception : public AtumException {
	public:
		Exception(int line, const char* file, HRESULT hresult) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hresult) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hresult;
	};
public:
	Window(int width, int height, const LPCWSTR name);
	~Window();
	HWND GetHandle() const;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width{};
	int height{};
	HWND hWnd;
};

// Error exception helper macro
#define ATUM_WND_EXCEPT(hresult) Window::Exception(__LINE__, __FILE__, hresult)
#define ATUM_WND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())
