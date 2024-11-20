#include "Logging.hpp"

#include <3rdParty/plog/Init.h>
#include <3rdParty/plog/Appenders/ColorConsoleAppender.h>
#include <3rdParty/plog/Appenders/DebugOutputAppender.h>

plog::DynamicAppender Logging::dynamicAppender_;

void Logging::initialize(const plog::Severity maxSeverity) {
	rootLogger_ = &plog::init<PLOG_DEFAULT_INSTANCE_ID>(maxSeverity, &dynamicAppender_);
}

void Logging::initializeConsoleLogger(const plog::Severity maxSeverity) {
	PLOGI << "Initializing Console Logger";
	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> console_appender;
	plog::Logger<CONSOLE>& console_logger = plog::init<CONSOLE>(maxSeverity, &console_appender);
	consoleLogger_ = &console_logger;
	dynamicAppender_.addAppender(&console_logger);
	PLOGV << "Console Logger Initialized";
}

void Logging::initializeDebugOutputLogger(const plog::Severity maxSeverity) {
	PLOGI << "Initializing DebugOutput Logger";
	static plog::DebugOutputAppender<plog::TxtFormatter> debug_output_appender;
	plog::Logger<DEBUG_OUTPUT>& debug_output_logger = plog::init<DEBUG_OUTPUT>(maxSeverity, &debug_output_appender);
	debugOutputLogger_ = &debug_output_logger;
	dynamicAppender_.addAppender(&debug_output_logger);
	PLOGV << "DebugOutput Logger Initialized";
}

void Logging::setLoggerSeverity(const plog::Severity maxSeverity)
{
	rootLogger_->setMaxSeverity(maxSeverity);
}

void Logging::setConsoleLoggerSeverity(const plog::Severity maxSeverity)
{
	consoleLogger_->setMaxSeverity(maxSeverity);
}

void Logging::setDebugOutputLoggerSeverity(const plog::Severity maxSeverity)
{
	debugOutputLogger_->setMaxSeverity(maxSeverity);
}

void Logging::shutdownConsoleLogger() {
	if (consoleLogger_) {
		dynamicAppender_.removeAppender(consoleLogger_);
		consoleLogger_ = nullptr;
	}
	PLOGI << "Shutdown Console Logger";
}

void Logging::shutdownDebugOutputLogger() {
	if (debugOutputLogger_) {
		dynamicAppender_.removeAppender(debugOutputLogger_);
		debugOutputLogger_ = nullptr;
	}
	PLOGI << "Shutdown DebugOutput Logger";
}
