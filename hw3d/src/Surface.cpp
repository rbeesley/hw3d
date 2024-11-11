/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Surface.cpp																			  *
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
#include "surface.h"

#include <algorithm>
#include <codecvt>
#include <gdiplus.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

namespace Gdiplus
{
	using std::min;
	using std::max;
}

surface::surface(const unsigned int width, const unsigned int height) noexcept
	:
	p_buffer_(std::make_unique<color[]>(static_cast<size_t>(width)* height)),
	width_(width),
	height_(height)
{}

surface& surface::operator=(surface&& donor) noexcept
{
	width_ = donor.width_;
	height_ = donor.height_;
	p_buffer_ = std::move(donor.p_buffer_);
	donor.p_buffer_ = nullptr;
	return *this;
}

surface::surface(surface&& source) noexcept
	:
	p_buffer_(std::move(source.p_buffer_)),
	width_(source.width_),
	height_(source.height_)
{}

surface::~surface()
= default;

void surface::clear(const color& fill_value) const noexcept {
	for (unsigned int y = 0; y < height_; ++y) {
		for (unsigned int x = 0; x < width_; ++x) {
			p_buffer_[static_cast<size_t>(y) * width_ + x] = fill_value;
		}
	}
}

void surface::put_pixel(const unsigned int x, const unsigned int y, const color& c) noexcept(!IS_DEBUG)
{
	assert(x < width_);
	assert(y < height_);
	p_buffer_[static_cast<size_t>(y) * width_ + x] = c;
}

surface::color surface::get_pixel(const unsigned int x, const unsigned int y) const noexcept(!IS_DEBUG)
{
	assert(x < width_);
	assert(y < height_);
	return p_buffer_[static_cast<size_t>(y) * width_ + x];
}

unsigned int surface::get_width() const noexcept
{
	return width_;
}

unsigned int surface::get_height() const noexcept
{
	return height_;
}

surface::color* surface::get_buffer_ptr() noexcept
{
	return p_buffer_.get();
}

const surface::color* surface::get_buffer_ptr() const noexcept
{
	return p_buffer_.get();
}

const surface::color* surface::get_buffer_ptr_const() const noexcept
{
	return p_buffer_.get();
}

surface surface::from_file(const std::wstring& name) {
	unsigned int width;
	unsigned int height;
	std::unique_ptr<color[]> p_buffer = nullptr;

	{
		const auto bitmap = std::make_unique<Gdiplus::Bitmap>(name.c_str());
		if (bitmap->GetLastStatus() != Gdiplus::Status::Ok) {
			const std::wstring exception_message = L"Loading image [" + name + L"]: failed to load.";
			const int size_needed = WideCharToMultiByte(CP_UTF8, 0, exception_message.data(), static_cast<int>(exception_message.size()), nullptr, 0, nullptr, nullptr);
			std::string str_to_throw(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, exception_message.data(), static_cast<int>(exception_message.size()), str_to_throw.data(), size_needed, nullptr, nullptr);

			throw exception(__LINE__, __FILE__, str_to_throw);
		}

		height = bitmap->GetHeight();
		width = bitmap->GetWidth();
		p_buffer = std::make_unique<color[]>(static_cast<size_t>(width) * height);

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				Gdiplus::Color c;
				bitmap->GetPixel(static_cast<int>(x), static_cast<int>(y), &c);
				p_buffer[static_cast<size_t>(y) * width + x] = c.GetValue();
			}
		}
	}

	return { width, height, std::move(p_buffer) };
}

void surface::save(const std::string& filename) const
{
	auto get_encoder_clsid = [&filename](const WCHAR* format, CLSID* p_clsid) -> void
		{
			UINT num = 0;
			UINT size = 0;
			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
				throw exception(__LINE__, __FILE__, ss.str());
			}

			std::unique_ptr<Gdiplus::ImageCodecInfo[]> p_image_codec_info(new Gdiplus::ImageCodecInfo[size]);
			if (p_image_codec_info == nullptr)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
				throw exception(__LINE__, __FILE__, ss.str());
			}

			GetImageEncoders(num, size, p_image_codec_info.get());
			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(p_image_codec_info[j].MimeType, format) == 0)
				{
					*p_clsid = p_image_codec_info[j].Clsid;
					return;
				}
			}
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
			throw exception(__LINE__, __FILE__, ss.str());
		};

	CLSID bmp_id;
	get_encoder_clsid(L"image/bmp", &bmp_id);

	wchar_t wide_name[512];
	static_cast<void>(mbstowcs_s(nullptr, wide_name, filename.c_str(), _TRUNCATE));

	if (Gdiplus::Bitmap bitmap(static_cast<int>(width_), static_cast<int>(height_), static_cast<int>(width_) * sizeof(color), PixelFormat32bppARGB, reinterpret_cast<BYTE*>(p_buffer_.get()));
		bitmap.Save(wide_name, &bmp_id, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw exception(__LINE__, __FILE__, ss.str());
	}
}

void surface::copy(const surface& src) noexcept(!IS_DEBUG) {
	assert(width_ == src.width_);
	assert(height_ == src.height_);
	for (unsigned int y = 0; y < height_; ++y) {
		for (unsigned int x = 0; x < width_; ++x) {
			p_buffer_[static_cast<size_t>(y) * width_ + x] = src.p_buffer_[static_cast<size_t>(y) * width_ + x];
		}
	}
}

surface::surface(const unsigned int width, const unsigned int height, std::unique_ptr<color[]> p_buffer_param) noexcept
	:
	p_buffer_(std::move(p_buffer_param)),
	width_(width),
	height_(height)
{}

// surface exception stuff
surface::exception::exception(const int line, const char* file, std::string note) noexcept
	:
	atum_exception(line, file),
	note_(std::move(note))
{}

const char* surface::exception::what() const noexcept
{
	std::ostringstream oss;
	oss << atum_exception::what() << "/n"
		<< "[Note] " << get_note();
	what_buffer_ = oss.str();
	return what_buffer_.c_str();
}

const char* surface::exception::get_type() const noexcept
{
	return "Atum Graphics Exception";
}

const std::string& surface::exception::get_note() const noexcept
{
	return note_;
}
