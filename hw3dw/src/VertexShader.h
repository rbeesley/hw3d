#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& graphics, const std::wstring& path);

	~VertexShader() override = default;
	VertexShader(const VertexShader&) = delete;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader(const VertexShader&&) = delete;
	VertexShader& operator=(const VertexShader&&) = delete;

	void bind(Graphics& graphics) noexcept override;
	ID3DBlob* getByteCode() const noexcept;
protected:
	VertexShader() = default;

	Microsoft::WRL::ComPtr<ID3DBlob> bytecodeBlob_;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
};
