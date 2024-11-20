#pragma once
#include "Box.hpp"
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
	App();
	~App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(const App&&) = delete;
	App& operator=(const App&&) = delete;

	[[nodiscard]] int initialize();
	int run();

private:
	void renderFrame(const ImVec4& clearColor);

private:
	std::unique_ptr<Window> window_;
#if (IS_DEBUG)
	std::unique_ptr<Console> console_;
	FpsMetric fps_{};
	CpuMetric cpu_{};
#endif
	std::shared_ptr<Graphics> graphics_;
	std::shared_ptr<Mouse> mouse_;
	std::shared_ptr<Keyboard> keyboard_;
	Timer timer_;
	std::unique_ptr<GdiPlusManager> gdiManager_;
	std::vector<std::unique_ptr<Drawable>> drawables_;
};
