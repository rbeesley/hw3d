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

		//    6-------7
		//   /|      /|
		//  2-------3 |
		//  | |     | |
		//  | 4-----|-5
		//  |/      |/
		//  0-------1
		std::vector<dx::XMFLOAT3> vertices = {
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

		return
		{
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
};
