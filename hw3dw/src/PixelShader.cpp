#include "PixelShader.h"

#include <d3dcompiler.h>

#include "GraphicsThrowMacros.h"

PixelShader::PixelShader(Graphics& graphics, const std::wstring& path)
{
	INFOMAN(graphics);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &blob));
	GFX_THROW_INFO(getDevice(graphics)->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader_));
}

void PixelShader::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->PSSetShader(pixelShader_.Get(), nullptr, 0u);
}
