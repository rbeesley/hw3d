#pragma once
#include "Bindable.h"
#include "Surface.h"

class texture : public bindable
{
public:
	texture() = default;
	texture(graphics& graphics, const surface& surface);
	~texture() override = default;
	texture(const texture&) = delete;
	texture& operator=(const texture&) = delete;
	texture(const texture&&) = delete;
	texture& operator=(const texture&&) = delete;

	void bind(graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_texture_view_;
};
