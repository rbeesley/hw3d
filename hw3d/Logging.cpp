#include "AtumWindows.h"
#include "Console.h"
#include "Logging.h"

static plog::DynamicAppender dynamicAppender;
inline static Logging* logging;

plog::ColorConsoleAppender<plog::FuncMessageFormatter>& Logging::Console::Instance() {
	static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
	return consoleAppender;
}
Logging::Console::Console() { PLOGV << "Console Initialied"; };

plog::DebugOutputAppender<plog::TxtFormatter>& Logging::DebugOutput::Instance() {
	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	return debugOutputAppender;
}
Logging::DebugOutput::DebugOutput() { PLOGV << "DebugOutput Initialied"; };

Logging::Logging(plog::Severity maxSeverity) noexcept
	: maxSeverity(maxSeverity)
{
	PLOGV << "Logging::Logging(plog::Severity maxSeverity)";
	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get();
	if (logger == nullptr) {
		plog::init(this->maxSeverity, &dynamicAppender);
	}

	logging = this;
}

Logging::~Logging() noexcept {
	logging = nullptr;
}

Logging* Logging::get() noexcept {
	return logging;
}

void Logging::InitConsole(plog::Severity maxSeverity) {
	plog::init<Logging::kConsole>(maxSeverity, &Logging::Console::Instance());
	dynamicAppender.addAppender(&Logging::Console::Instance());
}

void Logging::InitDebugOutput(plog::Severity maxSeverity) {
	plog::init<Logging::kDebugOutput>(maxSeverity, &Logging::DebugOutput::Instance());
	dynamicAppender.addAppender(&Logging::DebugOutput::Instance());
}

void Logging::RemoveConsole() {
	PLOGV << "Removing Console Logger";
	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get();
	if (logger != nullptr) {
		dynamicAppender.removeAppender(plog::get<Logging::kConsole>());
	}
	else {
		PLOGW << "Can't remove Console Logger";
	}
}

void Logging::RemoveDebugOutput() {
	PLOGV << "Removing DebugOutput Logger";
	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get();
	if (logger != nullptr) {
		dynamicAppender.removeAppender(plog::get<Logging::kDebugOutput>());
	}
	else {
		PLOGW << "Can't remove DebugOutput Logger";
	}
}
