#include "DeviceHAL.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

device_hal::device_hal()
	:
	p_mouse_(std::make_unique<mouse>()),
	p_keyboard_(std::make_unique<keyboard>())
{}

std::shared_ptr<mouse> device_hal::get_mouse()
{
	return std::shared_ptr{ std::move(p_mouse_) };
}

std::shared_ptr<keyboard> device_hal::get_keyboard()
{
	return std::shared_ptr{ std::move(p_keyboard_) };
}

std::shared_ptr<graphics> device_hal::get_graphics(const HWND parent, int width, int height)
{
	p_graphics_ = std::make_unique<graphics>(parent, width, height);
	return std::shared_ptr{ std::move(p_graphics_) };
}
