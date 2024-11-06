#include "Sampler.h"
#include "GraphicsThrowMacros.h"

sampler::sampler(graphics& graphics)
{
	INFOMAN(graphics);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(get_device(graphics)->CreateSamplerState(&sampler_desc, &p_sampler_));
}

void sampler::bind(graphics& graphics) noexcept
{
	get_context(graphics)->PSSetSamplers(0, 1, p_sampler_.GetAddressOf());
}