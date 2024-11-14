#pragma once
#include "Bindable.h"

class sampler : public bindable
{
public:
	explicit sampler(graphics& graphics);
	void bind(graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> p_sampler_;
};