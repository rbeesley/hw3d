#pragma once

#pragma once
#include "ConstantBuffers.hpp"
#include "Drawable.hpp"
#include <DirectXMath.h>

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(Graphics& graphics, const Drawable& parent);

	TransformConstantBuffer() = delete;
	~TransformConstantBuffer() override = default;
	TransformConstantBuffer(const TransformConstantBuffer&) = delete;
	TransformConstantBuffer& operator=(const TransformConstantBuffer&) = delete;
	TransformConstantBuffer(const TransformConstantBuffer&&) = delete;
	TransformConstantBuffer& operator=(const TransformConstantBuffer&&) = delete;

	void bind(Graphics& graphics) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> vertexConstantBuffer_;
	const Drawable& parent_;
};
