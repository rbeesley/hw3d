#include "DeviceConfig.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

device_config::device_config()
	:
	p_mouse_(std::make_unique<mouse>()),
	p_keyboard_(std::make_unique<keyboard>())
{}

std::shared_ptr<i_mouse> device_config::get_mouse()
{
	return std::shared_ptr{ std::move(p_mouse_) };
}

std::shared_ptr<i_keyboard> device_config::get_keyboard()
{
	return std::shared_ptr{ std::move(p_keyboard_) };
}

std::shared_ptr<i_graphics> device_config::get_graphics(const HWND parent, int width, int height)
{
	p_graphics_ = std::make_unique<graphics>(parent, width, height);
	return std::shared_ptr{ std::move(p_graphics_) };
}
