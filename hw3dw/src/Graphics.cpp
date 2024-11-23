// ReSharper disable CppClangTidyClangDiagnosticExtraSemiStmt
#include "Graphics.hpp"
#include "GraphicsThrowMacros.hpp"
#include "DXErr.h"
#include "backends/imgui_impl_dx11.h"

#include "Logging.hpp"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include "WindowsMessageMap.hpp"
const static WindowsMessageMap windowsMessageMap;
#endif

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

Graphics::Graphics(HWND parent, int width, int height) :
	parent_(parent),
	width_(static_cast<float>(width)),
	height_(static_cast<float>(height)),
	projection_()
{
	PLOGI << "Initialize Graphics";

	PLOGD << "Initialize Swap Chain";
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = static_cast<UINT>(width);
	swapChainDesc.BufferDesc.Height = static_cast<UINT>(height);
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = parent_;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT swapCreateFlags = 0u;
#if (IS_DEBUG)
	PLOGD << "Enable D3D11 Device Debugging";
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	constexpr D3D_FEATURE_LEVEL featureLevelsArray[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};

	// For checking results of D3D functions
	HRESULT hresult = {};

#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "Create device, front/back buffers, swap chain, and rendering context";
#endif
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		featureLevelsArray,
		2,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		swapChain_.GetAddressOf(),
		device_.GetAddressOf(),
		&featureLevel,
		deviceContext_.GetAddressOf()
	));

	createRenderTarget();

#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "Create and set the depth stencil state";
#endif
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {
		.DepthEnable = TRUE,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS,
		.StencilEnable = FALSE,
		.StencilReadMask = 0,
		.StencilWriteMask = 0,
		.FrontFace = {},
		.BackFace = {}
	};

	wrl::ComPtr<ID3D11DepthStencilState> depthStencilState;
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "device_->CreateDepthStencilState(&depthStencilDesc, &depthStencilState)";
#endif
	GFX_THROW_INFO(device_->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));

	PLOGD << "Bind depth state to the pipeline";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "deviceContext_->OMSetDepthStencilState(depthStencilState.Get(), 0u);";
#endif
	deviceContext_->OMSetDepthStencilState(depthStencilState.Get(), 0u);

	PLOGV << "Create depth stencil texture";
	D3D11_TEXTURE2D_DESC depthDesc = {
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

	wrl::ComPtr<ID3D11Texture2D> depthStencil;
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "device_->CreateTexture2D(&depthDesc, nullptr, &depthStencil)";
#endif
	GFX_THROW_INFO(device_->CreateTexture2D(&depthDesc, nullptr, &depthStencil));

	PLOGV << "Create view of the depth stencil texture";
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {
		.Format = DXGI_FORMAT_D32_FLOAT,
		// May also be DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, and it will use the same as the depth_desc
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Flags = 0u,
		.Texture2D = {
			.MipSlice = 0u,
		}
	};

#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "device_->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &depthStencilView_))";
#endif
	GFX_THROW_INFO(
		device_->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &depthStencilView_));

	PLOGD << "Bind the render target and stencil views";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "deviceContext_->OMSetRenderTargets(1u, renderTargetView_.GetAddressOf(), depthStencilView_.Get())";
#endif
	//  - Output Merger
	deviceContext_->OMSetRenderTargets(1u, renderTargetView_.GetAddressOf(), depthStencilView_.Get());

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
	deviceContext_->RSSetViewports(1u, &viewport);

	PLOGD << "Setup Dear ImGui Render backend";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui_ImplDX11_Init";
#endif
	ImGui_ImplDX11_Init(device_.Get(), deviceContext_.Get());
}

LRESULT Graphics::handleMsg([[maybe_unused]] HWND window, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	switch(msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (const auto& io = ImGui::GetIO(); io.WantCaptureKeyboard)
		{
			return 1;
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		if (const auto& io = ImGui::GetIO(); io.WantCaptureMouse)
		{
			return 1;
		}
		break;
	default:
		break;
	}
	return 0;
}

void Graphics::createRenderTarget()
{
	HRESULT hresult;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
	PLOGD << "Get the address of the back buffer";
	wrl::ComPtr<ID3D11Resource> backBuffer;
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer))";
#endif
	GFX_THROW_INFO(swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	PLOGD << "Use the back buffer address to create the render target";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf())";
#endif
	GFX_THROW_INFO(device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf()));
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

bool Graphics::beginFrame(const unsigned int targetWidth, const unsigned int targetHeight)
{
	// Handle window being minimized or screen locked
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "swapChain_->Present(0, DXGI_PRESENT_TEST)";
#endif
	if (swapChainOccluded_ && swapChain_->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		::Sleep(10);
		return false;
	}
	swapChainOccluded_ = false;

	// Handle window resize (we don't resize directly in the WM_SIZE handler)
	if (targetWidth != 0 && targetHeight != 0)
	{

		if (swapChain_)
		{
			deviceContext_->OMSetRenderTargets(0, 0, 0);

			// Release all outstanding references to the swap chain's buffers
			renderTargetView_->Release();

			HRESULT hresult;
			// Preserve the existing buffer count and format
			// Automatically choose the width and height to match the client area rect for HWNDs.
			GFX_THROW_INFO(swapChain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

			// Get buffer and create a render-target-view.
			ID3D11Texture2D* buffer;
			GFX_THROW_INFO(swapChain_->GetBuffer(0, IID_PPV_ARGS(&buffer)));

			GFX_THROW_INFO(device_->CreateRenderTargetView(buffer, nullptr, renderTargetView_.GetAddressOf()));
			buffer->Release();

			deviceContext_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), nullptr);

			// ReSharper disable once CppInitializedValueIsAlwaysRewritten
			D3D11_VIEWPORT vp{};
			vp.Width = static_cast<float>(targetWidth);
			vp.Height = static_cast<float>(targetHeight);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			deviceContext_->RSSetViewports(1, &vp);
		}
	}
	return true;
}

void Graphics::endFrame()
{
	HRESULT hresult;
#if (IS_DEBUG)
	infoManager_.set();
#endif
#if (UNCAPPED_FRAMERATE)
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "swapChain_->Present(0u, 0u)";
#endif
	if (FAILED(hresult = swapChain_->Present(0u, 0u)))
#else
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "swapChain_->Present(1u, 0u)";
#endif
	if (FAILED(hresult = swapChain_->Present(1u, 0u)))
#endif
	{
		if (hresult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(device_->GetDeviceRemovedReason());
		}
		throw GFX_EXCEPT(hresult);
	}
	swapChainOccluded_ = (hresult == DXGI_STATUS_OCCLUDED);
}

void Graphics::clearBuffer(const ImVec4& clearColor) const
{
	clearBuffer(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
}

void Graphics::clearBuffer(const float red, const float green, const float blue, const float alpha) const
{
	const float clearColorWithAlpha[4] = { red * alpha, green * alpha, blue * alpha, alpha };
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "deviceContext_->ClearRenderTargetView";
#endif
	deviceContext_->ClearRenderTargetView(renderTargetView_.Get(), clearColorWithAlpha);
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "deviceContext_->ClearDepthStencilView";
#endif
	deviceContext_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}


// ReSharper disable once CppMemberFunctionMayBeConst
void Graphics::drawIndexed(const UINT count) noexcept(!IS_DEBUG)
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "deviceContext_->DrawIndexed( " << count << ", 0u, 0u)";
#endif
	GFX_THROW_INFO_ONLY(deviceContext_->DrawIndexed(count, 0u, 0u));
}

void Graphics::setProjection(DirectX::FXMMATRIX& projection) noexcept
{
	projection_ = projection;
}

DirectX::XMMATRIX Graphics::getProjection() const noexcept
{
	return projection_;
}

void Graphics::shutdown()
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui_ImplDX11_Shutdown()";
#endif
	ImGui_ImplDX11_Shutdown();
}

// Graphics Exception
std::string Graphics::GraphicsException::toNarrow(const wchar_t* s, const char fallback, const std::locale& loc) noexcept
{
	std::ostringstream out;
	while (*s != L'\0') {
		out << std::use_facet<std::ctype<wchar_t>>(loc).narrow(*s++, fallback);
	}
	return out.str();
}

Graphics::HResultException::HResultException(const int line, const char* file, const HRESULT hresult, const std::vector<std::string>& infoMessages) noexcept
	:
	GraphicsException(line, file),
	hresult_(hresult)
{
	// Join all the info messages into a single string
	for (const auto& message : infoMessages)
	{
		infoMessage_ += message;
		infoMessage_.push_back('\n');
	}
}

const char* Graphics::HResultException::what() const noexcept
{
	std::ostringstream out;
	out << "[Error Code] 0x" << std::hex << std::uppercase << getErrorCode()
		<< std::dec << " (" << static_cast<unsigned long>(getErrorCode()) << ")" << '\n'
		<< "[Error String] " << getErrorString() << '\n'
		<< "[Description] " << getErrorDescription() << '\n';
	if (!infoMessage_.empty())
	{
		out << "[Error Info]\n" << getErrorInfo() << '\n';
	}
	out << getOriginString();
	whatBuffer_ = out.str();
	return whatBuffer_.c_str();
}

const char* Graphics::HResultException::getType() const noexcept
{
	return "Atum Graphics Exception";
}

HRESULT Graphics::HResultException::getErrorCode() const noexcept
{
	return hresult_;
}

std::string Graphics::HResultException::getErrorString() const noexcept
{
#ifdef _UNICODE
	return toNarrow(DXGetErrorString(hresult_));
#else
	return DXGetErrorString(hresult_);
#endif
}

std::string Graphics::HResultException::getErrorDescription() const noexcept
{
#ifdef _UNICODE
	WCHAR buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return toNarrow(buffer);
#else
	char buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return buffer;
#endif
}

std::string Graphics::HResultException::getErrorInfo() const noexcept
{
	return infoMessage_;
}

Graphics::InfoException::InfoException(const int line, const char* file, const std::vector<std::string>& infoMessages) noexcept
	:
	GraphicsException(line, file)
{
	// Join all the info messages into a single string
	for (const auto& message : infoMessages)
	{
		infoMessage_ += message;
		infoMessage_.push_back('\n');
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream out;
	out << getType() << '\n';
	if (!infoMessage_.empty())
	{
		out << "[Error Info]\n" << getErrorInfo() << '\n';
	}
	out << getOriginString();
	whatBuffer_ = out.str();
	return whatBuffer_.c_str();
}

const char* Graphics::InfoException::getType() const noexcept
{
	return "Atum Graphics Info Exception";
}

std::string Graphics::InfoException::getErrorInfo() const noexcept
{
	return infoMessage_;
}

const char* Graphics::DeviceRemovedException::getType() const noexcept
{
	return "Atum Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
