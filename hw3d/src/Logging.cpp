#include "AtumWindows.h"
#include "Logging.h"

#include <functional>

plog::DynamicAppender logging::dynamic_appender_;

logging::logging(const plog::Severity max_severity) noexcept
	: max_severity_(max_severity)
	, default_logger_(init(max_severity_, &dynamic_appender_))
{}

void logging::init_console(const plog::Severity max_severity) {
	PLOGI << "Initializing Console Logger";
	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> console_appender;
	plog::Logger<CONSOLE>& console_logger = plog::init<CONSOLE>(max_severity, &console_appender);
	dynamic_appender_.addAppender(&console_logger);
	PLOGI << "Console Logger Initialized";
}

void logging::init_debug_output(const plog::Severity max_severity) {
	PLOGI << "Initializing DebugOutput Logger";
	static plog::DebugOutputAppender<plog::TxtFormatter> debug_output_appender;
	plog::Logger<DEBUG_OUTPUT>& debug_output_logger = plog::init<DEBUG_OUTPUT>(max_severity, &debug_output_appender);
	dynamic_appender_.addAppender(&debug_output_logger);
	PLOGI << "DebugOutput Logger Initialized";
}
