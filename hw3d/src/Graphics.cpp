#include "Graphics.h"

#include "Logging.h"
#include "LoggingConfig.h"

#include "3rdParty/DxErr/DXErr.h"
#include <sstream>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")

// Graphics exception macros, some with DXGI info
#define GFX_EXCEPT_NOINFO(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
#define GFX_THROW_NOINFO(hresult_call) if(FAILED(hresult = (hresult_call))) throw graphics::hresult_exception(__LINE__, __FILE__, hresult)

#if defined(DEBUG) || defined(_DEBUG)
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
#define GFX_THROW_INFO(hresult_call) info_manager_.set(); if(FAILED(hresult = (hresult_call))) throw GFX_EXCEPT(hresult)
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
#else
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
#define GFX_THROW_INFO(hresult_call) if(FAILED(hresult = (hresult_call))) throw graphics::hresult_exception(__LINE__, __FILE__, (hresult))
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult))
#endif

graphics::graphics(const HWND window_handle)
{
	PLOGI << "Initializing Graphics";

	DXGI_SWAP_CHAIN_DESC scd = {
		.BufferDesc = {
			.Width = 0,
			.Height = 0,
			.RefreshRate = {
				.Numerator = 0,
				.Denominator = 0,
			},
			//.Format = DXGI_FORMAT_B8G8R8A8_UNORM, // BGRA
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // RGBA
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		},
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,
		.OutputWindow = window_handle,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = 0,
	};

	UINT swap_create_flags = 0u;
#if defined(DEBUG) || defined(_DEBUG)
	swap_create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// For checking results of D3D functions
	HRESULT hresult;

	// Create device and front/back buffers, swap chain, and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swap_create_flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&scd,
		swap_chain_.GetAddressOf(),
		device_.GetAddressOf(),
		nullptr,
		device_context_.GetAddressOf()
	));

	// Get the address of the back buffer
	wrl::ComPtr<ID3D11Resource> back_buffer;
	GFX_THROW_INFO(swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer));

	// Use the back buffer address to create the render target
	GFX_THROW_INFO(device_->CreateRenderTargetView(back_buffer.Get(), nullptr, target_view_.GetAddressOf()));
}

void graphics::end_frame()
{
	HRESULT hresult;
#if defined(DEBUG) || defined(_DEBUG)
	info_manager_.set();
#endif
	if (FAILED(hresult = swap_chain_->Present(1u, 0u)))
	{
		if (hresult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(device_->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hresult);
		}
	}
	
}

void graphics::clear_buffer(const float red, const float green, const float blue) const
{
	const float color[] = { red, green, blue, 1.0f };
	device_context_->ClearRenderTargetView(target_view_.Get(), color);
}

// Graphics Exception
std::string graphics::graphics_exception::to_narrow(const wchar_t* s, const char fallback, const std::locale& loc) noexcept
{
	std::ostringstream out;
	while (*s != L'\0') {
		out << std::use_facet<std::ctype<wchar_t>>(loc).narrow(*s++, fallback);
	}
	return out.str();
}

graphics::hresult_exception::hresult_exception(const int line, const char* file, const HRESULT hresult, const std::vector<std::string>& info_messages) noexcept
	:
	graphics_exception(line, file),
	hresult_(hresult)
{
	// Join all the info messages into a single string
	for (const auto& message : info_messages)
	{
		info_message_ += message;
		info_message_.push_back('\n');
	}
}

const char* graphics::hresult_exception::what() const noexcept
{
	std::ostringstream out;
	out	<< "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
		<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << '\n'
		<< "[Error String] " << get_error_string() << '\n'
		<< "[Description] " << get_error_description() << '\n';
	if(!info_message_.empty())
	{
		out << "[Error Info]\n" << get_error_info() << '\n';
	}
	out << get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* graphics::hresult_exception::get_type() const noexcept
{
	return "Atum Graphics Exception";
}

HRESULT graphics::hresult_exception::get_error_code() const noexcept
{
	return hresult_;
}

std::string graphics::hresult_exception::get_error_string() const noexcept
{
#ifdef UNICODE
	return to_narrow(DXGetErrorString(hresult_));
#else
	return DXGetErrorString(hresult_);
#endif
}

std::string graphics::hresult_exception::get_error_description() const noexcept
{
#ifdef UNICODE
	WCHAR buffer[512];
	DXGetErrorDescription(hresult_, buffer, sizeof(buffer));
	return to_narrow(buffer);
#else
	char buffer[512];
	DXGetErrorDescription(hresult_, buffer, sizeof(buffer));
	return buffer;
#endif
}

std::string graphics::hresult_exception::get_error_info() const noexcept
{
	return info_message_;
}

const char* graphics::device_removed_exception::get_type() const noexcept
{
	return "Atum Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
