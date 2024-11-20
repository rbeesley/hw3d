#pragma once

#include "IndexedTriangleList.hpp"
#include <DirectXMath.h>
#include "AtumMath.hpp"

class Prism
{
public:
	template<class V>
	static IndexedTriangleList<V> make()
	{
		return makeTessellated<V>(24);
	}

	template<class V>
	static IndexedTriangleList<V> makeTessellated(const int longitudinalDivisions)
	{
		namespace dx = DirectX;
		assert(longitudinalDivisions >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / static_cast<float>(longitudinalDivisions);

		// near center
		std::vector<V> vertices;
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto indexCenterNear = static_cast<unsigned short>(vertices.size() - 1);

		// far center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto indexCenterFar = static_cast<unsigned short>(vertices.size() - 1);

		// base vertices
		for (int indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++)
		{
			// near base
			{
				vertices.emplace_back();
				auto vertex = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * static_cast<float>(indexLongitude))
				);
				dx::XMStoreFloat3(&vertices.back().pos, vertex);
			}

			// far base
			{
				vertices.emplace_back();
				auto vertex = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * static_cast<float>(indexLongitude))
				);
				vertex = dx::XMVectorAdd(vertex, offset);
				dx::XMStoreFloat3(&vertices.back().pos, vertex);
			}
		}

		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++)
		{
			const auto index = indexLongitude * 2;
			const auto mod = longitudinalDivisions * 2;
			indices.push_back(index + 2);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back(index + 1 + 2);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back((index + 3) % mod + 2);
			indices.push_back(index + 1 + 2);
		}

		// base indices
		for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++)
		{
			const auto index = indexLongitude * 2;
			const auto mod = longitudinalDivisions * 2;
			indices.push_back(index + 2);
			indices.push_back(indexCenterNear);
			indices.push_back((index + 2) % mod + 2);
			indices.push_back(indexCenterFar);
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