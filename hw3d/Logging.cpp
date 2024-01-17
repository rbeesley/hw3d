#include "AtumWindows.h"
#include "Logging.h"

namespace Logging
{
	void initLog() {
		// Create a console window
		AllocConsole();
		FILE* pCout;
		freopen_s(&pCout, "CONOUT$", "w", stdout);

		// Use the ShowWindow() function to show the console window
		ShowWindow(GetConsoleWindow(), SW_SHOW);

		// Configure logging
		static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
		plog::init<kDebugOutput>(plog::info, &debugOutputAppender);

		static plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
		plog::init<kConsole>(plog::verbose, &consoleAppender);

		plog::init(plog::debug, plog::get<kDebugOutput>()).addAppender(plog::get<kConsole>());
	}
}