#pragma once
#include <queue>
#include <bitset>
#include <optional>

class keyboard
{
	friend class window;
public:
	class event
	{
	public:
		enum class state
		{
			press,
			release,
			invalid
		};
	private:
		state state_;
		unsigned char code_;
	public:
		event() = delete;
		event(const state state, const unsigned char code) noexcept
			:
			state_(state),
			code_(code)
		{}

		[[nodiscard]] bool is_press() const noexcept
		{
			return state_ == state::press;
		}

		[[nodiscard]] bool is_release() const noexcept
		{
			return state_ == state::release;
		}

		[[nodiscard]] bool is_valid() const noexcept
		{
			return state_ != state::invalid;
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
	void clear_key() noexcept;
	// char event stuff
	std::optional<char> read_char() noexcept;
	[[nodiscard]] bool is_char_empty() const noexcept;
	void clear_char() noexcept;
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
	std::bitset<number_of_keys> key_states_;
	std::queue<event> key_buffer_;
	std::queue<char> char_buffer_;
};
