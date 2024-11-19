#include "Topology.h"

Topology::Topology(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type_(type)
{
}

void Topology::bind(Graphics& graphics) noexcept
{
	getContext(graphics)->IASetPrimitiveTopology(type_);
}
