#pragma once

/* Quick override to turn on Verbose logging for all sinks - WinMain.cpp */
//#define LOG_LEVEL_FULL

#if !defined(LOG_LEVEL_FULL)
//plog::none	// Effectively turns off logging
//plog::fatal
//plog::error	// Default for production
//plog::warning
//plog::info	// Default for development
//plog::debug
//plog::verbose
#define LOG_LEVEL_DEFAULT plog::info
#define LOG_LEVEL_CONSOLE plog::info
#define LOG_LEVEL_DEBUG_OUTPUT plog::info
#else
#define LOG_LEVEL_DEFAULT plog::verbose
#define LOG_LEVEL_CONSOLE plog::verbose
#define LOG_LEVEL_DEBUG_OUTPUT plog::verbose
#endif

/* Log the Window Messages - Window.cpp */
//#define LOG_WINDOW_MESSAGES
//#define LOG_WINDOW_MOUSE_MESSAGES
//#define LOG_WINDOW_MOVEMENT_MESSAGES

/* Log the Keyboard Messages and/or Characters - Keyboard.cpp */
//#define LOG_KEYBOARD_MESSAGES
//#define LOG_KEYBOARD_CHARS

/* Log the Mouse Messages - Mouse.cpp */
//#define LOG_MOUSE_MESSAGES
