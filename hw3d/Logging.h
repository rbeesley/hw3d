#pragma once
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/DebugOutputAppender.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Formatters/TxtFormatter.h>

namespace Logging
{
	enum
	{
		kDebugOutput = 1,
		kConsole
	};
	void initLog();
}
