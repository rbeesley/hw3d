#pragma once

// Error exception helper macro
#define APP_EXCEPT(message) App::AppException(__LINE__, __FILE__, message)
