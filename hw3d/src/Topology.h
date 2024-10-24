#pragma once
#include "Bindable.h"

class topology : public bindable
{
public:
	topology(graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type);

	topology() = delete;
	~topology() override = default;
	topology(const topology&) = delete;
	topology& operator=(const topology&) = delete;
	topology(const topology&&) = delete;
	topology& operator=(const topology&&) = delete;

	void bind(graphics& graphics) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type_;
};
