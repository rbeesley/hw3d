#include "VertexShader.hpp"
#include "Logging.hpp"

#include <d3dcompiler.h>

#include "GraphicsThrowMacros.hpp"

VertexShader::VertexShader(Graphics& graphics, const std::wstring& path)
{
	INFOMAN(graphics);

	PLOGD << "D3DReadFileToBlob(\"" << path.c_str() << "\", [bytecodeBlob_]));";
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &bytecodeBlob_));
	GFX_THROW_INFO(getDevice(graphics)->CreateVertexShader(bytecodeBlob_->GetBufferPointer(), bytecodeBlob_->GetBufferSize(), nullptr, &vertexShader_));
}

void VertexShader::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->VSSetShader(vertexShader_.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::getByteCode() const noexcept
{
	return bytecodeBlob_.Get();
}
