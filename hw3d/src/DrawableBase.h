#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

#include "IndexBuffer.h"

class bindable;

class drawable_base
{
	template<class T>
	friend class drawable;

public:
	virtual ~drawable_base() = default;
	drawable_base(const drawable_base&) = delete;
	drawable_base& operator=(const drawable_base&) = delete;
	drawable_base(const drawable_base&&) = delete;
	drawable_base& operator=(const drawable_base&&) = delete;

	virtual DirectX::XMMATRIX get_transform_xm() const noexcept = 0;
	void draw(graphics& graphics) const noexcept(!IS_DEBUG);
	virtual void update(float dt) noexcept = 0;

protected:
	virtual void add_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG);
	virtual void add_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept;

private:
	drawable_base() = default;

	virtual const std::vector<std::unique_ptr<bindable>>& get_static_binds() const noexcept = 0;

private:
	const class index_buffer* p_index_buffer_ = nullptr;
	std::vector<std::unique_ptr<bindable>> binds_;
};
