#pragma once
#include <optional>
#include <queue>
#include <utility>

class mouse
{
	friend class window;
public:
	class event
	{
	public:
		enum class event_type
		{
			l_press,
			l_release,
			l_double,
			r_press,
			r_release,
			r_double,
			m_press,
			m_release,
			m_double,
			x1_press,
			x1_release,
			x2_press,
			x2_release,
			wheel_up,
			wheel_down,
			wheel_right,
			wheel_left,
			move,
			invalid
		};
	private:
		event_type event_type_;
		bool left_is_pressed_;
		bool right_is_pressed_;
		bool middle_is_pressed_;
		int x_;
		int y_;
	public:
		event() = delete;
		event(const event_type event_type, const mouse& parent) noexcept
			:
			event_type_(event_type),
			left_is_pressed_(parent.left_is_pressed_),
			right_is_pressed_(parent.right_is_pressed_),
			middle_is_pressed_(parent.middle_is_pressed_),
			x_(parent.x_),
			y_(parent.y_)
		{}
		bool is_valid() const noexcept
		{
			return event_type_ != event_type::invalid;
		}
		event_type get_type() const noexcept
		{
			return event_type_;
		}
		std::pair<int, int> get_pos() const noexcept
		{
			return { x_, y_ };
		}
		int get_pos_x() const noexcept
		{
			return x_;
		}
		int get_pos_y() const noexcept
		{
			return y_;
		}
		bool is_left_pressed() const noexcept
		{
			return left_is_pressed_;
		}
		bool is_middle_pressed() const noexcept
		{
			return middle_is_pressed_;
		}
		bool is_right_pressed() const noexcept
		{
			return right_is_pressed_;
		}
	};
public:
	mouse() = default;
	mouse(const mouse&) = delete;
	mouse& operator=(const mouse&) = delete;
	std::pair<int, int> get_pos() const noexcept;
	int get_pos_x() const noexcept;
	int get_pos_y() const noexcept;
	bool is_left_pressed() const noexcept;
	bool is_right_pressed() const noexcept;
	bool is_middle_pressed() const noexcept;
	std::optional<event> read() noexcept;
	bool is_empty() const noexcept
	{
		return event_buffer_.empty();
	}
	void clear() noexcept;
private:
	void on_mouse_move(int x, int y) noexcept;
	void on_left_pressed(int x, int y) noexcept;
	void on_left_released(int x, int y) noexcept;
	void on_left_double(int x, int y) noexcept;
	void on_right_pressed(int x, int y) noexcept;
	void on_right_released(int x, int y) noexcept;
	void on_right_double(int x, int y) noexcept;
	void on_middle_pressed(int x, int y) noexcept;
	void on_middle_released(int x, int y) noexcept;
	void on_middle_double(int x, int y) noexcept;
	void on_x1_pressed(int x, int y) noexcept;
	void on_x1_released(int x, int y) noexcept;
	void on_x2_pressed(int x, int y) noexcept;
	void on_x2_released(int x, int y) noexcept;
	void on_wheel_up(int x, int y) noexcept;
	void on_wheel_down(int x, int y) noexcept;
	void on_wheel_right(int x, int y) noexcept;
	void on_wheel_left(int x, int y) noexcept;
	void trim_buffer() noexcept;
private:
	static constexpr unsigned int buffer_size = 16u;
	int x_;
	int y_;
	bool left_is_pressed_;
	bool right_is_pressed_;
	bool middle_is_pressed_;
	std::queue<event> event_buffer_;
};
