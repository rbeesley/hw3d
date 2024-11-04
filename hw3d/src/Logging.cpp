#include "Logging.h"

#include <3rdParty/plog/Init.h>
#include <3rdParty/plog/Appenders/ColorConsoleAppender.h>
#include <3rdParty/plog/Appenders/DebugOutputAppender.h>

plog::DynamicAppender logging::dynamic_appender_;

void logging::initialize(const plog::Severity max_severity) {
    root_logger_ = &plog::init<PLOG_DEFAULT_INSTANCE_ID>(max_severity, &dynamic_appender_);
}

void logging::initialize_console_logger(const plog::Severity max_severity) {
    PLOGI << "Initializing Console Logger";
    static plog::ColorConsoleAppender<plog::FuncMessageFormatter> console_appender;
    plog::Logger<console>& console_logger = plog::init<console>(max_severity, &console_appender);
    console_logger_ = &console_logger;
    dynamic_appender_.addAppender(&console_logger);
    PLOGV << "Console Logger Initialized";
}

void logging::initialize_debug_output_logger(const plog::Severity max_severity) {
    PLOGI << "Initializing DebugOutput Logger";
    static plog::DebugOutputAppender<plog::TxtFormatter> debug_output_appender;
    plog::Logger<debug_output>& debug_output_logger = plog::init<debug_output>(max_severity, &debug_output_appender);
    debug_output_logger_ = &debug_output_logger;
    dynamic_appender_.addAppender(&debug_output_logger);
    PLOGV << "DebugOutput Logger Initialized";
}

void logging::shutdown_console_logger() {
    if (console_logger_) {
        dynamic_appender_.removeAppender(console_logger_);
        console_logger_ = nullptr;
    }
    PLOGI << "Shutdown Console Logger";
}

void logging::shutdown_debug_output_logger() {
    if (debug_output_logger_) {
        dynamic_appender_.removeAppender(debug_output_logger_);
        debug_output_logger_ = nullptr;
    }
    PLOGI << "Shutdown DebugOutput Logger";
}
