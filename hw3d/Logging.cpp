#include "AtumWindows.h"
#include "Console.h"
#include "Logging.h"

static plog::DynamicAppender dynamicAppender;
//static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
//static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
//static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
static Logging* Logger;

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

//Logging::Logging() noexcept
//	: maxSeverity(plog::none)
//{}

Logging::Logging(plog::Severity maxSeverity) noexcept
	: maxSeverity(maxSeverity)
{
	PLOGV << "Logging::Logging(plog::Severity maxSeverity)";
	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get();
	if (logger == nullptr) {
		plog::init(this->maxSeverity, &dynamicAppender);
	}

	Logger = this;
}

Logging::~Logging() noexcept {}

//std::ostream& operator<< (std::ostream& out, Logging::InstanceId instanceId)
//{
//	switch (instanceId)
//	{
//	case Logging::InstanceId::Console: return out << "Console";
//	case Logging::InstanceId::DebugOutput: return out << "DebugOutput";
//	};
//	return out << static_cast<int>(instanceId);
//}

Logging* Logging::GetLogger() noexcept {
	return Logger;
}

// Private
//template<Logging::InstanceId instanceId>
//void Logging::InitDebugOutput(plog::Severity maxSeverity) noexcept {
//	PLOGV << "Initializing DebugOutput Logger";
//
//	plog::init<instanceId>(maxSeverity, &debugOutputAppender);
//
//
//	PLOGV << "Logging Initialized with instanceId: " << instanceId;
//}

//template<Logging::InstanceId instanceId>
//template<int instanceId>
//inline void Logging::InitLogger(plog::Severity maxSeverity) {
//	PLOGV << "Initializing Logger with instanceId: " << instanceId;
//	switch (instanceId) {
//	case Logging::kConsole:
//		plog::init<Logging::kConsole>(maxSeverity, &Logging::consoleAppender);
//		dynamicAppender.addAppender(&consoleAppender);
//		break;
//	case Logging::kDebugOutput:
//		plog::init<Logging::kDebugOutput>(maxSeverity, &Logging::debugOutputAppender);
//		dynamicAppender.addAppender(&debugOutputAppender);
//		break;
//	}
//	PLOGV << "Initialized " << instanceId << " Logger";
//}

//template<Logging::InstanceId instanceId>
//template<int instanceId>
//inline void Logging::RemoveLogger() {
//	PLOGV << "Removing Logger with instanceId: " << instanceId;
//	plog::Logger<PLOG_DEFAULT_INSTANCE_ID>* logger = plog::get();
//	if (logger != nullptr) {
//		dynamicAppender.removeAppender(plog::get<instanceId>());
//	}
//	else {
//		PLOGW << "Can't remove logger with instanceId: " << instanceId;
//	}
//}

void Logging::InitConsole(plog::Severity maxSeverity) {
	//plog::init<Logging::kConsole>(maxSeverity, &consoleAppender);
	//dynamicAppender.addAppender(&consoleAppender);
	plog::init<Logging::kConsole>(maxSeverity, &Logging::Console::Instance());
	dynamicAppender.addAppender(&Logging::Console::Instance());
}

void Logging::InitDebugOutput(plog::Severity maxSeverity) {
	//plog::init<Logging::kDebugOutput>(maxSeverity, &debugOutputAppender);
	//dynamicAppender.addAppender(&debugOutputAppender);
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
