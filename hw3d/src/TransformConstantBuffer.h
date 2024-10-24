#pragma once

#pragma once
#include "ConstantBuffers.h"
#include "DrawableBase.h"
#include <DirectXMath.h>

class transform_constant_buffer : public bindable
{
public:
	transform_constant_buffer(graphics& graphics, const drawable_base& parent);

	transform_constant_buffer() = delete;
	~transform_constant_buffer() override = default;
	transform_constant_buffer(const transform_constant_buffer&) = delete;
	transform_constant_buffer& operator=(const transform_constant_buffer&) = delete;
	transform_constant_buffer(const transform_constant_buffer&&) = delete;
	transform_constant_buffer& operator=(const transform_constant_buffer&&) = delete;

	void bind(graphics& graphics) noexcept override;
private:
	vertex_constant_buffer<DirectX::XMMATRIX> vertex_constant_buffer_;
	const drawable_base& parent_;
};
