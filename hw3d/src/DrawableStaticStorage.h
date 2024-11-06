#pragma once
#include <regex>

#include "Drawable.h"
#include "IndexBuffer.h"
#include "TransformConstantBuffer.h"
#include "Logging.h"

template<class _>
class drawable_static_storage : public drawable
{
public:
	// Copy and move semantics required for drawables
	//drawable_static_storage(const drawable_static_storage&) = delete;
	//drawable_static_storage& operator=(const drawable_static_storage&) = delete;
	//drawable_static_storage(const drawable_static_storage&&) = delete;
	//drawable_static_storage& operator=(const drawable_static_storage&&) = delete;

	static bool is_static_initialized() noexcept
	{
		return !static_binds_.empty();
	}

	static std::string clean_type_name(const std::string& type_name) {
		// Regular expression to match the core type and strip out unnecessary parts
		const std::regex re(R"((.+?)(?=<struct))");
		if (std::smatch match; std::regex_search(type_name, match, re)) {
			return match.str();
		}
		return type_name;
	}

	static void add_static_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG)
	{
		PLOGV << "binding " << clean_type_name(typeid(*p_bind).name());
		assert("*Must* use add_index_buffer or add_static_index_buffer to bind index buffer" && typeid(*p_bind) != typeid(index_buffer));
		static_binds_.push_back(std::move(p_bind));
	}

	void add_static_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept(!IS_DEBUG)
	{
		PLOGV << "index buffer";
		assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
		p_index_buffer_ = p_index_buffer.get();
		static_binds_.push_back(std::move(p_index_buffer));
	}

	void set_index_buffer_from_static_binds() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
		for (const auto& bind : static_binds_)
		{
			if (const auto p_index_buffer = dynamic_cast<index_buffer*>(bind.get()))
			{
				p_index_buffer_ = p_index_buffer;
				return;
			}
		}
	}

private:
	const std::vector<std::unique_ptr<bindable>>& get_static_binds() const noexcept override
	{
		return static_binds_;
	}

private:
	static std::vector<std::unique_ptr<bindable>> static_binds_;
};

template<class T>
std::vector<std::unique_ptr<bindable>> drawable_static_storage<T>::static_binds_;
