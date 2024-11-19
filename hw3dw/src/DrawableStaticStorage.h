#pragma once
#include <regex>

#include "Drawable.h"
#include "IndexBuffer.h"
#include "TransformConstantBuffer.h"
#include "Logging.h"

template<class _>
class DrawableStaticStorage : public Drawable
{
public:
	// Copy and move semantics required for drawables
	//DrawableStaticStorage(const DrawableStaticStorage&) = delete;
	//DrawableStaticStorage& operator=(const DrawableStaticStorage&) = delete;
	//DrawableStaticStorage(const DrawableStaticStorage&&) = delete;
	//DrawableStaticStorage& operator=(const DrawableStaticStorage&&) = delete;

	static bool isStaticInitialized() noexcept
	{
		return !staticBinds_.empty();
	}

	static std::string cleanTypeName(const std::string& typeName) {
		// Regular expression to match the core type and strip out unnecessary parts
		const std::regex re(R"((.+?)(?=<struct))");
		if (std::smatch match; std::regex_search(typeName, match, re)) {
			return match.str();
		}
		return typeName;
	}

	static void addStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		PLOGV << "binding " << cleanTypeName(typeid(*bind).name());
		assert("*Must* use add_index_buffer or addStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds_.push_back(std::move(bind));
	}

	void addStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept(!IS_DEBUG)
	{
		PLOGV << "index buffer";
		assert("Attempting to add index buffer a second time" && indexBuffer_ == nullptr);
		indexBuffer_ = indexBuffer.get();
		staticBinds_.push_back(std::move(indexBuffer));
	}

	void setIndexBufferFromStaticBinds() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && indexBuffer_ == nullptr);
		for (const auto& bind : staticBinds_)
		{
			if (const auto indexBuffer = dynamic_cast<IndexBuffer*>(bind.get()))
			{
				indexBuffer_ = indexBuffer;
				return;
			}
		}
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& getStaticBinds() const noexcept override
	{
		return staticBinds_;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds_;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableStaticStorage<T>::staticBinds_;
