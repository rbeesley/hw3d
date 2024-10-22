#pragma once

#include "Graphics.h"

class bindable
{
public:
	virtual ~bindable() = default;
	bindable(const bindable&) = delete;
	bindable& operator=(const bindable&) = delete;
	bindable(const bindable&&) = delete;
	bindable& operator=(const bindable&&) = delete;

	virtual void bind(graphics& graphics) noexcept = 0;
protected:
	bindable() = default;

	static ID3D11DeviceContext* get_context(const graphics& graphics) noexcept;
	static ID3D11Device* get_device(const graphics& graphics) noexcept;
	static dxgi_info_manager& get_info_manager(graphics& graphics) noexcept(!IS_DEBUG);
};