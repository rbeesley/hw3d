#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class VertexBuffer : public Bindable
{
public:
	template<class Vertex>
	VertexBuffer(Graphics& graphics, const std::vector<Vertex>& vertices)
		: Bindable(),
		stride_(sizeof(Vertex))
	{
		INFOMAN(graphics);

		const D3D11_BUFFER_DESC bufferDesc = {
			.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size()),
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = 0u,
			.MiscFlags = 0u,
			.StructureByteStride = sizeof(Vertex),
		};

		const D3D11_SUBRESOURCE_DATA subresourceData = {
			.pSysMem = vertices.data(),
			.SysMemPitch = 0u,
			.SysMemSlicePitch = 0u
		};

		GFX_THROW_INFO(getDevice(graphics)->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer_));
	}

	VertexBuffer() = delete;
	~VertexBuffer() override = default;
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(const VertexBuffer&&) = delete;
	VertexBuffer& operator=(const VertexBuffer&&) = delete;

	void bind(Graphics& graphics) noexcept override;
protected:
	UINT stride_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
};