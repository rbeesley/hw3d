#include "Graphics.h"

#include "Logging.h"
#include "LoggingConfig.h"

#include <d3dcompiler.h>
#include <sstream>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// Add the DXErr library
#include "DXErr.h"

// Graphics exception macros, some with DXGI info

// Get the resulting error message and log the exception, no info manager
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT_NOINFO(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check, no info manager
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_NOINFO(hresult_call) if(FAILED(hresult = (hresult_call))) throw graphics::hresult_exception(__LINE__, __FILE__, hresult)

#if defined(DEBUG) || defined(_DEBUG)
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) info_manager_.set(); if(FAILED(hresult = (hresult_call))) throw GFX_EXCEPT(hresult)
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) info_manager_.set(); (call); { auto v = info_manager_.get_messages(); if( !v.empty() ) { throw graphics::info_exception(__LINE__, __FILE__, v); }}
#else
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) if(FAILED(hresult = (hresult_call))) throw graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) (call)
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

// Experimental drawing code
void graphics::draw_test_triangle()
{
	namespace wrl = Microsoft::WRL;

	struct vertex
	{
		float x;
		float y;
	};

	// Create a vertex buffer (1 2D triangle at the center of the screen)
	constexpr vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f},
	};

	wrl::ComPtr<ID3D11Buffer> vertex_buffer;
	constexpr D3D11_BUFFER_DESC buffer_desc = {
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = sizeof(vertex),
	};

	const D3D11_SUBRESOURCE_DATA subresource_data = {
		.pSysMem = vertices,
	};

	HRESULT hresult;
	GFX_THROW_INFO(device_->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer));

	// Bind the vertex buffer to the pipeline
	constexpr UINT stride = sizeof(vertex);
	constexpr UINT offset = 0u;
	device_context_->IASetVertexBuffers(0u, 1u, vertex_buffer.GetAddressOf(), &stride, &offset);

	// Create vertex shader
	wrl::ComPtr<ID3D11VertexShader> vertex_shader;
	wrl::ComPtr<ID3DBlob> blob;

	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &blob));
	GFX_THROW_INFO(device_->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertex_shader));

	device_context_->VSSetShader(vertex_shader.Get(), nullptr, 0u);

	GFX_THROW_INFO_ONLY(device_context_->Draw(std::size(vertices), 0u));
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
#ifdef _UNICODE
	return to_narrow(DXGetErrorString(hresult_));
#else
	return DXGetErrorString(hresult_);
#endif
}

std::string graphics::hresult_exception::get_error_description() const noexcept
{
#ifdef _UNICODE
	WCHAR buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return to_narrow(buffer);
#else
	char buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return buffer;
#endif
}

std::string graphics::hresult_exception::get_error_info() const noexcept
{
	return info_message_;
}

graphics::info_exception::info_exception(int line, const char* file, const std::vector<std::string>& info_messages) noexcept
	:
	graphics_exception(line, file)
{
	// Join all the info messages into a single string
	for (const auto& message : info_messages)
	{
		info_message_ += message;
		info_message_.push_back('\n');
	}
}

const char* graphics::info_exception::what() const noexcept
{
	std::ostringstream out;
	out << get_type() << '\n';
	if (!info_message_.empty())
	{
		out << "[Error Info]\n" << get_error_info() << '\n';
	}
	out << get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* graphics::info_exception::get_type() const noexcept
{
	return "Atum Graphics Info Exception";
}

std::string graphics::info_exception::get_error_info() const noexcept
{
	return info_message_;
}

const char* graphics::device_removed_exception::get_type() const noexcept
{
	return "Atum Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
