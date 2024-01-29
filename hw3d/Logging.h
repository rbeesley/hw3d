#pragma once
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include <plog/Appenders/DynamicAppender.h>

class Logging
{
public:
	class Console {
	public:
		static plog::ColorConsoleAppender<plog::FuncMessageFormatter>& Instance();
	private:
		Console();
	};

	class DebugOutput {
	public:
		static plog::DebugOutputAppender<plog::TxtFormatter>& Instance();
	private:
		DebugOutput();
	};

	enum {
		kConsole = 1,
		kDebugOutput
	};

	Logging(plog::Severity maxSeverity) noexcept;
	~Logging() noexcept;
	Logging(const Logging&) = delete;
	Logging& operator=(const Logging&) = delete;
	static Logging* GetLogger() noexcept;
	//template<int instanceId>
	//inline void InitLogger(plog::Severity maxSeverity);
	//template<int instanceId>
	//inline void RemoveLogger();

	void InitConsole(plog::Severity maxSeverity);
	void RemoveConsole();
	void InitDebugOutput(plog::Severity maxSeverity);
	void RemoveDebugOutput();
private:
	plog::Severity maxSeverity;
};

