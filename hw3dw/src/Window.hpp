#pragma once

#include <optional>
#include <string>
#include <memory>

#include "AtumException.hpp"
#include "AtumWindows.hpp"
#include "Graphics.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"

#include "backends/imgui_impl_win32.h"

// -----------------------------
// Window Class Declaration
// -----------------------------
class Window {
public:
    // Lifecycle
    explicit Window(unsigned int width, unsigned int height, LPCWSTR name);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // ImGui Platform Backend
    class ImGui {
    public:
        static void NewFrame() { ImGui_ImplWin32_NewFrame(); }
        static void Init(HWND hwnd) { ImGui_ImplWin32_Init(hwnd); }
        static void Shutdown() { ImGui_ImplWin32_Shutdown(); }
        static void UpdatePlatformWindows() { ::ImGui::UpdatePlatformWindows(); }
        static void RenderPlatformWindowsDefault() { ::ImGui::RenderPlatformWindowsDefault(); }
    };

    // Message Handling
    LRESULT forwardMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    // Accessors
    [[nodiscard]] HWND getHandle() const noexcept;
    Graphics& getGraphics() const noexcept;
    Mouse& getMouse() const noexcept;
    Keyboard& getKeyboard() const noexcept;

    // Window Management
    void setTitle(const std::wstring& title) const;
    HWND setActive(HWND window) const;
    void createWindow(LPCWSTR name);

    // Dimensions and Position
    struct WindowDimensions { LONG width; LONG height; };
    struct WindowPosition { int x; int y; };

    WindowDimensions getDimensions() const;
    WindowPosition getPosition() const;
    void setPosition(int x, int y);
    WindowDimensions getTargetDimensions();
    void setTargetDimensions(unsigned int width, unsigned int height);

    // -----------------------------
    // Exception Types
    // -----------------------------
    class Exception : public AtumException {
        using AtumException::AtumException;
    public:
        static std::string translateErrorCode(HRESULT result) noexcept;
    private:
        HRESULT result_;
    };

    class HResultException : public Exception {
    public:
        HResultException(int line, const char* file, HRESULT hresult) noexcept;
        const char* what() const noexcept override;
        const char* getType() const noexcept override;
        HRESULT getErrorCode() const noexcept;
        std::string getErrorDescription() const noexcept;
    private:
        HRESULT hresult_;
    };

    class NoGfxException : public Exception {
    public:
        using Exception::Exception;
        const char* getType() const noexcept override;
    };

private:
    // Internal Window Class
    class WindowClass {
    public:
        WindowClass();
        ~WindowClass();

        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        WindowClass(WindowClass&&) = delete;
        WindowClass& operator=(WindowClass&&) = delete;

        LPCWSTR getName() const noexcept;
        HINSTANCE getInstance() const noexcept;
    private:
        static constexpr LPCWSTR windowClassName = L"Atum.D3D";
        HINSTANCE instanceHandle_;
    };

    // Message Handlers
    static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT CALLBACK handleMsgImpl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    // Members
    HWND windowHandle_;
    LONG width_;
    LONG height_;
    std::unique_ptr<WindowClass> windowClass_;
    std::unique_ptr<Graphics> graphics_;
    std::unique_ptr<Mouse> mouse_;
    std::unique_ptr<Keyboard> keyboard_;

    int x_;
    int y_;
    bool inSizeMove_;
    bool minimized_;
    LONG targetWidth_ = 0;
    LONG targetHeight_ = 0;
};