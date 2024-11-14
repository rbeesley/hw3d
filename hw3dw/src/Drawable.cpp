#include "Drawable.h"

#include <regex>

#include "IndexBuffer.h"
#include "Logging.h"

void drawable::draw(graphics& graphics) const noexcept(!IS_DEBUG)
{
	for (auto& bindable : p_binds_)
	{
		bindable->bind(graphics);
	}
	for (auto& bindable : get_static_binds())
	{
		bindable->bind(graphics);
	}
	graphics.draw_indexed(p_index_buffer_->get_count());
}

static std::string clean_type_name(const std::string& type_name) {
	// Regular expression to match the core type and strip out unnecessary parts
	const std::regex re(R"((.+?)(?=<struct))");
	if (std::smatch match; std::regex_search(type_name, match, re)) {
		return match.str();
	}
	return type_name;
}

void drawable::add_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG)
{
	PLOGV << "binding " << clean_type_name(typeid(*p_bind).name());
	assert("*Must* use add_index_buffer or add_static_index_buffer to bind index buffer" && typeid(*p_bind) != typeid(index_buffer));
	p_binds_.push_back(std::move(p_bind));
}

void drawable::add_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept
{
	PLOGV << "index buffer";
	assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
	p_index_buffer_ = p_index_buffer.get();
	p_binds_.push_back(std::move(p_index_buffer));
}
