#pragma once
#include <bitset>
#include <optional>
#include <queue>

class keyboard
{
	friend class window;
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
			return event_type_ == event_type::press;
		}

		[[nodiscard]] bool is_release() const noexcept
		{
			return event_type_ == event_type::release;
		}

		[[nodiscard]] bool is_valid() const noexcept
		{
			return event_type_ != event_type::invalid;
		}

		[[nodiscard]] unsigned char get_code() const noexcept
		{
			return code_;
		}
	};
public:
	keyboard() = default;
	~keyboard() noexcept = default;
	keyboard(const keyboard&) = delete;
	keyboard& operator=(const keyboard&) = delete;
	keyboard(const keyboard&&) = delete;
	keyboard& operator=(const keyboard&&) = delete;
	// key event stuff
	[[nodiscard]] bool is_key_pressed(unsigned char keycode) const noexcept;
	std::optional<event> read_key() noexcept;
	[[nodiscard]] bool is_key_empty() const noexcept;
	void clear_event_buffer() noexcept;
	// char event stuff
	std::optional<char> read_char() noexcept;
	[[nodiscard]] bool is_char_empty() const noexcept;
	void clear_char_buffer() noexcept;
	void clear() noexcept;
	// auto repeat control
	void enable_autorepeat() noexcept;
	void disable_autorepeat() noexcept;
	[[nodiscard]] bool is_autorepeat_enabled() const noexcept;
private:
	void on_key_pressed(unsigned char key_code) noexcept;
	void on_key_released(unsigned char key_code) noexcept;
	void on_char(unsigned char character) noexcept;
	void clear_state() noexcept;
	template<typename T>
	static void trim_buffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int number_of_keys = 256u;
	static constexpr unsigned int buffer_size = 16u;
	bool autorepeat_enabled_ = false;
	std::bitset<number_of_keys> key_state_;
	std::queue<event> event_buffer_;
	std::queue<char> char_buffer_;
};
