#pragma once

#include "AtumWindows.h"

#include "IGraphics.h"
#include "IMouse.h"
#include "IKeyboard.h"

class device_config
{
public:
	device_config();
private:
	std::unique_ptr<i_mouse> p_mouse_;
	std::unique_ptr<i_keyboard> p_keyboard_;
	std::unique_ptr<i_graphics> p_graphics_;
public:
	std::shared_ptr<i_mouse> get_mouse();
	std::shared_ptr<i_keyboard> get_keyboard();
	std::shared_ptr<i_graphics> get_graphics(const HWND parent, int width, int height);
};
