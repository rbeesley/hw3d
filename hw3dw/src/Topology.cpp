#include "Topology.h"

topology::topology(graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type_(type)
{
}

void topology::bind(graphics& graphics) noexcept
{
	get_context(graphics)->IASetPrimitiveTopology(type_);
}
