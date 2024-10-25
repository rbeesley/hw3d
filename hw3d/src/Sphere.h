#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "AtumMath.h"

class sphere
{
public:
	template<class V>
	static indexed_triangle_list<V> make()
	{
		return sphere::make_tessellated<V>(12, 24);
	}

	template<class V>
	static indexed_triangle_list<V> make_tessellated(const int latitudinal_divisions, const int longitudinal_divisions)
	{
		namespace dx = DirectX;
		assert(latitudinal_divisions >= 3);
		assert(longitudinal_divisions >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float latitudinal_angle = PI / static_cast<float>(latitudinal_divisions);
		const float longitude_angle = 2.0f * PI / static_cast<float>(longitudinal_divisions);

		std::vector<V> vertices;
		for (int index_latitude = 1; index_latitude < latitudinal_divisions; index_latitude++)
		{
			const auto latitude_base = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationX(latitudinal_angle * static_cast<float>(index_latitude))
			);

			for (int index_longitude = 0; index_longitude < longitudinal_divisions; index_longitude++)
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					latitude_base,
					dx::XMMatrixRotationZ(longitude_angle * static_cast<float>(index_longitude))
				);

				dx::XMStoreFloat3(&vertices.back().pos, v);
			}
		}

		// add the cap vertices
		const auto index_north_pole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, base);

		const auto index_south_pole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, dx::XMVectorNegate(base));

		const auto calc_index = [latitudinal_divisions, longitudinal_divisions](const unsigned short index_latitude, const unsigned short index_longitude)
			{ return index_latitude * longitudinal_divisions + index_longitude; };

		std::vector<unsigned short> indices;
		for (unsigned short index_latitude = 0; index_latitude < latitudinal_divisions - 2; index_latitude++)
		{
			for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions - 1; index_longitude++)
			{
				indices.push_back(calc_index(index_latitude, index_longitude));
				indices.push_back(calc_index(index_latitude + 1, index_longitude));
				indices.push_back(calc_index(index_latitude, index_longitude + 1));
				indices.push_back(calc_index(index_latitude, index_longitude + 1));
				indices.push_back(calc_index(index_latitude + 1, index_longitude));
				indices.push_back(calc_index(index_latitude + 1, index_longitude + 1));
			}

			// wrap band
			indices.push_back(calc_index(index_latitude, longitudinal_divisions - 1));
			indices.push_back(calc_index(index_latitude + 1, longitudinal_divisions - 1));
			indices.push_back(calc_index(index_latitude, 0));
			indices.push_back(calc_index(index_latitude, 0));
			indices.push_back(calc_index(index_latitude + 1, longitudinal_divisions - 1));
			indices.push_back(calc_index(index_latitude + 1, 0));
		}

		// cap fans
		for (unsigned short index_longitude = 0; index_longitude < longitudinal_divisions - 1; index_longitude++)
		{
			// north
			indices.push_back(index_north_pole);
			indices.push_back(calc_index(0, index_longitude));
			indices.push_back(calc_index(0, index_longitude + 1));

			// south
			indices.push_back(calc_index(latitudinal_divisions - 2, index_longitude + 1));
			indices.push_back(calc_index(latitudinal_divisions - 2, index_longitude));
			indices.push_back(index_south_pole);
		}

		// wrap triangles
		// north
		indices.push_back(index_north_pole);
		indices.push_back(calc_index(0, longitudinal_divisions - 1));
		indices.push_back(calc_index(0, 0));

		// south
		indices.push_back(calc_index(latitudinal_divisions - 2, 0));
		indices.push_back(calc_index(latitudinal_divisions - 2, longitudinal_divisions - 1));
		indices.push_back(index_south_pole);

		return
		{
			std::move(vertices),
			std::move(indices)
		};
	}
};