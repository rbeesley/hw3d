#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

class vertex_buffer : public bindable
{
public:
	template<class Vertex>
	vertex_buffer(graphics& graphics, const std::vector<Vertex>& vertices)
		: bindable(),
		  stride(sizeof(Vertex))
	{
		INFOMAN(graphics);

		const D3D11_BUFFER_DESC buffer_desc = {
			.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size()),
			.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = 0u,
			.MiscFlags = 0u,
			.StructureByteStride = sizeof(Vertex),
		};

		const D3D11_SUBRESOURCE_DATA subresource_data = {
			.pSysMem = vertices.data(),
			.SysMemPitch = 0u,
			.SysMemSlicePitch = 0u
		};

		GFX_THROW_INFO(get_device(graphics)->CreateBuffer(&buffer_desc, &subresource_data, &p_vertex_buffer));
	}

	vertex_buffer() = delete;
	~vertex_buffer() override = default;
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer(const vertex_buffer&&) = delete;
	vertex_buffer& operator=(const vertex_buffer&&) = delete;

	void bind(graphics& graphics) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_vertex_buffer;
};