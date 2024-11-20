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
#include "Surface.hpp"

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

Surface::Surface(const unsigned int width, const unsigned int height) noexcept
	:
	buffer_(std::make_unique<color[]>(static_cast<size_t>(width)* height)),
	width_(width),
	height_(height)
{}

Surface& Surface::operator=(Surface&& donor) noexcept
{
	width_ = donor.width_;
	height_ = donor.height_;
	buffer_ = std::move(donor.buffer_);
	donor.buffer_ = nullptr;
	return *this;
}

Surface::Surface(Surface&& source) noexcept
	:
	buffer_(std::move(source.buffer_)),
	width_(source.width_),
	height_(source.height_)
{}

Surface::~Surface()
= default;

void Surface::clear(const color& fillValue) const noexcept {
	for (unsigned int y = 0; y < height_; ++y) {
		for (unsigned int x = 0; x < width_; ++x) {
			buffer_[static_cast<size_t>(y) * width_ + x] = fillValue;
		}
	}
}

void Surface::putPixel(const unsigned int x, const unsigned int y, const color& c) noexcept(!IS_DEBUG)
{
	assert(x < width_);
	assert(y < height_);
	buffer_[static_cast<size_t>(y) * width_ + x] = c;
}

Surface::color Surface::getPixel(const unsigned int x, const unsigned int y) const noexcept(!IS_DEBUG)
{
	assert(x < width_);
	assert(y < height_);
	return buffer_[static_cast<size_t>(y) * width_ + x];
}

unsigned int Surface::getWidth() const noexcept
{
	return width_;
}

unsigned int Surface::getHeight() const noexcept
{
	return height_;
}

Surface::color* Surface::getBufferPtr() noexcept
{
	return buffer_.get();
}

const Surface::color* Surface::getBufferPtr() const noexcept
{
	return buffer_.get();
}

Surface Surface::fromFile(const std::wstring& name) {
	unsigned int width;
	unsigned int height;
	std::unique_ptr<color[]> buffer = nullptr;

	{
		const auto bitmap = std::make_unique<Gdiplus::Bitmap>(name.c_str());
		if (bitmap->GetLastStatus() != Gdiplus::Status::Ok) {
			const std::wstring exceptionMessage = L"Loading image [" + name + L"]: failed to load.";
			const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, exceptionMessage.data(), static_cast<int>(exceptionMessage.size()), nullptr, 0, nullptr, nullptr);
			std::string strToThrow(sizeNeeded, 0);
			WideCharToMultiByte(CP_UTF8, 0, exceptionMessage.data(), static_cast<int>(exceptionMessage.size()), strToThrow.data(), sizeNeeded, nullptr, nullptr);

			throw Exception(__LINE__, __FILE__, strToThrow);
		}

		height = bitmap->GetHeight();
		width = bitmap->GetWidth();
		buffer = std::make_unique<color[]>(static_cast<size_t>(width) * height);

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				Gdiplus::Color c;
				bitmap->GetPixel(static_cast<int>(x), static_cast<int>(y), &c);
				buffer[static_cast<size_t>(y) * width + x] = c.GetValue();
			}
		}
	}

	return { width, height, std::move(buffer) };
}

void Surface::save(const std::string& filename) const
{
	auto getEncoderClsid = [&filename](const WCHAR* format, CLSID* clsid) -> void
		{
			UINT num = 0;
			UINT size = 0;
			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size == 0)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}

			std::unique_ptr<Gdiplus::ImageCodecInfo[]> imageCodecInfo(new Gdiplus::ImageCodecInfo[size]);
			if (imageCodecInfo == nullptr)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
				throw Exception(__LINE__, __FILE__, ss.str());
			}

			GetImageEncoders(num, size, imageCodecInfo.get());
			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(imageCodecInfo[j].MimeType, format) == 0)
				{
					*clsid = imageCodecInfo[j].Clsid;
					return;
				}
			}
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
			throw Exception(__LINE__, __FILE__, ss.str());
		};

	CLSID bmpId;
	getEncoderClsid(L"image/bmp", &bmpId);

	wchar_t wideName[512];
	static_cast<void>(mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE));

	if (Gdiplus::Bitmap bitmap(static_cast<int>(width_), static_cast<int>(height_), static_cast<int>(width_) * sizeof(color), PixelFormat32bppARGB, reinterpret_cast<BYTE*>(buffer_.get()));
		bitmap.Save(wideName, &bmpId, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void Surface::copy(const Surface& src) noexcept(!IS_DEBUG) {
	assert(width_ == src.width_);
	assert(height_ == src.height_);
	for (unsigned int y = 0; y < height_; ++y) {
		for (unsigned int x = 0; x < width_; ++x) {
			buffer_[static_cast<size_t>(y) * width_ + x] = src.buffer_[static_cast<size_t>(y) * width_ + x];
		}
	}
}

Surface::Surface(const unsigned int width, const unsigned int height, std::unique_ptr<color[]> bufferParam) noexcept
	:
	buffer_(std::move(bufferParam)),
	width_(width),
	height_(height)
{}

// surface exception stuff
Surface::Exception::Exception(const int line, const char* file, std::string note) noexcept
	:
	AtumException(line, file),
	note_(std::move(note))
{}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << AtumException::what() << "/n"
		<< "[Note] " << getNote();
	whatBuffer_ = oss.str();
	return whatBuffer_.c_str();
}

const char* Surface::Exception::getType() const noexcept
{
	return "Atum Graphics Exception";
}

const std::string& Surface::Exception::getNote() const noexcept
{
	return note_;
}
