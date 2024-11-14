#include "Keyboard.h"

#include <optional>

#include "Logging.h"

#if defined(LOG_KEYBOARD_MESSAGES) || defined(LOG_KEYBOARD_CHARS)  // defined in LoggingConfig.h
#include <format>
#include "VirtualKeyMap.h"

const static class virtual_key_map virtual_key_map;
#endif

bool keyboard::is_key_pressed(const unsigned char keycode) const noexcept
{
	return key_state_[keycode];
}

std::optional<keyboard::event> keyboard::read_key() noexcept
{
	if (!event_buffer_.empty())
	{
		const event e = event_buffer_.front();
		event_buffer_.pop();
		return e;
	}
	return {};
}

bool keyboard::is_key_empty() const noexcept
{
	return event_buffer_.empty();
}

void keyboard::clear_event_buffer() noexcept
{
	event_buffer_ = std::queue<event>();
}

bool keyboard::is_char_empty() const noexcept
{
	return char_buffer_.empty();
}

void keyboard::clear_char_buffer() noexcept
{
	char_buffer_ = std::queue<char>();
}

void keyboard::clear() noexcept
{
	clear_event_buffer();
	clear_char_buffer();
}

void keyboard::enable_autorepeat() noexcept
{
	autorepeat_enabled_ = true;
}

void keyboard::disable_autorepeat() noexcept
{
	autorepeat_enabled_ = false;
}

bool keyboard::is_autorepeat_enabled() const noexcept
{
	return autorepeat_enabled_;
}

void keyboard::on_key_pressed(unsigned char key_code) noexcept
{
	key_state_[key_code] = true;
	event_buffer_.emplace(event::event_type::press, key_code);
	trim_buffer(event_buffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in LoggingConfig.h
	PLOGV << "keydown: " << virtual_key_map(key_code);
#endif
}

void keyboard::on_key_released(unsigned char key_code) noexcept
{
	key_state_[key_code] = false;
	event_buffer_.emplace(event::event_type::release, key_code);
	trim_buffer(event_buffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in LoggingConfig.h
	PLOGV << "keyup: " << virtual_key_map(key_code);
#endif
}

void keyboard::on_char(unsigned char character) noexcept
{
	char_buffer_.emplace(character);
	trim_buffer(char_buffer_);
#ifdef LOG_KEYBOARD_CHARS // defined in LoggingConfig.h
	PLOGV << "char: " << character;
#endif
}

void keyboard::clear_state() noexcept
{
	key_state_.reset();
}

template <typename T>
void keyboard::trim_buffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > buffer_size)
	{
		buffer.pop();
	}
}

std::optional<char> keyboard::read_char() noexcept
{
	if (!char_buffer_.empty())
	{
		const char char_code = char_buffer_.front();
		char_buffer_.pop();
		return char_code;
	}
	else
	{
		return {};
	}
}
