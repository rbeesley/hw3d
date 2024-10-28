#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class index_buffer;
class bindable;

class drawable
{
	template<class T>
	friend class drawable_static_storage;

public:
	drawable() = default;
	virtual ~drawable() = default;
	drawable(const drawable&) = delete;
	drawable& operator=(const drawable&) = delete;
	drawable(const drawable&&) = delete;
	drawable& operator=(const drawable&&) = delete;

	virtual DirectX::XMMATRIX get_transform_xm() const noexcept = 0;
	void draw(graphics& graphics) const noexcept(!IS_DEBUG);
	virtual void update(float dt) noexcept = 0;

protected:
	virtual void add_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG);
	virtual void add_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept;

private:
	virtual const std::vector<std::unique_ptr<bindable>>& get_static_binds() const noexcept = 0;

private:
	const index_buffer* p_index_buffer_ = nullptr;
	std::vector<std::unique_ptr<bindable>> p_binds_;
};
