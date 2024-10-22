#include "Drawable.h"

#include "IndexBuffer.h"

void drawable::draw(graphics& graphics) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds_)
	{
		b->bind(graphics);
	}
	graphics.draw_indexed(p_index_buffer_->get_count());
}

void drawable::add_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG)
{
	assert("*Must* use add_index_buffer to bind index buffer" && typeid(*bind) != typeid(index_buffer));
	binds_.push_back(std::move(p_bind));
}

void drawable::add_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept
{
	assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
	p_index_buffer_ = p_index_buffer.get();
	binds_.push_back(std::move(p_index_buffer));
}
