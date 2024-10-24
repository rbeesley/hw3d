#pragma once
#include "DrawableBase.h"
#include "IndexBuffer.h"
#include "TransformConstantBuffer.h"
#include "Logging.h"

template<class _>
class drawable : public drawable_base
{
public:
	// Copy and move semantics required for drawables
	//drawable(const drawable&) = delete;
	//drawable& operator=(const drawable&) = delete;
	//drawable(const drawable&&) = delete;
	//drawable& operator=(const drawable&&) = delete;

	static bool is_static_initialized() noexcept
	{
		return !static_binds_.empty();
	}

	static void add_static_bind(std::unique_ptr<bindable> p_bind) noexcept(!IS_DEBUG)
	{
		PLOGD << "static binding " << typeid(*p_bind).name();
		assert("*Must* use add_index_buffer or add_static_index_buffer to bind index buffer" && typeid(*p_bind) != typeid(index_buffer));
		static_binds_.push_back(std::move(p_bind));
	}

	void add_static_index_buffer(std::unique_ptr<index_buffer> p_index_buffer) noexcept(!IS_DEBUG)
	{
		PLOGD << "static index buffer";
		assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
		p_index_buffer_ = p_index_buffer.get();
		static_binds_.push_back(std::move(p_index_buffer));
	}

	void set_index_buffer_from_static_binds() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && p_index_buffer_ == nullptr);
		for(const auto& bind : static_binds_)
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
std::vector<std::unique_ptr<bindable>> drawable<T>::static_binds_;
