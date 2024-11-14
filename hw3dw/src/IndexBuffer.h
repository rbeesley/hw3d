#pragma once

#include "Bindable.h"

class index_buffer : public bindable
{
public:
	index_buffer() = default;
	index_buffer(const graphics& graphics, const std::vector<unsigned short>& indices);
	~index_buffer() override = default;
	index_buffer(const index_buffer&) = delete;
	index_buffer& operator=(const index_buffer&) = delete;
	index_buffer(const index_buffer&&) = delete;
	index_buffer& operator=(const index_buffer&&) = delete;

	void bind(graphics& graphics) noexcept override;
	UINT get_count() const noexcept;
protected:
	UINT count_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_index_buffer_;
};
