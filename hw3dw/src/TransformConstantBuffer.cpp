#include "TransformConstantBuffer.hpp"

#include "plog/Log.h"

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

	auto world = parent_.getTransformXm();
	auto view = graphics.getCamera()->getView();
	auto proj = graphics.getCamera()->getProjection();

#if false
	DirectX::XMFLOAT4X4 w, v, p;
	XMStoreFloat4x4(&w, world);
	XMStoreFloat4x4(&v, view);
	XMStoreFloat4x4(&p, proj);

	PLOGD << "World: " << w._11 << "," << w._22 << "," << w._33 << "," << w._44;
	PLOGD << "View: " << v._11 << "," << v._22 << "," << v._33 << "," << v._44;
	PLOGD << "Proj: " << p._11 << "," << p._22 << "," << p._33 << "," << p._44;
#endif

	vertexConstantBuffer_->update(graphics,
		DirectX::XMMatrixTranspose(
			world *
			view *
			proj
		)
	);
	vertexConstantBuffer_->bind(graphics);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::vertexConstantBuffer_;