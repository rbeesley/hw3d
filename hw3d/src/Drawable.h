#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

#include "IndexBuffer.h"

class bindable;

class drawable
{
public:
	drawable(const drawable&) = delete;
	drawable& operator=(const drawable&) = delete;
	drawable(const drawable&&) = delete;
	drawable& operator=(const drawable&&) = delete;

	virtual DirectX::XMMATRIX get_transform_xm() const noexcept = 0;
	void draw(graphics& graphics) const noexcept(!IS_DEBUG);
	virtual void update(float dt) noexcept = 0;
	void add_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG);
	void add_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept;
protected:
	drawable() = default;
	virtual ~drawable() = default;
private:
	const index_buffer* p_index_buffer_ = nullptr;
	std::vector<std::unique_ptr<bindable>> binds_;
};