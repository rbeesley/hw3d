#pragma once
#include <optional>
#include <queue>
#include <utility>

#include "IMouse.h"

class mouse final : public i_mouse
{
public:
	mouse() = default;
	~mouse() override = default;
	mouse(const mouse&) = delete;
	mouse& operator=(const mouse&) = delete;
	mouse(const mouse&&) = delete;
	mouse& operator=(const mouse&&) = delete;

public:
	class event
	{
	public:
		enum class event_type : std::uint8_t
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
			enter,
			leave
		};
		using enum event_type;

	private:
		event_type event_type_;
		int x_;
		int y_;
		bool left_is_pressed_;
		bool right_is_pressed_;
		bool middle_is_pressed_;
		bool x1_is_pressed_;
		bool x2_is_pressed_;

	public:
		event() = delete;
		event(const event_type event_type, const mouse& parent) noexcept
			:
			event_type_(event_type),
			x_(parent.x_),
			y_(parent.y_),
			left_is_pressed_(parent.left_is_pressed_),
			right_is_pressed_(parent.right_is_pressed_),
			middle_is_pressed_(parent.middle_is_pressed_),
			x1_is_pressed_(parent.x1_is_pressed_),
			x2_is_pressed_(parent.x2_is_pressed_)
		{}

		[[nodiscard]] event_type get_type() const noexcept
		{
			return event_type_;
		}

		[[nodiscard]] std::pair<int, int> get_pos() const noexcept
		{
			return { x_, y_ };
		}

		[[nodiscard]] int get_pos_x() const noexcept
		{
			return x_;
		}

		[[nodiscard]] int get_pos_y() const noexcept
		{
			return y_;
		}

		[[nodiscard]] bool is_left_pressed() const noexcept
		{
			return left_is_pressed_;
		}

		[[nodiscard]] bool is_middle_pressed() const noexcept
		{
			return middle_is_pressed_;
		}

		[[nodiscard]] bool is_right_pressed() const noexcept
		{
			return right_is_pressed_;
		}

		[[nodiscard]] bool is_x1_pressed() const noexcept
		{
			return x1_is_pressed_;
		}

		[[nodiscard]] bool is_x2_pressed() const noexcept
		{
			return x2_is_pressed_;
		}
	};

private:
	void trim_buffer() noexcept;
	std::optional<event> read() noexcept;
	[[nodiscard]] bool is_empty() const noexcept;
	void clear() noexcept;

public:
	[[nodiscard]] std::pair<int, int> get_pos() const noexcept override;
	[[nodiscard]] position pos() const noexcept override;
	[[nodiscard]] int get_pos_x() const noexcept override;
	[[nodiscard]] int get_pos_y() const noexcept override;
	[[nodiscard]] bool is_in_window() const noexcept override;
	[[nodiscard]] bool is_left_pressed() const noexcept override;
	[[nodiscard]] bool is_right_pressed() const noexcept override;
	[[nodiscard]] bool is_middle_pressed() const noexcept override;
	[[nodiscard]] bool is_x1_pressed() const noexcept override;
	[[nodiscard]] bool is_x2_pressed() const noexcept override;
	void on_mouse_move(int x, int y) noexcept override;
	void on_mouse_leave() noexcept override;
	void on_mouse_enter(int x, int y) noexcept override;
	void on_left_pressed(int x, int y) noexcept override;
	void on_left_released(int x, int y) noexcept override;
	void on_right_pressed(int x, int y) noexcept override;
	void on_right_released(int x, int y) noexcept override;
	void on_middle_pressed(int x, int y) noexcept override;
	void on_middle_released(int x, int y) noexcept override;
	void on_x1_pressed(int x, int y) noexcept override;
	void on_x1_released(int x, int y) noexcept override;
	void on_x2_pressed(int x, int y) noexcept override;
	void on_x2_released(int x, int y) noexcept override;
	void on_wheel_up(int x, int y) noexcept override;
	void on_wheel_down(int x, int y) noexcept override;
	void on_wheel_right(int x, int y) noexcept override;
	void on_wheel_left(int x, int y) noexcept override;
	void on_v_wheel_delta(int x, int y, int delta) noexcept override;
	void on_h_wheel_delta(int x, int y, int delta) noexcept override;

private:
	static constexpr unsigned int buffer_size = 16u;
	int x_ = 0;
	int y_ = 0;
	bool in_window_ = false;
	bool left_is_pressed_ = false;
	bool right_is_pressed_ = false;
	bool middle_is_pressed_ = false;
	bool x1_is_pressed_ = false;
	bool x2_is_pressed_ = false;
	int v_wheel_delta_carry_ = 0;
	int h_wheel_delta_carry_ = 0;
	std::queue<event> event_buffer_;
};
