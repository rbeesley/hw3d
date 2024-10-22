#include "PixelShader.h"

#include <d3dcompiler.h>

#include "GraphicsThrowMacros.h"

pixel_shader::pixel_shader(graphics& graphics, const std::wstring& path)
{
	INFOMAN(graphics);

	Microsoft::WRL::ComPtr<ID3DBlob> p_blob;
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &p_blob));
	GFX_THROW_INFO(get_device(graphics)->CreatePixelShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), nullptr, &p_pixel_shader_));
}

void pixel_shader::bind(graphics& graphics) noexcept
{
	get_context(graphics)->PSSetShader(p_pixel_shader_.Get(), nullptr, 0u);
}
