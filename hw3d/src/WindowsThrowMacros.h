#pragma once

// Error exception helper macro
#define ATUM_WND_EXCEPT(h_result) window::hresult_exception(__LINE__, __FILE__, h_result)
#define ATUM_WND_LAST_EXCEPT() window::hresult_exception(__LINE__, __FILE__, GetLastError())
#define ATUM_WND_NO_GFX_EXCEPT() window::no_gfx_exception(__LINE__, __FILE__)
