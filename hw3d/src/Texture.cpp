#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

texture::texture(graphics& graphics, const surface& surface)
{
	INFOMAN(graphics);

	// create texture resource
	const D3D11_TEXTURE2D_DESC texture_desc = {
		.Width = surface.get_width(),
		.Height = surface.get_height(),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	const D3D11_SUBRESOURCE_DATA subresource_data = {
		.pSysMem = surface.get_buffer_ptr(),
		.SysMemPitch = static_cast<unsigned int>(surface.get_width() * sizeof(surface::color)),
		.SysMemSlicePitch = 0,
	};

	wrl::ComPtr<ID3D11Texture2D> p_texture;
	GFX_THROW_INFO(get_device(graphics)->CreateTexture2D(
		&texture_desc, &subresource_data, &p_texture
	));

	// create the resource view on the texture
	const D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {
		.Format = texture_desc.Format,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D = {
			.MostDetailedMip = 0,
			.MipLevels = 1, 
		},
	};

	GFX_THROW_INFO(get_device(graphics)->CreateShaderResourceView(
		p_texture.Get(), &shader_resource_view_desc, &p_texture_view_
	));
}

void texture::bind(graphics& graphics) noexcept
{
	get_context(graphics)->PSSetShaderResources(0u, 1u, p_texture_view_.GetAddressOf());
}