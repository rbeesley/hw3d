#pragma once

// Error exception helper macro
#define ATUM_WND_EXCEPT(h_result) Window::HresultException(__LINE__, __FILE__, h_result)
#define ATUM_WND_LAST_EXCEPT() Window::HresultException(__LINE__, __FILE__, GetLastError())
#define ATUM_WND_NO_GFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)
