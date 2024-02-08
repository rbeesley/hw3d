#include "AtumWindows.h"
#include "Logging.h"

#include <functional>

plog::DynamicAppender logging::dynamic_appender_;

logging::logging(const plog::Severity max_severity) noexcept
	: max_severity_(max_severity)
{
	if (const plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get(); logger == nullptr) {
		init(max_severity_, &dynamic_appender_);
	}
}

void logging::init_console(const plog::Severity max_severity) {
	PLOGI << "Initializing Console Logger";
	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> console_appender;
	plog::IAppender& console_logger = plog::init<CONSOLE>(max_severity, &console_appender);
	dynamic_appender_.addAppender(&console_logger);
	PLOGI << "Console Logger Initialized";
}

void logging::init_debug_output(const plog::Severity max_severity) {
	PLOGI << "Initializing DebugOutput Logger";
	static plog::DebugOutputAppender<plog::TxtFormatter> debug_output_appender;
	plog::IAppender& debug_output_logger = plog::init<DEBUG_OUTPUT>(max_severity, &debug_output_appender);
	dynamic_appender_.addAppender(&debug_output_logger);
	PLOGI << "DebugOutput Logger Initialized";
}

void logging::remove_console() {
	PLOGI << "Removing Console Logger";
	dynamic_appender_.removeAppender(plog::get<CONSOLE>());
	PLOGI << "Console Logger Removed";
}

void logging::remove_debug_output() {
	PLOGI << "Removing DebugOutput Logger";
	dynamic_appender_.removeAppender(plog::get<DEBUG_OUTPUT>());
	PLOGI << "DebugOutput Logger Removed";
}
