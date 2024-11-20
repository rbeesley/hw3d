#pragma once

#include "Bindable.hpp"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer() = default;
	IndexBuffer(const Graphics& graphics, const std::vector<unsigned short>& indices);
	~IndexBuffer() override = default;
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer(const IndexBuffer&&) = delete;
	IndexBuffer& operator=(const IndexBuffer&&) = delete;

	void bind(Graphics& graphics) noexcept override;
	UINT getCount() const noexcept;
protected:
	UINT count_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
};
