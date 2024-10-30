#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "AtumMath.h"

class prism
{
public:
	template<class V>
	static indexed_triangle_list<V> make()
	{
		return make_tessellated<V>(24);
	}

	template<class V>
	static indexed_triangle_list<V> make_tessellated(const int longitudinal_divisions)
	{
		namespace dx = DirectX;
		assert(longitudinal_divisions >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitude_angle = 2.0f * PI / static_cast<float>(longitudinal_divisions);

		// near center
		std::vector<V> vertices;
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto index_center_near = static_cast<unsigned short>(vertices.size() - 1);

		// far center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto index_center_far = static_cast<unsigned short>(vertices.size() - 1);

		// base vertices
		for (int index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++)
		{
			// near base
			{
				vertices.emplace_back();
				auto vertex = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitude_angle * static_cast<float>(index_longitude))
				);
				dx::XMStoreFloat3(&vertices.back().pos, vertex);
			}

			// far base
			{
				vertices.emplace_back();
				auto vertex = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitude_angle * static_cast<float>(index_longitude))
				);
				vertex = dx::XMVectorAdd(vertex, offset);
				dx::XMStoreFloat3(&vertices.back().pos, vertex);
			}
		}

		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++)
		{
			const auto index = index_longitude * 2;
			const auto mod = longitudinal_divisions * 2;
			indices.push_back(index + 2);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back(index + 1 + 2);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back((index + 3) % mod + 2);
			indices.push_back(index + 1 + 2);
		}

		// base indices
		for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++)
		{
			const auto index = index_longitude * 2;
			const auto mod = longitudinal_divisions * 2;
			indices.push_back(index + 2);
			indices.push_back(index_center_near);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back(index_center_far);
			indices.push_back(index + 1 + 2);
			indices.push_back((index + 3) % mod + 2);
		}

		return
		{
			std::move(vertices),
			std::move(indices)
		};
	}
};