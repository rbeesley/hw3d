#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type);

	Topology() = delete;
	~Topology() override = default;
	Topology(const Topology&) = delete;
	Topology& operator=(const Topology&) = delete;
	Topology(const Topology&&) = delete;
	Topology& operator=(const Topology&&) = delete;

	void bind(Graphics& graphics) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type_;
};
