#pragma once
#include <DirectXMath.h>
#include <functional>
#include <vector>

template<class T>
class indexed_triangle_list
{
public:
	indexed_triangle_list() = default;
	indexed_triangle_list(std::vector<T> vertices, std::vector<unsigned short> indices)
		:
	vertices_(std::move(vertices)),
	indices_(std::move(indices))
	{
		assert(vertices_.size() > 2);
		assert(indices_.size() % 3 == 0);
	}

	//// Constructor accepting an optional lambda for setting vertex attributes
	//explicit indexed_triangle_list(std::vector<T> vertices, std::vector<unsigned short> indices, std::function<void(std::vector<T>&)> set_attributes = nullptr)
	//	: vertices_(std::move(vertices)), indices_(std::move(indices)) {
	//	assert(vertices_.size() > 2);
	//	assert(indices_.size() % 3 == 0);

	//	if (set_attributes) {
	//		set_attributes(vertices_);
	//	}
	//}

	void transform(DirectX::FXMMATRIX& matrix)
	{
		for(auto& vertex: vertices_)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&vertex.pos);
			DirectX::XMStoreFloat3(&vertex.pos, DirectX::XMVector3Transform(pos, matrix));
		}
	}

	// Getter for vertices (read-only)
	const std::vector<T>& vertices() const { return vertices_; }

	// Getter for indices (read-only)
	const std::vector<unsigned short>& indices() const { return indices_; }

private:
	std::vector<T> vertices_;
	std::vector<unsigned short> indices_;
};
