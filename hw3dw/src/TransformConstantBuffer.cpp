#include "TransformConstantBuffer.hpp"

TransformConstantBuffer::TransformConstantBuffer(Graphics& graphics, const Drawable& parent)
	:
	parent_(parent)
{
	if (!vertexConstantBuffer_)
	{
		vertexConstantBuffer_ = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphics);
	}
}

void TransformConstantBuffer::bind(Graphics& graphics) noexcept
{
	vertexConstantBuffer_->update(graphics,
		DirectX::XMMatrixTranspose(
			parent_.getTransformXm() * graphics.getProjection()
		)
	);
	vertexConstantBuffer_->bind(graphics);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::vertexConstantBuffer_;