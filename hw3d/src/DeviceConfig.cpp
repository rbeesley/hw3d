#include "DeviceConfig.h"

i_mouse& device_config::get_mouse()
{
	return mouse_;
}

i_keyboard& device_config::get_keyboard()
{
	return keyboard_;
}
