#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout() = default;
	InputLayout(Graphics& graphics, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDescs, ID3DBlob* vertexShaderBytecode);
	~InputLayout() override = default;
	InputLayout(const InputLayout&) = delete;
	InputLayout& operator=(const InputLayout&) = delete;
	InputLayout(const InputLayout&&) = delete;
	InputLayout& operator=(const InputLayout&&) = delete;

	void bind(Graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
};
