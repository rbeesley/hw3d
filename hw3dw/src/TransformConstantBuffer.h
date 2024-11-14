#pragma once

#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class transform_constant_buffer : public bindable
{
public:
	transform_constant_buffer(graphics& graphics, const drawable& parent);

	transform_constant_buffer() = delete;
	~transform_constant_buffer() override = default;
	transform_constant_buffer(const transform_constant_buffer&) = delete;
	transform_constant_buffer& operator=(const transform_constant_buffer&) = delete;
	transform_constant_buffer(const transform_constant_buffer&&) = delete;
	transform_constant_buffer& operator=(const transform_constant_buffer&&) = delete;

	void bind(graphics& graphics) noexcept override;
private:
	static std::unique_ptr<vertex_constant_buffer<DirectX::XMMATRIX>> p_vertex_constant_buffer_;
	const drawable& parent_;
};
