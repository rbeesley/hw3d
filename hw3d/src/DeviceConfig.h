#pragma once
#include "IMouse.h"
#include "Mouse.h"
#include "IKeyboard.h"
#include "Keyboard.h"

class device_config
{
public:
	device_config() = default;
private:
	mouse mouse_ = {};
	keyboard keyboard_ = {};
public:
	i_mouse& get_mouse();
	i_keyboard& get_keyboard();
};
