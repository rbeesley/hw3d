#pragma once
#include <3rdParty/plog/Log.h>
#include <3rdParty/plog/Appenders/DynamicAppender.h>
#include <3rdParty/plog/Formatters/FuncMessageFormatter.h>
#include <3rdParty/plog/Formatters/TxtFormatter.h>

// ReSharper disable once CppUnusedIncludeDirective
#include "LoggingConfig.h"

class Logging {
public:
	enum : std::uint8_t { DEBUG_OUTPUT = 1, CONSOLE };

	static void initialize(plog::Severity maxSeverity);
	static void initializeConsoleLogger(plog::Severity maxSeverity);
	static void initializeDebugOutputLogger(plog::Severity maxSeverity);
	static void setLoggerSeverity(plog::Severity maxSeverity);
	static void setConsoleLoggerSeverity(plog::Severity maxSeverity);
	static void setDebugOutputLoggerSeverity(plog::Severity maxSeverity);
	static void shutdownConsoleLogger();
	static void shutdownDebugOutputLogger();
private:
	static plog::DynamicAppender dynamicAppender_;
	static inline plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* rootLogger_;
	static inline plog::Logger<CONSOLE>* consoleLogger_;
	static inline plog::Logger<DEBUG_OUTPUT>* debugOutputLogger_;
};
