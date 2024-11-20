#include "GraphicsThrowMacros.hpp"
#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer(const Graphics& graphics, const std::vector<unsigned short>& indices)
	:
	count_(static_cast<UINT>(indices.size()))
{
	INFOMAN(graphics);

	const D3D11_BUFFER_DESC bufferDesc = {
		.ByteWidth = static_cast<UINT>(count_ * sizeof(unsigned short)),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = sizeof(unsigned short)
	};

	const D3D11_SUBRESOURCE_DATA subresourceData = {
		.pSysMem = indices.data(),
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(getDevice(graphics)->CreateBuffer(&bufferDesc, &subresourceData, &indexBuffer_));
}

void IndexBuffer::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::getCount() const noexcept
{
	return count_;
}