#include "Sampler.hpp"
#include "GraphicsThrowMacros.hpp"

Sampler::Sampler(Graphics& graphics)
{
	INFOMAN(graphics);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(getDevice(graphics)->CreateSamplerState(&samplerDesc, &sampler_));
}

void Sampler::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->PSSetSamplers(0, 1, sampler_.GetAddressOf());
}