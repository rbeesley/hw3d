#pragma once

#include "IndexedTriangleList.h"
#include "DirectXMath.h"

class cube
{
public:
	template<class V>
	static indexed_triangle_list<V> make()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<dx::XMFLOAT3> vertices = {
		//    6-------7
		//   /|      /|
		//  2-------3 |
		//  | |     | |
		//  | 4-----|-5
		//  |/      |/
		//  0-------1

		{-side, -side, -side},	// Bottom-left-front vertex
		{side, -side, -side},	// Bottom-right-front vertex
		{-side, side, -side},	// Top-left-front vertex
		{side, side, -side},	// Top-right-front vertex
		{-side, -side, side},	// Bottom-left-back vertex
		{side, -side, side},	// Bottom-right-back vertex
		{-side, side, side},	// Top-left-back vertex
		{side, side, side},	// Top-right-back vertex
		};

		std::vector<V> indices(vertices.size());
		for(size_t i = 0; i < vertices.size(); i++)
		{
			indices[i].pos = vertices[i];
		}

		return indexed_triangle_list<V>{
			std::move(indices), {
				//         2------6
				//         | 4  //|
				//	       |  //  |
				//	       |//  5 |
				//  2------3------7------6------2
				//  |\\  1 |\\  3 |\\  7 | 9  //|
				//  |  \\  |  \\  |  \\  |  //  |
				//  | 0  \\| 2  \\| 6  \\|//  8 |
				//  0------1------5------4------0
				//         |\\ 11 |
				//	       |  \\  |
				//	       | 10 \\|
				//	       0------4

				0, 2, 1,   2, 3, 1, // Front
				1, 3, 5,   3, 7, 5, // Right
				2, 6, 3,   3, 6, 7, // Top
				4, 5, 7,   4, 7, 6, // Back
				0, 4, 2,   2, 4, 6, // Left
				0, 1, 4,   1, 5, 4, // Bottom
			}
		};
	}

	template<class V>
	static indexed_triangle_list<V> make_skinned()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices = {
			//    6-------7
			//   /|      /|
			//  2-------3 |
			//  | |     | |
			//  | 4-----|-5
			//  |/      |/
			//  0-------1

			//         6------7
			//         |M\   P|
			//	       | 4\\5 |
			//	       |N   \O|
			//  9------2------3-----11-----13
			//  |E\   H|A\   D|I\   L|U\   X|
			//  | 0\\1 | 2\\3 | 6\\7 | 9\\8 |
			//  |F   \G|B   \C|J   \K|V   \W|
			//  8------0------1-----10-----12
			//         |Q\   T|
			//	       |10\\11|
			//	       |R   \S|
			//	       4------5

			// Front
			// Bottom-left-front vertex [0]
			{{-side, -side, -side},	{1.0f / 4.0f, 2.0f / 3.0f}},
			// Bottom-right-front vertex [1]
			{{side, -side, -side},	{2.0f / 4.0f, 2.0f / 3.0f}},
			// Top-left-front vertex [2]
			{{-side, side, -side},	{1.0f / 4.0f, 1.0f / 3.0f}},
			// Top-right-front vertex [3]
			{{side, side, -side},	{2.0f / 4.0f, 1.0f / 3.0f}},

			// Bottom
			// Bottom-left-back vertex [4]
			{{-side, -side, side},	{1.0f / 4.0f, 3.0f / 3.0f}},
			// Bottom-right-back vertex [5]
			{{side, -side, side},	{2.0f / 4.0f, 3.0f / 3.0f}},

			// Top
			// Top-left-back vertex [6]
			{{-side, side, side},	{1.0f / 4.0f, 0.0f / 3.0f}},
			// Top-right-back vertex [7]
			{{side, side, side},	{2.0f / 4.0f, 0.0f / 3.0f}},

			// Left
			// Bottom-left-back vertex [8]
			{{-side, -side, side},	{0.0f / 4.0f, 2.0f / 3.0f}},
			// Top-left-back vertex [9]
			{{-side, side, side},	{0.0f / 4.0f, 1.0f / 3.0f}},

			// Right
			// Bottom-right-back vertex [10]
			{{side, -side, side},	{3.0f / 4.0f, 2.0f / 3.0f}},
			// Top-right-back vertex [11]
			{{side, side, side},	{3.0f / 4.0f, 1.0f / 3.0f}},

			// Back
			// Bottom-left-back vertex [12]
			{{-side, -side, side},	{4.0f / 4.0f, 2.0f / 3.0f}},
			// Top-left-back vertex [13]
			{{-side, side, side},	{4.0f / 4.0f, 1.0f / 3.0f}},
		};

		return{
			std::move(vertices),{
				//         6------7
				//         |M\   P|
				//	       | 4\\5 |
				//	       |N   \O|
				//  9------2------3-----11-----13
				//  |E\   H|A\   D|I\   L|U\   X|
				//  | 0\\1 | 2\\3 | 6\\7 | 9\\8 |
				//  |F   \G|B   \C|J   \K|V   \W|
				//  8------0------1-----10-----12
				//         |Q\   T|
				//	       |10\\11|
				//	       |R   \S|
				//	       4------5

				// Front
				0, 2, 1,		2, 3, 1,
				// Bottom
				4, 0, 5,		0, 1, 5,
				// Top
				2, 6, 3,		6, 7, 3,
				// Left
				8, 9, 0,		9, 2, 0,
				// Right
				1, 3, 10,		3, 11, 10,
				// Back
				10, 11, 12,		11, 13, 12
			}
		};
	}
};
