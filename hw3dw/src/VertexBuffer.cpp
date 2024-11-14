#include "VertexBuffer.h"

void vertex_buffer::bind(graphics& graphics) noexcept
{
	constexpr UINT offset = 0u;
	get_context(graphics)->IASetVertexBuffers(0u, 1u, p_vertex_buffer.GetAddressOf(), &stride, &offset);
}
