#pragma once
#include "Bindable.h"

class pixel_shader : public bindable
{
public:
	pixel_shader(graphics& graphics, const std::wstring& path);

	~pixel_shader() override = default;
	pixel_shader(const pixel_shader&) = delete;
	pixel_shader& operator=(const pixel_shader&) = delete;
	pixel_shader(const pixel_shader&&) = delete;
	pixel_shader& operator=(const pixel_shader&&) = delete;

	void bind(graphics& graphics) noexcept override;
protected:
	pixel_shader() = default;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> p_pixel_shader_;
};
