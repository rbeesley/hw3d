#include "Sampler.h"
#include "GraphicsThrowMacros.h"

sampler::sampler(graphics& graphics)
{
	INFOMAN(graphics);

	D3D11_SAMPLER_DESC sampler_desc = {
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		//.MipLODBias = ,
		//.MaxAnisotropy = ,
		//.ComparisonFunc = ,
		//.BorderColor = ,
		//.MinLOD = ,
		//.MaxLOD = 
	};

	GFX_THROW_INFO(get_device(graphics)->CreateSamplerState(&sampler_desc, &p_sampler_));
}

void sampler::bind(graphics& graphics) noexcept
{
	get_context(graphics)->PSSetSamplers(0, 1, p_sampler_.GetAddressOf());
}