#pragma once
#include "AtumWindows.h"
#include <d3d11.h>

class graphics
{
public:
	graphics(HWND window_handle);
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;
	~graphics();
	void end_frame();
	void clear_buffer(float red, float green, float blue) noexcept;
private:
	ID3D11Device* p_device_ = nullptr;
	IDXGISwapChain* p_swap_chain_ = nullptr;
	ID3D11DeviceContext* p_context_ = nullptr;
	ID3D11RenderTargetView* p_target_ = nullptr;
};
