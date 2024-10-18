#pragma once
// ReSharper disable CppUnusedIncludeDirective
#include <3rdParty/plog/Init.h>
#include <3rdParty/plog/Log.h>
#include <3rdParty/plog/Appenders/ColorConsoleAppender.h>
#include <3rdParty/plog/Appenders/DebugOutputAppender.h>
#include <3rdParty/plog/Appenders/DynamicAppender.h>
#include <3rdParty/plog/Formatters/FuncMessageFormatter.h>
#include <3rdParty/plog/Formatters/TxtFormatter.h>
// ReSharper restore CppUnusedIncludeDirective

class logging
{
public:
	class console {
	public:
		console() = delete;
	};

	class debug_output {
	public:
		debug_output() = delete;
	};

	enum : std::uint8_t {
		console = 1,
		debug_output
	};

	explicit logging(plog::Severity max_severity) noexcept;
	~logging() noexcept = default;
	logging(const logging&) = delete;
	logging& operator=(const logging&) = delete;
	logging(const logging&&) = delete;
	logging& operator=(const logging&&) = delete;
	static void init_console(plog::Severity max_severity);
	static void init_debug_output(plog::Severity max_severity);
private:
	plog::Severity max_severity_;
	static plog::DynamicAppender dynamic_appender_;
	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>& default_logger_;
};
