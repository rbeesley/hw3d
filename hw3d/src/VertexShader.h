#pragma once
#include "Bindable.h"

class vertex_shader : public bindable
{
public:
	vertex_shader(graphics& graphics, const std::wstring& path);

	~vertex_shader() override = default;
	vertex_shader(const vertex_shader&) = delete;
	vertex_shader& operator=(const vertex_shader&) = delete;
	vertex_shader(const vertex_shader&&) = delete;
	vertex_shader& operator=(const vertex_shader&&) = delete;

	void bind(graphics& graphics) noexcept override;
	ID3DBlob* get_byte_code() const noexcept;
protected:
	vertex_shader() = default;

	Microsoft::WRL::ComPtr<ID3DBlob> p_bytecode_blob_;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> p_vertex_shader_;
};
