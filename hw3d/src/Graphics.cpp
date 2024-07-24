#include "Graphics.h"

#include "LoggingConfig.h"
#include "Logging.h"

#pragma comment(lib, "d3d11.lib")

graphics::graphics(HWND window_handle)
{
	PLOGI << "Initializing Graphics";

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
//	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // BGRA
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = window_handle;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// Create device and front/back buffers, swap chain, and rendering context
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&p_swap_chain_,
		&p_device_,
		nullptr,
		&p_context_
	);

	// Gain access to the texture subresource in the swap chain (back buffer)
	ID3D11Resource* p_back_buffer = nullptr;
	p_swap_chain_->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&p_back_buffer));
	p_device_->CreateRenderTargetView(p_back_buffer, nullptr, &p_target_);
	p_back_buffer->Release();
}

graphics::~graphics()
{
	if (p_target_ != nullptr)
	{
		p_target_->Release();
	}
	if (p_swap_chain_ != nullptr)
	{
		p_swap_chain_->Release();
	}
	if (p_context_ != nullptr)
	{
		p_context_->Release();
	}
	if (p_device_ != nullptr)
	{
		p_device_->Release();
	}
}

void graphics::end_frame()
{
	p_swap_chain_->Present(1u, 0u);
}

void graphics::clear_buffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	p_context_->ClearRenderTargetView(p_target_, color);
}
