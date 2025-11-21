#include "InputLayout.hpp"

#include "GraphicsThrowMacros.hpp"

InputLayout::InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, ID3DBlob* vertexShaderBytecode)
{
	INFOMAN(graphics);

	GFX_THROW_INFO(getDevice(graphics)->CreateInputLayout(
		inputElementDescs.data(), static_cast<UINT>(inputElementDescs.size()),
		vertexShaderBytecode->GetBufferPointer(),
		vertexShaderBytecode->GetBufferSize(),
		&inputLayout_
	));
}

void InputLayout::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->IASetInputLayout(inputLayout_.Get());
}
