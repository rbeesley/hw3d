#include "Texture.hpp"
#include "Surface.hpp"
#include "GraphicsThrowMacros.hpp"

namespace wrl = Microsoft::WRL;

Texture::Texture(Graphics& graphics, const Surface& surface)
{
	INFOMAN(graphics);

	// create texture resource
	const D3D11_TEXTURE2D_DESC textureDesc = {
		.Width = surface.getWidth(),
		.Height = surface.getHeight(),
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

	const D3D11_SUBRESOURCE_DATA subresourceData = {
		.pSysMem = surface.getBufferPtr(),
		.SysMemPitch = static_cast<unsigned int>(surface.getWidth() * sizeof(Surface::color)),
		.SysMemSlicePitch = 0,
	};

	wrl::ComPtr<ID3D11Texture2D> texture;
	GFX_THROW_INFO(getDevice(graphics)->CreateTexture2D(
		&textureDesc, &subresourceData, &texture
	));

	// create the resource view on the texture
	const D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {
		.Format = textureDesc.Format,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D = {
			.MostDetailedMip = 0,
			.MipLevels = 1,
		},
	};

	GFX_THROW_INFO(getDevice(graphics)->CreateShaderResourceView(
		texture.Get(), &shaderResourceViewDesc, &textureView_
	));
}

void Texture::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->PSSetShaderResources(0u, 1u, textureView_.GetAddressOf());
}