#pragma once
#include <3rdParty/plog/Log.h>
#include <3rdParty/plog/Appenders/DynamicAppender.h>
#include <3rdParty/plog/Formatters/FuncMessageFormatter.h>
#include <3rdParty/plog/Formatters/TxtFormatter.h>

// ReSharper disable once CppUnusedIncludeDirective
#include "LoggingConfig.h"

class logging {
public:
	enum : std::uint8_t { debug_output = 1, console };

	static void initialize(plog::Severity max_severity);
	static void initialize_console_logger(plog::Severity max_severity);
	static void initialize_debug_output_logger(plog::Severity max_severity);
	static void set_logger_severity(plog::Severity max_severity);
	static void set_console_logger_severity(plog::Severity max_severity);
	static void set_debug_output_logger_severity(plog::Severity max_severity);
	static void shutdown_console_logger();
	static void shutdown_debug_output_logger();
private:
	static plog::DynamicAppender dynamic_appender_;
	static inline plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* root_logger_;
	static inline plog::Logger<console>* console_logger_;
	static inline plog::Logger<debug_output>* debug_output_logger_;
};
