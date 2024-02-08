#pragma once
// ReSharper disable CppUnusedIncludeDirective
#include <3rdParty/plog/Log.h>
#include <3rdParty/plog/Init.h>
#include <3rdParty/plog/Formatters/FuncMessageFormatter.h>
#include <3rdParty/plog/Formatters/TxtFormatter.h>
#include <3rdParty/plog/Appenders/ColorConsoleAppender.h>
#include <3rdParty/plog/Appenders/DebugOutputAppender.h>
#include <3rdParty/plog/Appenders/DynamicAppender.h>
// ReSharper restore CppUnusedIncludeDirective

class logging
{
public:
	class console {
	public:
		static plog::ColorConsoleAppender<plog::FuncMessageFormatter>& instance();
	private:
		console();
	};

	class debug_output {
	public:
		static plog::DebugOutputAppender<plog::TxtFormatter>& instance();
	private:
		debug_output();
	};

	enum {
		CONSOLE = 1,
		DEBUG_OUTPUT
	};

	explicit logging(plog::Severity max_severity) noexcept;
	logging(const logging&) = delete;
	logging& operator=(const logging&) = delete;
	logging(const logging&&) = delete;
	logging& operator=(const logging&&) = delete;
	static void init_console(plog::Severity max_severity);
	static void remove_console();
	static void init_debug_output(plog::Severity max_severity);
	static void remove_debug_output();
private:
	plog::Severity max_severity_;
	static plog::DynamicAppender dynamic_appender_;
};

