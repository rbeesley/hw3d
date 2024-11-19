#include "VertexBuffer.h"

void VertexBuffer::bind(Graphics& graphics) noexcept
{
	constexpr UINT offset = 0u;
	getContext(graphics)->IASetVertexBuffers(0u, 1u, vertexBuffer_.GetAddressOf(), &stride_, &offset);
}
