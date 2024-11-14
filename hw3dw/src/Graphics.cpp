// ReSharper disable CppClangTidyClangDiagnosticExtraSemiStmt
#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "DXErr.h"
#include "backends/imgui_impl_dx11.h"

#include "Logging.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <functional>
#include <sstream>

#define UNCAPPED_FRAMERATE TRUE

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
	PLOGI << "Initialize Graphics";

	PLOGD << "Initialize Swap Chain";
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = static_cast<UINT>(width);
	swap_chain_desc.BufferDesc.Height = static_cast<UINT>(height);
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = parent_;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT swap_create_flags = 0u;
#if (IS_DEBUG)
	PLOGD << "Enable D3D11 Device Debugging";
	swap_create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_level;
	constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	// For checking results of D3D functions
	HRESULT hresult = {};

	PLOGV << "Create device, front/back buffers, swap chain, and rendering context";
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swap_create_flags,
		feature_level_array,
		2,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		p_swap_chain_.GetAddressOf(),
		p_device_.GetAddressOf(),
		&feature_level,
		p_device_context_.GetAddressOf()
	));

	create_render_target();

	PLOGV << "Create and set the depth stencil state";
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
	PLOGV << "p_device_->CreateDepthStencilState(&depth_stencil_desc, &p_depth_stencil_state)";
	GFX_THROW_INFO(p_device_->CreateDepthStencilState(&depth_stencil_desc, &p_depth_stencil_state));

	PLOGD << "Bind depth state to the pipeline";
	PLOGV << "p_context_->OMSetDepthStencilState(p_depth_stencil_state.Get(), 0u);";
	p_device_context_->OMSetDepthStencilState(p_depth_stencil_state.Get(), 0u);

	PLOGV << "Create depth stencil texture";
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
	PLOGV << "p_device_->CreateTexture2D(&depth_desc, nullptr, &p_depth_stencil)";
	GFX_THROW_INFO(p_device_->CreateTexture2D(&depth_desc, nullptr, &p_depth_stencil));

	PLOGV << "Create view of the depth stencil texture";
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {
		.Format = DXGI_FORMAT_D32_FLOAT,
		// May also be DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, and it will use the same as the depth_desc
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Flags = 0u,
		.Texture2D = {
			.MipSlice = 0u,
		}
	};

	PLOGV << "p_device_->CreateDepthStencilView(p_depth_stencil.Get(), &depth_stencil_view_desc, &p_depth_stencil_view_))";
	GFX_THROW_INFO(
		p_device_->CreateDepthStencilView(p_depth_stencil.Get(), &depth_stencil_view_desc, &p_depth_stencil_view_));

	PLOGD << "Bind the render target and stencil views";
	PLOGV << "p_context_->OMSetRenderTargets(1u, p_target_view_.GetAddressOf(), p_depth_stencil_view_.Get())";
	//  - Output Merger
	p_device_context_->OMSetRenderTargets(1u, p_render_target_view_.GetAddressOf(), p_depth_stencil_view_.Get());

	PLOGD << "Configure and set the viewport";
	//  - Rasterizer
	const D3D11_VIEWPORT viewport = {
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = width_,
		.Height = height_,
		.MinDepth = 0,
		.MaxDepth = 1
	};
	p_device_context_->RSSetViewports(1u, &viewport);

	PLOGD << "Setup Dear ImGui Render backend";
	PLOGV << "ImGui_ImplDX11_Init";
	ImGui_ImplDX11_Init(p_device_.Get(), p_device_context_.Get());
}

void graphics::create_render_target()
{
	HRESULT hresult;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
	PLOGD << "Get the address of the back buffer";
	wrl::ComPtr<ID3D11Resource> p_back_buffer;
	PLOGV << "p_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer))";
	GFX_THROW_INFO(p_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer)));

	PLOGD << "Use the back buffer address to create the render target";
	PLOGV << "p_device_->CreateRenderTargetView(p_back_buffer.Get(), nullptr, p_target_view_.GetAddressOf())";
	GFX_THROW_INFO(p_device_->CreateRenderTargetView(p_back_buffer.Get(), nullptr, p_render_target_view_.GetAddressOf()));
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

bool graphics::begin_frame(const unsigned int target_width, const unsigned int target_height)
{
	// Handle window being minimized or screen locked
	PLOGV << "p_swap_chain_->Present(0, DXGI_PRESENT_TEST)";
	if (swap_chain_occluded_ && p_swap_chain_->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		::Sleep(10);
		return false;
	}
	swap_chain_occluded_ = false;

	// Handle window resize (we don't resize directly in the WM_SIZE handler)
	if (target_width != 0 && target_height != 0)
	{

		if (p_swap_chain_)
		{
			p_device_context_->OMSetRenderTargets(0, 0, 0);

			// Release all outstanding references to the swap chain's buffers
			p_render_target_view_->Release();

			HRESULT hresult;
			// Preserve the existing buffer count and format
			// Automatically choose the width and height to match the client area rect for HWNDs.
			GFX_THROW_INFO(p_swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

			// Get buffer and create a render-target-view.
			ID3D11Texture2D* p_buffer;
			GFX_THROW_INFO(p_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&p_buffer)));

			GFX_THROW_INFO(p_device_->CreateRenderTargetView(p_buffer, nullptr, p_render_target_view_.GetAddressOf()));
			p_buffer->Release();

			p_device_context_->OMSetRenderTargets(1, p_render_target_view_.GetAddressOf(), nullptr);

			// ReSharper disable once CppInitializedValueIsAlwaysRewritten
			D3D11_VIEWPORT vp{};
			vp.Width = static_cast<float>(target_width);
			vp.Height = static_cast<float>(target_height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			p_device_context_->RSSetViewports(1, &vp);
		}
	}
	return true;
}

void graphics::end_frame()
{
	HRESULT hresult;
#if (IS_DEBUG)
	info_manager_.set();
#endif
#if (UNCAPPED_FRAMERATE)
	PLOGV << "p_swap_chain_->Present(0u, 0u)";
	if (FAILED(hresult = p_swap_chain_->Present(0u, 0u)))
#else
	PLOGV << "p_swap_chain_->Present(1u, 0u)";
	if (FAILED(hresult = p_swap_chain_->Present(1u, 0u)))
#endif
	{
		if (hresult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(p_device_->GetDeviceRemovedReason());
		}
		throw GFX_EXCEPT(hresult);
	}
	swap_chain_occluded_ = (hresult == DXGI_STATUS_OCCLUDED);
}

void graphics::clear_buffer(const ImVec4& clear_color) const
{
	clear_buffer(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
}

void graphics::clear_buffer(const float red, const float green, const float blue, const float alpha) const
{
	const float clear_color_with_alpha[4] = { red * alpha, green * alpha, blue * alpha, alpha };
	PLOGV << "p_context_->ClearRenderTargetView";
	p_device_context_->ClearRenderTargetView(p_render_target_view_.Get(), clear_color_with_alpha);
	PLOGV << "p_context_->ClearDepthStencilView";
	p_device_context_->ClearDepthStencilView(p_depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}


// ReSharper disable once CppMemberFunctionMayBeConst
void graphics::draw_indexed(const UINT count) noexcept(!IS_DEBUG)
{
	PLOGV << "p_context_->DrawIndexed( " << count << ", 0u, 0u)";
	GFX_THROW_INFO_ONLY(p_device_context_->DrawIndexed(count, 0u, 0u));
}

void graphics::set_projection(DirectX::FXMMATRIX& projection) noexcept
{
	projection_ = projection;
}

DirectX::XMMATRIX graphics::get_projection() const noexcept
{
	return projection_;
}

void graphics::shutdown()
{
	PLOGV << "ImGui_ImplDX11_Shutdown()";
	ImGui_ImplDX11_Shutdown();
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
	out << "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
		<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << '\n'
		<< "[Error String] " << get_error_string() << '\n'
		<< "[Description] " << get_error_description() << '\n';
	if (!info_message_.empty())
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

graphics::info_exception::info_exception(const int line, const char* file, const std::vector<std::string>& info_messages) noexcept
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
