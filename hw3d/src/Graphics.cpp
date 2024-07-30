#include "Graphics.h"

#include "LoggingConfig.h"
#include "Logging.h"

#include "3rdParty/DxErr/DXErr.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")

// Graphics exception macros, some with DXGI info
#define GFX_EXCEPT_NOINFO(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
#define GFX_THROW_NOINFO(hresult_call) if(FAILED(hresult = (hresult_call))) throw graphics::hresult_exception(__LINE__, __FILE__, hresult)

#ifdef _DEBUG
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
#ifdef _DEBUG
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
		&swap_chain_,
		&device_,
		nullptr,
		&device_context_
	));

	// Get the address of the back buffer
	ID3D11Resource* back_buffer = nullptr;
	GFX_THROW_INFO(swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer)));

	// Use the back buffer address to create the render target
	GFX_THROW_INFO(device_->CreateRenderTargetView(back_buffer, nullptr, &target_view_));
	back_buffer->Release();

	// Set the render target as the back buffer
	device_context_->OMSetRenderTargets(1, &target_view_, nullptr);
}

graphics::~graphics()
{
	if (target_view_ != nullptr)
	{
		target_view_->Release();
	}
	if (swap_chain_ != nullptr)
	{
		swap_chain_->Release();
	}
	if (device_context_ != nullptr)
	{
		device_context_->Release();
	}
	if (device_ != nullptr)
	{
		device_->Release();
	}
}

void graphics::end_frame()
{
	HRESULT hresult;
#ifdef _DEBUG
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

void graphics::clear_buffer(const float red, const float green, const float blue) const noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	device_context_->ClearRenderTargetView(target_view_, color);
}

// Graphics Exception
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
	// Remove the final newline if it exists
	if(!info_message_.empty())
	{
		info_message_.pop_back();
	}
}

const char* graphics::hresult_exception::what() const noexcept
{
	std::ostringstream oss;
	oss << get_type() << '\n'
		<< "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
		<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << '\n'
		<< "[Error String] " << get_error_string() << '\n'
		<< "[Description] " << get_error_description() << '\n';
	if(!info_message_.empty())
	{
		oss << "\n[Error Info]\n" << get_error_info() << "\n\n";
	}
	oss << get_origin_string();
	what_buffer_ = oss.str();
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
	const WCHAR* buffer = DXGetErrorString(hresult_);
	// Convert WCHAR* to std::wstring
	std::wstring ws(buffer);
	// Set the exception message
	std::string utf8_message(ws.begin(), ws.end());
	return utf8_message;
#else
	return DXGetErrorString(hresult_);
#endif
}

std::string graphics::hresult_exception::get_error_description() const noexcept
{
#ifdef UNICODE
	WCHAR buffer[2048];
	DXGetErrorDescription(hresult_, buffer, sizeof(buffer));
	// Convert WCHAR* to std::wstring
	std::wstring ws(buffer);
	// Set the exception message
	std::string utf8_message(ws.begin(), ws.end());
	return utf8_message;
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
