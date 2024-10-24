#include "TransformConstantBuffer.h"

transform_constant_buffer::transform_constant_buffer(graphics& graphics, const drawable& parent)
	:
	  vertex_constant_buffer_(graphics),
	  parent_(parent)
{
}

void transform_constant_buffer::bind(graphics& graphics) noexcept
{
	vertex_constant_buffer_.update(graphics,
		DirectX::XMMatrixTranspose(
			parent_.get_transform_xm() * graphics.get_projection()
		)
	);
	vertex_constant_buffer_.bind(graphics);
}
