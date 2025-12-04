#pragma once

#include "Drawable.hpp"
#include "Window.hpp"
#include "Console.hpp"
#include "Timer.hpp"
#include "CpuMetric.hpp"
#include "FpsMetric.hpp"
#include "GDIPlusManager.hpp"

class App
{
public:
    // Exception types
    class Exception : public AtumException {
        using AtumException::AtumException;
    };

	class AppException : public Exception {
    public:
        AppException(int line, const char* file, const std::string& msg) noexcept;
        const char* getType() const noexcept override;
    };

    // Lifecycle
    explicit App(bool allowConsoleLogging);
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    // Main loop
    int run();

    // ImGui configuration
    void configureImGui() const;

    // Message handling
    LRESULT thunkEntry(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    static void setMainWindowHandle(HWND mainWindow);
    static HWND getMainWindowHandle();

private:
    // Helpers
    static std::optional<unsigned int> processMessages();
    void renderFrame(const ImVec4& clearColor);
    void populateDrawables();

    // Members
    static HWND s_mainWindow;
    std::unique_ptr<Window> window_;
    Graphics* graphics_;
    Mouse* mouse_;
    Keyboard* keyboard_;
#if (IS_DEBUG)
    std::unique_ptr<Console> console_;
    FpsMetric fps_{};
    CpuMetric cpu_{};
#endif
    Timer timer_;
    std::unique_ptr<GdiPlusManager> gdiManager_;
    std::vector<std::unique_ptr<Drawable>> drawables_;
    bool stop_;
};
