#include "Drawable.hpp"

#include <regex>

#include "IndexBuffer.hpp"
#include "Logging.hpp"

void Drawable::draw(Graphics& graphics) const noexcept(!IS_DEBUG)
{
	for (auto& bindable : binds_)
	{
		bindable->bind(graphics);
	}
	for (auto& bindable : getStaticBinds())
	{
		bindable->bind(graphics);
	}
	graphics.drawIndexed(indexBuffer_->getCount());
}

static std::string cleanTypeName(const std::string& typeName) {
	// Regular expression to match the core type and strip out unnecessary parts
	const std::regex re(R"((.+?)(?=<struct))");
	if (std::smatch match; std::regex_search(typeName, match, re)) {
		return match.str();
	}
	return typeName;
}

void Drawable::addBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "binding " << cleanTypeName(typeid(*bind).name());
#endif
	assert("*Must* use addIndexBuffer or add_static_index_buffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds_.push_back(std::move(bind));
}

void Drawable::addIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "index buffer";
#endif
	assert("Attempting to add index buffer a second time" && indexBuffer_ == nullptr);
	indexBuffer_ = indexBuffer.get();
	binds_.push_back(std::move(indexBuffer));
}
