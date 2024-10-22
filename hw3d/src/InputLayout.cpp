#include "InputLayout.h"

#include "GraphicsThrowMacros.h"

input_layout::input_layout(graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input_element_descs, ID3DBlob* p_vertex_shader_bytecode)
{
	INFOMAN(graphics);

	GFX_THROW_INFO(get_device(graphics)->CreateInputLayout(
		input_element_descs.data(), static_cast<UINT>(input_element_descs.size()),
		p_vertex_shader_bytecode->GetBufferPointer(),
		p_vertex_shader_bytecode->GetBufferSize(),
		&p_input_layout_
	));
}

void input_layout::bind(graphics& graphics) noexcept
{
	get_context(graphics)->IASetInputLayout(p_input_layout_.Get());
}
