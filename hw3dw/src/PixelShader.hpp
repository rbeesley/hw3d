#pragma once
#include "Bindable.hpp"

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics& graphics, const std::wstring& path);

	~PixelShader() override = default;
	PixelShader(const PixelShader&) = delete;
	PixelShader& operator=(const PixelShader&) = delete;
	PixelShader(const PixelShader&&) = delete;
	PixelShader& operator=(const PixelShader&&) = delete;

	void bind(Graphics& graphics) noexcept override;
protected:
	PixelShader() = default;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
};
