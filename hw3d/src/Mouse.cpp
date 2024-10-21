#include "Mouse.h"

#include <optional>

#include "AtumWindows.h"
#include "LoggingConfig.h"

#if defined(LOG_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include <format>
#include "Logging.h"
#include "VirtualKeyMap.h"

const static virtual_key_map virtual_key_map;
#endif

using enum mouse::event::event_type;

void mouse::trim_buffer() noexcept
{
	while (event_buffer_.size() > buffer_size)
	{
		event_buffer_.pop();
	}
}

std::optional<mouse::event> mouse::read() noexcept
{
	if (!event_buffer_.empty())
	{
		const event e = event_buffer_.front();
		event_buffer_.pop();
		return e;
	}
	return {};
}

bool mouse::is_empty() const noexcept
{
	return event_buffer_.empty();
}

void mouse::clear() noexcept
{
	event_buffer_ = std::queue<event>();
}

std::pair<int, int> mouse::get_pos() const noexcept
{
	return { x_, y_ };
}

mouse::position mouse::pos() const noexcept
{
	return position{ x_, y_ };
}

int mouse::get_pos_x() const noexcept
{
	return x_;
}

int mouse::get_pos_y() const noexcept
{
	return y_;
}

bool mouse::is_in_window() const noexcept
{
	return in_window_;
}

bool mouse::is_left_pressed() const noexcept
{
	return left_is_pressed_;
}

bool mouse::is_right_pressed() const noexcept
{
	return right_is_pressed_;
}

bool mouse::is_middle_pressed() const noexcept
{
	return middle_is_pressed_;
}

bool mouse::is_x1_pressed() const noexcept
{
	return x1_is_pressed_;
}

bool mouse::is_x2_pressed() const noexcept
{
	return x2_is_pressed_;
}

void mouse::on_mouse_move(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(move, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse move: x:" << x << " y:" << y;
#endif
}

void mouse::on_mouse_leave() noexcept
{
	in_window_ = false;

	event_buffer_.emplace(leave, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse leave window";
#endif
}

void mouse::on_mouse_enter(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	in_window_ = true;

	event_buffer_.emplace(enter, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse enter window: x:" << x << " y:" << y;
#endif
}

void mouse::on_left_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	left_is_pressed_ = true;
	event_buffer_.emplace(l_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse left pressed";
#endif
}

void mouse::on_left_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	left_is_pressed_ = false;
	event_buffer_.emplace(l_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse left released";
#endif
}

void mouse::on_right_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	right_is_pressed_ = true;
	event_buffer_.emplace(r_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse right pressed";
#endif
}

void mouse::on_right_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	right_is_pressed_ = false;
	event_buffer_.emplace(r_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse right released";
#endif
}

void mouse::on_middle_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	middle_is_pressed_ = true;
	event_buffer_.emplace(m_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse middle pressed";
#endif
}

void mouse::on_middle_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	middle_is_pressed_ = false;
	event_buffer_.emplace(m_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse middle released";
#endif
}

void mouse::on_x1_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	x1_is_pressed_ = true;
	event_buffer_.emplace(x1_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 4 pressed";
#endif
}

void mouse::on_x1_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	x1_is_pressed_ = false;
	event_buffer_.emplace(x1_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 4 released";
#endif
}

void mouse::on_x2_pressed(const int x, const int y) noexcept
{
	x2_is_pressed_ = true;
	x_ = x;
	y_ = y;

	event_buffer_.emplace(x2_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 5 pressed";
#endif
}

void mouse::on_x2_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	x2_is_pressed_ = false;
	event_buffer_.emplace(x2_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 5 released";
#endif
}

void mouse::on_wheel_up(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(wheel_up, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel up";
#endif
}

void mouse::on_wheel_down(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(wheel_down, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel down";
#endif
}

void mouse::on_wheel_right(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(wheel_right, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel right";
#endif
}

void mouse::on_wheel_left(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(wheel_left, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel left";
#endif
}

void mouse::on_v_wheel_delta(const int x, const int y, const int delta) noexcept
{
	v_wheel_delta_carry_ += delta;
	while(v_wheel_delta_carry_ >= WHEEL_DELTA)
	{
		v_wheel_delta_carry_ -= WHEEL_DELTA;
		on_wheel_up(x, y);
	}
	while(v_wheel_delta_carry_ <= -WHEEL_DELTA)
	{
		v_wheel_delta_carry_ += WHEEL_DELTA;
		on_wheel_down(x, y);
	}
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse v wheel delta: " << v_wheel_delta_carry_;
#endif
}

void mouse::on_h_wheel_delta(const int x, const int y, const int delta) noexcept
{
	h_wheel_delta_carry_ += delta;
	while (h_wheel_delta_carry_ >= WHEEL_DELTA)
	{
		h_wheel_delta_carry_ -= WHEEL_DELTA;
		on_wheel_right(x, y);
	}
	while (h_wheel_delta_carry_ <= -WHEEL_DELTA)
	{
		h_wheel_delta_carry_ += WHEEL_DELTA;
		on_wheel_left(x, y);
	}
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse h wheel delta: " << h_wheel_delta_carry_;
#endif
}
