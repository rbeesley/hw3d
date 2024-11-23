#pragma once
#include <array>

#include "AtumMath.hpp"
#include "IndexedTriangleList.hpp"

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> make(std::function<void(std::vector<V>&)> set_attributes = nullptr)
	{
		return makeTessellated<V>(1, 1, set_attributes);
	}

	template<class V>
	static IndexedTriangleList<V> makeTessellated(const size_t divisionsX, const size_t divisionsY, std::function<void(std::vector<V>&)> setAttributes = nullptr)
	{
		namespace dx = DirectX;
		assert(divisionsX >= 1);
		assert(divisionsY >= 1);

		const size_t numberVerticesX = divisionsX + 1;
		const size_t numberVerticesY = divisionsY + 1;
		std::vector<V> vertices(numberVerticesX * numberVerticesY);
		{
			constexpr float height = 2.0f;
			constexpr float width = 2.0f;
			constexpr float sideX = width / 2.0f;
			constexpr float sideY = height / 2.0f;
			const float divisionSizeX = width / static_cast<float>(divisionsX);
			const float divisionSizeY = height / static_cast<float>(divisionsY);
			const auto bottomLeft = dx::XMVectorSet(-sideX, -sideY, 0.0f, 0.0f);

			for (size_t y = 0, i = 0; y < numberVerticesY; y++)
			{
				const float yPos = static_cast<float>(y) * divisionSizeY;
				for (size_t x = 0; x < numberVerticesX; x++, i++)
				{
					const auto vertex = dx::XMVectorAdd(
						bottomLeft,
						dx::XMVectorSet(static_cast<float>(x) * divisionSizeX, yPos, 0.0f, 0.0f)
					);
					dx::XMStoreFloat3(&vertices[i].pos, vertex);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(static_cast<std::vector<unsigned short>::size_type>(sq(divisionsX * divisionsY)) * 6);
		{
			const auto vertexPosToIndex = [numberVerticesX](const size_t x, const size_t y)
			{
				return static_cast<unsigned short>(y * numberVerticesX + x);
			};

			for (size_t y = 0; y < divisionsY; y++)
			{
				for (size_t x = 0; x < divisionsX; x++)
				{
					const std::array<unsigned short, 4> indexArray =
					{
						vertexPosToIndex(x, y),
						vertexPosToIndex(x + 1, y),
						vertexPosToIndex(x, y + 1),
						vertexPosToIndex(x + 1, y + 1)
					};
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		if (setAttributes) {
			setAttributes(vertices);
		}

		return
		{
			std::move(vertices),
			std::move(indices)
		};
	}
};
