#pragma once
#include "Bindable.hpp"

class Sampler : public Bindable
{
public:
	explicit Sampler(Graphics& graphics);
	void bind(Graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;
};