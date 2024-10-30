#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "AtumMath.h"

class cone
{
public:
	template<class V>
	static indexed_triangle_list<V> make()
	{
		return make_tessellated<V>(24);
	}

	template<class V>
	static indexed_triangle_list<V> make_tessellated(const int longitudinal_divisions, std::function<void(std::vector<V>&)> set_attributes = nullptr) {
		namespace dx = DirectX;
		assert(longitudinal_divisions >= 3);
		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitude_angle = 2.0f * PI / static_cast<float>(longitudinal_divisions);

		// base vertices
		std::vector<V> vertices;
		for (int index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++) {
			vertices.emplace_back();
			auto vertex = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * static_cast<float>(index_longitude)));
			dx::XMStoreFloat3(&vertices.back().pos, vertex);
		}

		// the center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto index_center = static_cast<unsigned short>(vertices.size() - 1);

		// the tip
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto index_tip = static_cast<unsigned short>(vertices.size() - 1);

		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++) {
			indices.push_back(index_center);
			indices.push_back((index_longitude + 1) % longitudinal_divisions);
			indices.push_back(index_longitude);
		}

		// cone indices
		for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++) {
			indices.push_back(index_longitude);
			indices.push_back((index_longitude + 1) % longitudinal_divisions);
			indices.push_back(index_tip);
		}

		if (set_attributes) {
			set_attributes(vertices);
		}

		return indexed_triangle_list<V>{std::move(vertices), std::move(indices)};
	}
};