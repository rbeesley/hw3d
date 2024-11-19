#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class IndexBuffer;
class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableStaticStorage;

public:
	Drawable() = default;
	virtual ~Drawable() = default;
	Drawable(const Drawable&) = delete;
	Drawable& operator=(const Drawable&) = delete;
	Drawable(const Drawable&&) = delete;
	Drawable& operator=(const Drawable&&) = delete;

	virtual DirectX::XMMATRIX getTransformXm() const noexcept = 0;
	void draw(Graphics& graphics) const noexcept(!IS_DEBUG);
	virtual void update(float dt) noexcept = 0;

protected:
	virtual void addBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	virtual void addIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept;

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& getStaticBinds() const noexcept = 0;

private:
	const IndexBuffer* indexBuffer_ = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds_;
};
