#include "TransformConstantBuffer.h"

transform_constant_buffer::transform_constant_buffer(graphics& graphics, const drawable& parent)
	:
	parent_(parent)
{
	if (!p_vertex_constant_buffer_)
	{
		p_vertex_constant_buffer_ = std::make_unique<vertex_constant_buffer<DirectX::XMMATRIX>>(graphics);
	}
}

void transform_constant_buffer::bind(graphics& graphics) noexcept
{
	p_vertex_constant_buffer_->update(graphics,
		DirectX::XMMatrixTranspose(
			parent_.get_transform_xm() * graphics.get_projection()
		)
	);
	p_vertex_constant_buffer_->bind(graphics);
}

std::unique_ptr<vertex_constant_buffer<DirectX::XMMATRIX>> transform_constant_buffer::p_vertex_constant_buffer_;