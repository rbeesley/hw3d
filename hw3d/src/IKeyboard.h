#pragma once

class i_keyboard
{
public:
	i_keyboard() = default;
	virtual ~i_keyboard() = default;
	i_keyboard(const i_keyboard&) = delete;
	virtual i_keyboard& operator=(const i_keyboard&) = delete;
	i_keyboard(const i_keyboard&&) = delete;
	virtual i_keyboard& operator=(const i_keyboard&&) = delete;

public:
	[[nodiscard]] virtual bool is_key_pressed(const unsigned char keycode) const noexcept = 0;
	virtual void on_key_pressed(unsigned char key_code) noexcept = 0;
	virtual void on_key_released(unsigned char key_code) noexcept = 0;
	virtual void clear_state() noexcept = 0;
	virtual void on_char(unsigned char character) noexcept = 0;
	virtual void enable_autorepeat() noexcept = 0;
	virtual void disable_autorepeat() noexcept = 0;
	[[nodiscard]] virtual bool is_autorepeat_enabled() const noexcept = 0;
};
