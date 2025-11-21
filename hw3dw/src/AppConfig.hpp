#pragma once
#define MAX_FPS 0 // Set to 0 to disable

#if defined(MAX_FPS) && (MAX_FPS > 0)
// Timer for 1 fps execution
#include <iostream>
#include <chrono>
#include <thread>
#endif
