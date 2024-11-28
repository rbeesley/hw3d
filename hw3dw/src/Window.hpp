#pragma once
#include <optional>
#include <sstream>
#include <memory>

#include "AtumException.hpp"
#include "AtumWindows.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Graphics.hpp"

#define IMGUI_DOCKING

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
	class HResultException : public Exception
	{
	public:
		HResultException(int line, const char* file, HRESULT hresult) noexcept;
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
		SHORT width;
		SHORT height;
	};
	struct WindowPosition
	{
		int x;
		int y;
	};

	explicit Window(int width, int height, LPCWSTR name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(const Window&&) = delete;
	Window& operator=(const Window&&) = delete;

	void createWindow(LPCWSTR name);
	void configureImGui();
	static void initializeStaticMembers();
	static void shutdownStaticMembers();

	[[nodiscard]] static HWND getHandle();
	static HWND setActive(HWND window);
	WindowDimensions getDimensions() const;
	static WindowPosition getPosition();
	static void setPosition(int x, int y);
	static WindowDimensions getTargetDimensions();
	static void setTargetDimensions(unsigned int width, unsigned int height);
	static void setTitle(const std::wstring& title);
	static std::weak_ptr<Mouse> getMouseWeakPtr() noexcept;
	static std::weak_ptr<Keyboard> getKeyboardWeakPtr() noexcept;
	static std::weak_ptr<Graphics> getGraphicsWeakPtr() noexcept;
	static LRESULT CALLBACK handleMsg(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	static LRESULT CALLBACK handleMsgSetup(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK handleMsgThunk(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
private:
	static std::shared_ptr<Mouse> mouse_;
	static std::shared_ptr<Keyboard> keyboard_;
	static std::shared_ptr<Graphics> graphics_;
	inline static int x_{};
	inline static int y_{};
	SHORT width_{};
	SHORT height_{};
	static bool inSizeMove_;
	static bool minimized_;
	static SHORT targetWidth_;
	static SHORT targetHheight_;
	std::unique_ptr<WindowClass> windowClass_;
	static inline HWND windowHandle_;
};
