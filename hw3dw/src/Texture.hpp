#pragma once
#include "Bindable.hpp"
#include "Surface.hpp"

class Texture : public Bindable
{
public:
	Texture() = default;
	Texture(Graphics& graphics, const Surface& surface);
	~Texture() override = default;
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(const Texture&&) = delete;
	Texture& operator=(const Texture&&) = delete;

	void bind(Graphics& graphics) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView_;
};
