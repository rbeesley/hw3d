#include "Mouse.h"

#include <optional>

#include "DefinesConfig.h"

#if defined(LOG_MOUSE_MESSAGES) // defined in DefinesConfig.h
#include <format>
#include "Logging.h"
#include "VirtualKeyMap.h"

const static virtual_key_map virtual_key_map;
#endif

std::pair<int, int> mouse::get_pos() const noexcept
{
	return { x_, y_ };
}

int mouse::get_pos_x() const noexcept
{
	return x_;
}

int mouse::get_pos_y() const noexcept
{
	return y_;
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

void mouse::clear() noexcept
{
	event_buffer_ = std::queue<event>();
}

void mouse::on_mouse_move(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::move, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse move: x:" << x << " y:" << y;
#endif
}

void mouse::on_left_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::l_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse left pressed";
#endif
}

void mouse::on_left_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::l_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse left released";
#endif
}

void mouse::on_left_double(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::l_double, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse left double click";
#endif
}

void mouse::on_right_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::r_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse right pressed";
#endif
}

void mouse::on_right_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::r_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse right released";
#endif
}

void mouse::on_right_double(int x, int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::m_double, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse right double click";
#endif
}

void mouse::on_middle_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::m_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse middle pressed";
#endif
}

void mouse::on_middle_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::m_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse middle released";
#endif
}

void mouse::on_middle_double(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::m_double, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse middle double click";
#endif
}

void mouse::on_x1_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::x1_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse button 4 pressed";
#endif
}

void mouse::on_x1_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::x1_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse button 4 released";
#endif
}

void mouse::on_x2_pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::x2_press, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse button 5 pressed";
#endif
}

void mouse::on_x2_released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::x2_release, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse button 5 released";
#endif
}

void mouse::on_wheel_up(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::wheel_up, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse wheel up";
#endif
}

void mouse::on_wheel_down(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::wheel_down, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse wheel down";
#endif
}

void mouse::on_wheel_right(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::wheel_right, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse wheel right";
#endif
}

void mouse::on_wheel_left(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	event_buffer_.emplace(event::event_type::wheel_left, *this);
	trim_buffer();
#ifdef LOG_MOUSE_MESSAGES // defined in DefinesConfig.h
	PLOGV << "mouse wheel left";
#endif
}

void mouse::trim_buffer() noexcept
{
	while (event_buffer_.size() > buffer_size)
	{
		event_buffer_.pop();
	}
}
