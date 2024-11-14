#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

index_buffer::index_buffer(const graphics& graphics, const std::vector<unsigned short>& indices)
	:
	count_(static_cast<UINT>(indices.size()))
{
	INFOMAN(graphics);

	const D3D11_BUFFER_DESC buffer_desc = {
		.ByteWidth = static_cast<UINT>(count_ * sizeof(unsigned short)),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = sizeof(unsigned short)
	};

	const D3D11_SUBRESOURCE_DATA subresource_data = {
		.pSysMem = indices.data(),
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(get_device(graphics)->CreateBuffer(&buffer_desc, &subresource_data, &p_index_buffer_));
}

void index_buffer::bind(graphics& graphics) noexcept
{
	get_context(graphics)->IASetIndexBuffer(p_index_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT index_buffer::get_count() const noexcept
{
	return count_;
}