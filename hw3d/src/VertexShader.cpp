#include "VertexShader.h"
#include "Logging.h"

#include <d3dcompiler.h>

#include "GraphicsThrowMacros.h"

vertex_shader::vertex_shader(graphics& graphics, const std::wstring& path)
{
	INFOMAN(graphics);

	PLOGD << "D3DReadFileToBlob(\"" << path.c_str() << "\", [p_bytecode_blob_]));";
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &p_bytecode_blob_));
	GFX_THROW_INFO(get_device(graphics)->CreateVertexShader(p_bytecode_blob_->GetBufferPointer(), p_bytecode_blob_->GetBufferSize(), nullptr, &p_vertex_shader_));
}

void vertex_shader::bind(graphics& graphics) noexcept
{
	get_context(graphics)->VSSetShader(p_vertex_shader_.Get(), nullptr, 0u);
}

ID3DBlob* vertex_shader::get_byte_code() const noexcept
{
	return p_bytecode_blob_.Get();
}
