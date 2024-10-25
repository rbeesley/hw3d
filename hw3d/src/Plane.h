#pragma once
#include <array>

#include "AtumMath.h"
#include "IndexedTriangleList.h"

class plane
{
public:
	template<class V>
	static indexed_triangle_list<V> make()
	{
		return plane::make_tessellated<V>(1, 1);
	}

	template<class V>
	static indexed_triangle_list<V> make_tessellated(const int divisions_x, const int divisions_y)
	{
		namespace dx = DirectX;
		assert(divisions_x >= 1);
		assert(divisions_y >= 1);

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int number_vertices_x = divisions_x + 1;
		const int number_vertices_y = divisions_y + 1;
		std::vector<V> vertices(number_vertices_x * number_vertices_y);
		{
			constexpr float side_x = width / 2.0f;
			constexpr float side_y = height / 2.0f;
			const float division_size_x = width / static_cast<float>(divisions_x);
			const float division_size_y = height / static_cast<float>(divisions_y);
			const auto bottom_left = dx::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

			for(int y = 0, i = 0; y < number_vertices_y; y++)
			{
				const float y_pos = static_cast<float>(y) * division_size_y;
				for(int x = 0; x < number_vertices_x; x++, i++)
				{
					const auto vertex = dx::XMVectorAdd(
						bottom_left,
						dx::XMVectorSet(static_cast<float>(x) * division_size_x, y_pos, 0.0f, 0.0f)
					);
					dx::XMStoreFloat3(&vertices[i].pos, vertex);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(static_cast<std::vector<unsigned short>::size_type>(sq(divisions_x * divisions_y)) * 6);
		{
			const auto vertex_pos_to_index = [number_vertices_x](const size_t x, const size_t y)
				{ return static_cast<unsigned short>(y * number_vertices_x + x); };

			for (size_t y = 0; y < divisions_y; y++)
			{
				for (size_t x = 0; x < divisions_x; x++)
				{
					const std::array<unsigned short, 4> index_array =
					{
						vertex_pos_to_index(x, y),
						vertex_pos_to_index(x + 1, y),
						vertex_pos_to_index(x, y + 1),
						vertex_pos_to_index(x + 1, y + 1)
					};
					indices.push_back(index_array[0]);
					indices.push_back(index_array[2]);
					indices.push_back(index_array[1]);
					indices.push_back(index_array[1]);
					indices.push_back(index_array[2]);
					indices.push_back(index_array[3]);
				}
			}
		}

		return
		{
			std::move(vertices),
			std::move(indices)
		};
	}
};
