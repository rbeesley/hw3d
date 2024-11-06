/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Surface.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#define SURFACE_
#include "AtumWindows.h"
#include "AtumException.h"
#include <string>
#include <assert.h>
#include <memory>

class surface
{
public:
	class color
	{
	public:
		unsigned int dword;
	public:
		constexpr color() noexcept : dword()
		{}
		constexpr color(const color& color) noexcept
			:
			dword(color.dword)
		{}
		constexpr color(unsigned int dw) noexcept
			:
			dword(dw)
		{}
		constexpr color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((x << 24u) | (r << 16u) | (g << 8u) | b)
		{}
		constexpr color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((r << 16u) | (g << 8u) | b)
		{}
		constexpr color(color color, unsigned char x) noexcept
			:
			color((x << 24u) | color.dword)
		{}
		color& operator =(color color) noexcept
		{
			dword = color.dword;
			return *this;
		}
		constexpr unsigned char get_x() const noexcept
		{
			return dword >> 24u;
		}
		constexpr unsigned char get_a() const noexcept
		{
			return get_x();
		}
		constexpr unsigned char get_r() const noexcept
		{
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char get_g() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char get_b() const noexcept
		{
			return dword & 0xFFu;
		}
		void set_x(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}
		void set_a(unsigned char a) noexcept
		{
			set_x(a);
		}
		void set_r(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		void set_g(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		void set_b(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};
public:
	class exception : public atum_exception
	{
	public:
		exception(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* get_type() const noexcept override;
		const std::string& get_note() const noexcept;
	private:
		std::string note_;
	};
public:
	surface(unsigned int width, unsigned int height) noexcept;
	surface(surface&& source) noexcept;
	surface(surface&) = delete;
	surface& operator=(surface&& donor) noexcept;
	surface& operator=(const surface&) = delete;
	~surface();
	void clear(const color& fill_value) const noexcept;
	void put_pixel(unsigned int x, unsigned int y, const color& c) noexcept(!IS_DEBUG);
	color get_pixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG);
	unsigned int get_width() const noexcept;
	unsigned int get_height() const noexcept;
	color* get_buffer_ptr() noexcept;
	const color* get_buffer_ptr() const noexcept;
	const color* get_buffer_ptr_const() const noexcept;
	static surface from_file(const std::wstring& name);
	void save(const std::string& filename) const;
	void copy(const surface& src) noexcept(!IS_DEBUG);
private:
	surface(unsigned int width, unsigned int height, std::unique_ptr<color[]> p_buffer_param) noexcept;
private:
	std::unique_ptr<color[]> p_buffer_;
	unsigned int width_;
	unsigned int height_;
};
