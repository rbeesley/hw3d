#pragma once

#include "AtumWindows.h"

#include "Graphics.h"
#include "Mouse.h"
#include "Keyboard.h"

class device_hal
{
public:
	device_hal();
private:
	std::unique_ptr<mouse> p_mouse_;
	std::unique_ptr<keyboard> p_keyboard_;
	std::unique_ptr<graphics> p_graphics_;
public:
	std::shared_ptr<mouse> get_mouse();
	std::shared_ptr<keyboard> get_keyboard();
	std::shared_ptr<graphics> get_graphics(const HWND parent, int width, int height);
};
