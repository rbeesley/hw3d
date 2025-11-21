#pragma once
#include "Bindable.hpp"
#include "GraphicsThrowMacros.hpp"

template<typename T>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() override = default;
	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;
	ConstantBuffer(const ConstantBuffer&&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&&) = delete;

	ConstantBuffer(Graphics& graphics, const T& constantBufferStruct)
	{
		INFOMAN(graphics);

		const D3D11_BUFFER_DESC bufferDesc = {
			.ByteWidth = sizeof(constantBufferStruct),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags = 0u,
			.StructureByteStride = 0u
		};

		const D3D11_SUBRESOURCE_DATA subresourceData = {
			.pSysMem = &constantBufferStruct,
			.SysMemPitch = 0u,
			.SysMemSlicePitch = 0u
		};

		GFX_THROW_INFO(getDevice(graphics)->CreateBuffer(&bufferDesc, &subresourceData, &m_constantBuffer));
	}

	explicit ConstantBuffer(Graphics& graphics) : Bindable()
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

		GFX_THROW_INFO(getDevice(graphics)->CreateBuffer(&buffer_desc, nullptr, &m_constantBuffer));
	}

	void update(Graphics& graphics, const T& constant_buffer_struct)
	{
		INFOMAN(graphics);

		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		GFX_THROW_INFO(getContext(graphics)->Map(
			m_constantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&mapped_subresource));
		memcpy(mapped_subresource.pData, &constant_buffer_struct, sizeof(constant_buffer_struct));
		getContext(graphics)->Unmap(m_constantBuffer.Get(), 0u);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
};

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	void bind(Graphics& graphics) noexcept override
	{
		Bindable::getContext(graphics)->VSSetConstantBuffers(0u, 1u, ConstantBuffer<T>::m_constantBuffer.GetAddressOf());
	}
};

template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	void bind(Graphics& graphics) noexcept override
	{
		Bindable::getContext(graphics)->PSSetConstantBuffers(0u, 1u, ConstantBuffer<T>::m_constantBuffer.GetAddressOf());
	}
};
