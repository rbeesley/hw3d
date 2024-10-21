#pragma once

class i_graphics
{
public:
	i_graphics() = default;
	virtual ~i_graphics() = default;
	i_graphics(const i_graphics&) = delete;
	virtual i_graphics& operator=(const i_graphics&) = delete;
	i_graphics(const i_graphics&&) = delete;
	virtual i_graphics& operator=(const i_graphics&&) = delete;

public:
	virtual void end_frame() = 0;
	virtual void clear_buffer(float red, float green, float blue) const = 0;
	virtual void draw_test_triangle(float angle, float x, float y, float z) = 0;
};