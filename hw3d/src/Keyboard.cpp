#include "Keyboard.h"

#include <optional>

#include "DefinesConfig.h"

#if defined(LOG_KEYBOARD_MESSAGES) || defined(LOG_KEYBOARD_CHARS)  // defined in DefinesConfig.h
#include <format>
#include "Logging.h"
#include "VirtualKeyMap.h"

const static virtual_key_map virtual_key_map;
#endif

bool keyboard::is_key_pressed(const unsigned char keycode) const noexcept
{
	return key_states_[keycode];
}

std::optional<keyboard::event> keyboard::read_key() noexcept
{
	if (!key_buffer_.empty())
	{
		const event e = key_buffer_.front();
		key_buffer_.pop();
		return e;
	}
	return {};
}

bool keyboard::is_key_empty() const noexcept
{
	return key_buffer_.empty();
}

void keyboard::clear_key() noexcept
{
	key_buffer_ = std::queue<event>();
}

bool keyboard::is_char_empty() const noexcept
{
	return char_buffer_.empty();
}

void keyboard::clear_char() noexcept
{
	char_buffer_ = std::queue<char>();
}

void keyboard::clear() noexcept
{
	clear_key();
	clear_char();
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
	key_states_[key_code] = true;
	key_buffer_.emplace(event::state::press, key_code);
	trim_buffer(key_buffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in DefinesConfig.h
	PLOGV << "keydown: " << virtual_key_map(key_code);
#endif
}

void keyboard::on_key_released(unsigned char key_code) noexcept
{
	key_states_[key_code] = false;
	key_buffer_.emplace(event::state::release, key_code);
	trim_buffer(key_buffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in DefinesConfig.h
	PLOGV << "keyup: " << virtual_key_map(key_code);
#endif
}

void keyboard::on_char(unsigned char character) noexcept
{
	char_buffer_.emplace(character);
	trim_buffer(char_buffer_);
#ifdef LOG_KEYBOARD_CHARS // defined in DefinesConfig.h
	PLOGV << "char: " << character;
#endif
}

void keyboard::clear_state() noexcept
{
	key_states_.reset();
}

template <typename T>
void keyboard::trim_buffer(std::queue<T>& buffer) noexcept
{
	while(buffer.size() > buffer_size)
	{
		buffer.pop();
	}
}

std::optional<char> keyboard::read_char() noexcept
{
	if(!char_buffer_.empty())
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
