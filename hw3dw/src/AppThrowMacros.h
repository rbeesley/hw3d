#pragma once

// Error exception helper macro
#define APP_EXCEPT(message) app::app_exception(__LINE__, __FILE__, message)
