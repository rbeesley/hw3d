#pragma once
#include "Bindable.h"

class input_layout : public bindable
{
public:
	input_layout() = default;
	input_layout(graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input_element_descs, ID3DBlob* p_vertex_shader_bytecode);
	~input_layout() override = default;
	input_layout(const input_layout&) = delete;
	input_layout& operator=(const input_layout&) = delete;
	input_layout(const input_layout&&) = delete;
	input_layout& operator=(const input_layout&&) = delete;

	void bind(graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> p_input_layout_;
};
