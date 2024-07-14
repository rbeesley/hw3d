#pragma once
#include <bitset>
#include <memory>
#include <optional>
#include <queue>

#include "IKeyboard.h"

class keyboard final : public i_keyboard
{
public:
	keyboard() = default;
	~keyboard() override = default;
	keyboard(const keyboard&) = delete;
	keyboard& operator=(const keyboard&) = delete;
	keyboard(const keyboard&&) = delete;
	keyboard& operator=(const keyboard&&) = delete;

public:
	class event
	{
	public:
		enum class event_type
		{
			press,
			release,
			invalid
		};
		using enum event_type;

	private:
		event_type event_type_;
		unsigned char code_;

	public:
		event() = delete;
		event(const event_type event_type, const unsigned char code) noexcept
			:
			event_type_(event_type),
			code_(code)
		{}

		[[nodiscard]] bool is_press() const noexcept
		{
			return event_type_ == press;
		}

		[[nodiscard]] bool is_release() const noexcept
		{
			return event_type_ == release;
		}

		[[nodiscard]] unsigned char get_code() const noexcept
		{
			return code_;
		}
	};

	// key event and char event management
private:
	template<typename T>
	static void trim_buffer(std::queue<T>& buffer) noexcept;

	// key event management
private:
	std::optional<event> read_key() noexcept;
	[[nodiscard]] bool is_key_empty() const noexcept;
	void clear_event_buffer() noexcept;
public:
	[[nodiscard]] bool is_key_pressed(unsigned char keycode) const noexcept override;
	void on_key_pressed(unsigned char key_code) noexcept override;
	void on_key_released(unsigned char key_code) noexcept override;
	void clear_state() noexcept override;

	// char event management
private:
	std::optional<char> read_char() noexcept;
	[[nodiscard]] bool is_char_empty() const noexcept;
	void clear_char_buffer() noexcept;
	void clear() noexcept;
public:
	void on_char(unsigned char character) noexcept override;

	// auto repeat control
public:
	void enable_autorepeat() noexcept override;
	void disable_autorepeat() noexcept override;
	[[nodiscard]] bool is_autorepeat_enabled() const noexcept override;

private:
	static constexpr unsigned int number_of_keys = 256u;
	static constexpr unsigned int buffer_size = 16u;
	bool autorepeat_enabled_ = false;
	std::bitset<number_of_keys> key_state_;
	std::queue<event> event_buffer_;
	std::queue<char> char_buffer_;
};
