#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename T>
class constant_buffer : public bindable
{
public:
	constant_buffer() = default;
	~constant_buffer() override = default;
	constant_buffer(const constant_buffer&) = delete;
	constant_buffer& operator=(const constant_buffer&) = delete;
	constant_buffer(const constant_buffer&&) = delete;
	constant_buffer& operator=(const constant_buffer&&) = delete;

	constant_buffer(graphics& graphics, const T& constant_buffer_struct)
	{
		INFOMAN(graphics);

		const D3D11_BUFFER_DESC buffer_desc = {
			.ByteWidth = sizeof(constant_buffer_struct),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0u,
			.StructureByteStride = 0u
		};

		const D3D11_SUBRESOURCE_DATA subresource_data = {
			.pSysMem = &constant_buffer_struct,
			.SysMemPitch = 0u,
			.SysMemSlicePitch = 0u
		};

		GFX_THROW_INFO(get_device(graphics)->CreateBuffer(&buffer_desc, &subresource_data, &p_constant_buffer_));
	}

	explicit constant_buffer(graphics& graphics) : bindable()
	{
		INFOMAN(graphics);

		const D3D11_BUFFER_DESC buffer_desc = {
			.ByteWidth = sizeof(T),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0u,
			.StructureByteStride = 0u
		};

		GFX_THROW_INFO(get_device(graphics)->CreateBuffer(&buffer_desc, nullptr, &p_constant_buffer_));
	}

	void update(graphics& graphics, const T& constant_buffer_struct)
	{
		INFOMAN(graphics);

		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		GFX_THROW_INFO(get_context(graphics)->Map(
			p_constant_buffer_.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&mapped_subresource));
		memcpy(mapped_subresource.pData, &constant_buffer_struct, sizeof(constant_buffer_struct));
		get_context(graphics)->Unmap(p_constant_buffer_.Get(), 0u);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_constant_buffer_;
};

template<typename T>
class vertex_constant_buffer : public constant_buffer<T>
{
public:
	using constant_buffer<T>::constant_buffer;
	void bind(graphics& graphics) noexcept override
	{
		bindable::get_context(graphics)->VSSetConstantBuffers(0u, 1u, constant_buffer<T>::p_constant_buffer_.GetAddressOf());
	}
};

template<typename T>
class pixel_constant_buffer : public constant_buffer<T>
{
public:
	using constant_buffer<T>::constant_buffer;
	void bind(graphics& graphics) noexcept override
	{
		bindable::get_context(graphics)->PSSetConstantBuffers(0u, 1u, constant_buffer<T>::p_constant_buffer_.GetAddressOf());
	}
};
