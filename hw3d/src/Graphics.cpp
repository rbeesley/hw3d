// ReSharper disable CppClangTidyClangDiagnosticExtraSemiStmt
#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "DXErr.h"

#include "Logging.h"
#include "LoggingConfig.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <sstream>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#ifndef __cplusplus
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
#define __uuidof(iid) IID_##iid
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

graphics::graphics(HWND parent, int width, int height) :
	parent_(parent),
	width_(static_cast<float>(width)),
	height_(static_cast<float>(height)),
	projection_()
{
	PLOGI << "Initializing Graphics";

	PLOGI << "Initialize Swap Chain";
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
		.BufferDesc = {
			.Width = static_cast<UINT>(width),
			.Height = static_cast<UINT>(height),
			.RefreshRate = {
				.Numerator = 0u,
				.Denominator = 0u,
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM, // RGBA
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		},
		.SampleDesc = {
			.Count = 1u,
			.Quality = 0u,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1u,
		.OutputWindow = parent_,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = 0u,
	};

	UINT swap_create_flags = 0u;
#if defined(DEBUG) || defined(_DEBUG)
	PLOGI << "Enable D3D11 Device Debugging";
	swap_create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// For checking results of D3D functions
	HRESULT hresult;

	PLOGI << "Create device, front/back buffers, swap chain, and rendering context";
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swap_create_flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		p_swap_chain_.GetAddressOf(),
		p_device_.GetAddressOf(),
		nullptr,
		p_context_.GetAddressOf()
	));

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif

	PLOGI << "Get the address of the back buffer";
	wrl::ComPtr<ID3D11Resource> p_back_buffer;
	GFX_THROW_INFO(p_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &p_back_buffer));

	PLOGI << "Use the back buffer address to create the render target";
	GFX_THROW_INFO(p_device_->CreateRenderTargetView(p_back_buffer.Get(), nullptr, p_target_view_.GetAddressOf()));

#ifdef __clang__
#pragma clang diagnostic pop
#endif

	PLOGI << "Create and set the depth stencil state";
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {
		.DepthEnable = TRUE,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS,
		.StencilEnable = FALSE,
		.StencilReadMask = 0,
		.StencilWriteMask = 0,
		.FrontFace = {},
		.BackFace = {}
	};

	wrl::ComPtr<ID3D11DepthStencilState> p_depth_stencil_state;
	GFX_THROW_INFO(p_device_->CreateDepthStencilState(&depth_stencil_desc, &p_depth_stencil_state));

	PLOGI << "Bind depth state to the pipeline";
	p_context_->OMSetDepthStencilState(p_depth_stencil_state.Get(), 0u);

	PLOGI << "Create depth stencil texture";
	D3D11_TEXTURE2D_DESC depth_desc = {
		.Width = static_cast<UINT>(width),
		.Height = static_cast<UINT>(height),
		.MipLevels = 1u,
		.ArraySize = 1u,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc = {
			.Count = 1u,
			.Quality = 0u
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u
	};

	wrl::ComPtr<ID3D11Texture2D> p_depth_stencil;
	GFX_THROW_INFO(p_device_->CreateTexture2D(&depth_desc, nullptr, &p_depth_stencil));

	PLOGI << "Create view of the depth stencil texture";
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {
		.Format = DXGI_FORMAT_D32_FLOAT,
		// May also be DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, and it will use the same as the depth_desc
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Flags = 0u,
		.Texture2D = {
			.MipSlice = 0u,
		}
	};

	GFX_THROW_INFO(
		p_device_->CreateDepthStencilView(p_depth_stencil.Get(), &depth_stencil_view_desc, &p_depth_stencil_view_));

	PLOGI << "Bind the render target and stencil views";
	//  - Output Merger
	p_context_->OMSetRenderTargets(1u, p_target_view_.GetAddressOf(), p_depth_stencil_view_.Get());

	PLOGI << "Configure and set the viewport";
	//  - Rasterizer
	const D3D11_VIEWPORT viewport = {
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = width_,
		.Height = height_,
		.MinDepth = 0,
		.MaxDepth = 1
	};
	p_context_->RSSetViewports(1u, &viewport);
}

void graphics::end_frame()
{
	HRESULT hresult;
#if defined(DEBUG) || defined(_DEBUG)
	info_manager_.set();
#endif
	if (FAILED(hresult = p_swap_chain_->Present(1u, 0u)))
	{
		if (hresult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(p_device_->GetDeviceRemovedReason());
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
	p_context_->ClearRenderTargetView(p_target_view_.Get(), color);
	p_context_->ClearDepthStencilView(p_depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void graphics::draw_indexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(p_context_->DrawIndexed(count, 0u, 0u));
}

void graphics::set_projection(DirectX::FXMMATRIX& projection) noexcept
{
	projection_ = projection;
}

DirectX::XMMATRIX graphics::get_projection() const noexcept
{
	return projection_;
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
