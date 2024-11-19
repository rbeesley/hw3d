#pragma once

#include "Graphics.h"

class Bindable
{
public:
	virtual ~Bindable() = default;
	Bindable(const Bindable&) = delete;
	Bindable& operator=(const Bindable&) = delete;
	Bindable(const Bindable&&) = delete;
	Bindable& operator=(const Bindable&&) = delete;

	virtual void bind(Graphics& graphics) noexcept = 0;
protected:
	Bindable() = default;

	static ID3D11DeviceContext* getContext(const Graphics& graphics) noexcept;
	static ID3D11Device* getDevice(const Graphics& graphics) noexcept;
	static DxgiInfoManager& getInfoManager(Graphics& graphics) noexcept(IS_DEBUG);
};