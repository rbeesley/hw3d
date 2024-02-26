#pragma once
#include "IMouse.h"
#include "Mouse.h"

class DeviceConfig
{
private:
	mouse mouse_ = {};
public:
	i_mouse& get_mouse();
};

