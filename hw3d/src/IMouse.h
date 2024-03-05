#pragma once
#include <memory>
#include <utility>

class i_mouse
{
public:
	i_mouse() = default;
	virtual ~i_mouse() = default;
	i_mouse(const i_mouse&) = delete;
	virtual i_mouse& operator=(const i_mouse&) = delete;
	i_mouse(const i_mouse&&) = delete;
	virtual i_mouse& operator=(const i_mouse&&) = delete;

public:
	[[nodiscard]] virtual std::pair<int, int> get_pos() const noexcept = 0;
	[[nodiscard]] virtual int get_pos_x() const noexcept = 0;
	[[nodiscard]] virtual int get_pos_y() const noexcept = 0;
	[[nodiscard]] virtual bool is_in_window() const noexcept = 0;
	[[nodiscard]] virtual bool is_left_pressed() const noexcept = 0;
	[[nodiscard]] virtual bool is_right_pressed() const noexcept = 0;
	[[nodiscard]] virtual bool is_middle_pressed() const noexcept = 0;
	[[nodiscard]] virtual bool is_x1_pressed() const noexcept = 0;
	[[nodiscard]] virtual bool is_x2_pressed() const noexcept = 0;
	virtual void on_mouse_move(int x, int y) noexcept = 0;
	virtual void on_mouse_leave() noexcept = 0;
	virtual void on_mouse_enter(int x, int y) noexcept = 0;
	virtual void on_left_pressed(int x, int y) noexcept = 0;
	virtual void on_left_released(int x, int y) noexcept = 0;
	virtual void on_right_pressed(int x, int y) noexcept = 0;
	virtual void on_right_released(int x, int y) noexcept = 0;
	virtual void on_middle_pressed(int x, int y) noexcept = 0;
	virtual void on_middle_released(int x, int y) noexcept = 0;
	virtual void on_x1_pressed(int x, int y) noexcept = 0;
	virtual void on_x1_released(int x, int y) noexcept = 0;
	virtual void on_x2_pressed(int x, int y) noexcept = 0;
	virtual void on_x2_released(int x, int y) noexcept = 0;
	virtual void on_wheel_up(int x, int y) noexcept = 0;
	virtual void on_wheel_down(int x, int y) noexcept = 0;
	virtual void on_wheel_right(int x, int y) noexcept = 0;
	virtual void on_wheel_left(int x, int y) noexcept = 0;
	virtual void on_v_wheel_delta(int x, int y, int delta) noexcept = 0;
	virtual void on_h_wheel_delta(int x, int y, int delta) noexcept = 0;
};
