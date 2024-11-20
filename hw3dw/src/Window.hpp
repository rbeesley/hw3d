#pragma once
#include <optional>
#include <sstream>
#include <memory>

#include "AtumException.hpp"

#define WINDOW_
#include "AtumWindows.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Graphics.hpp"

class Window
{
private:
	// singleton
	class WindowClass {
	public:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		WindowClass(const WindowClass&&) = delete;
		WindowClass& operator=(const WindowClass&&) = delete;
		void shutdown() const noexcept;
		static LPCWSTR getName() noexcept;
		HINSTANCE getInstance() const noexcept;
	private:
		static constexpr LPCWSTR windowClassName = L"Atum.D3D";
		HINSTANCE instanceHandle_;
	};
public:
	class Exception : public AtumException {
		using AtumException::AtumException;
	public:
		static std::string translateErrorCode(HRESULT result) noexcept;
	private:
		HRESULT result_;
	};
public:
	class HresultException : public Exception
	{
	public:
		HresultException(int line, const char* file, HRESULT hresult) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		HRESULT getErrorCode() const noexcept;
		std::string getErrorDescription() const noexcept;
	private:
		HRESULT hresult_;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* getType() const noexcept override;

	};
public:
	struct WindowDimensions
	{
		unsigned int width;
		unsigned int height;
	};

	void createWindow(LPCWSTR name);
	void configureImGui();
	static void initializeStaticMembers();
	static void shutdownStaticMembers();
	explicit Window(int width, int height, LPCWSTR name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(const Window&&) = delete;
	Window& operator=(const Window&&) = delete;

	[[nodiscard]] static HWND getHandle();
	static std::optional<unsigned int> processMessages();
	std::shared_ptr<Mouse> get_mouse();
	std::shared_ptr<Keyboard> get_keyboard();
	std::shared_ptr<Graphics> get_graphics();
	static void setTitle(const std::wstring& title);
	static void setTargetDimensions(unsigned int width, unsigned int height);
	static WindowDimensions getTargetDimensions();
private:
	static LRESULT CALLBACK handleMsgSetup(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK handleMsgThunk(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static HWND setActive(HWND window);
	static LRESULT CALLBACK handleMsg(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
private:
	static std::shared_ptr<Mouse> mouse_;
	static std::shared_ptr<Keyboard> keyboard_;
	std::shared_ptr<Graphics> graphics_;
	inline static int x_{};
	inline static int y_{};
	int width_{};
	int height_{};
	static bool inSizeMove_;
	static bool minimized_;
	static unsigned int targetWidth_;
	static unsigned int targetHheight_;
	std::unique_ptr<WindowClass> windowClass_;
	static inline HWND windowHandle_;
};
