#include "App.h"
#include "Logging.h"

app::app()
	: window_(640, 360, TEXT("Atum D3D Window"))
#if defined(_DEBUG)
	, console_(TEXT("Debug Console"))
#endif
{
	PLOGI << "Initializing App";
}

int app::init() const
{
	if (!window_.get_handle())
	{
		PLOGF << "Failed to create Window";
		return -2;
	}

#ifdef _DEBUG
	if (!console_.get_window_handle())
	{
		PLOGE << "Failed to create Debug Console";
		return -3;
	}
#endif // _DEBUG

	return 0;
}

int app::run()
{
	while(true)
	{
		// Process pending messages without blocking
		if (const auto exit_code = window::process_messages())
		{
			// If the optional return has a value, it is the exit code
			return *exit_code;
		}
		process_frame();
	}
}

void app::process_frame()
{
}
