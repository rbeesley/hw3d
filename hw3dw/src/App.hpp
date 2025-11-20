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
	class Exception : public AtumException {
		using AtumException::AtumException;
	};
public:
	class AppException : public Exception
	{
	public:
		AppException(int line, const char* file, const std::string& msg) noexcept;
		const char* getType() const noexcept override;
	};

public:
	void configureImGui();
	App(bool allowConsoleLogging);
	~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(const App&&) = delete;
	App& operator=(const App&&) = delete;

	int run();
	static LRESULT handleMsg(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept;
	LRESULT CALLBACK handleMsgImpl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	static std::optional<unsigned int> processMessages();
	void renderFrame(const ImVec4& clearColor);
	void populateDrawables();

private:
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
