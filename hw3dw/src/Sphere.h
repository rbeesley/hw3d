#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "AtumMath.h"

class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> make()
	{
		return makeTessellated<V>(12, 24);
	}

	template<class V>
	static IndexedTriangleList<V> makeTessellated(const int latitudinalDivisions, const int longitudinalDivisions)
	{
		namespace dx = DirectX;
		assert(latitudinalDivisions >= 3);
		assert(longitudinalDivisions >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float latitudinalAngle = PI / static_cast<float>(latitudinalDivisions);
		const float longitudeAngle = 2.0f * PI / static_cast<float>(longitudinalDivisions);

		std::vector<V> vertices;
		for (int indexLatitude = 1; indexLatitude < latitudinalDivisions; indexLatitude++)
		{
			const auto latitudeBase = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationX(latitudinalAngle * static_cast<float>(indexLatitude))
			);

			for (int indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++)
			{
				vertices.emplace_back();
				auto v = dx::XMVector3Transform(
					latitudeBase,
					dx::XMMatrixRotationZ(longitudeAngle * static_cast<float>(indexLongitude))
				);

				dx::XMStoreFloat3(&vertices.back().pos, v);
			}
		}

		// add the cap vertices
		const auto indexNorthPole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, base);

		const auto indexSouthPole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		dx::XMStoreFloat3(&vertices.back().pos, dx::XMVectorNegate(base));

		// ReSharper disable once CppLambdaCaptureNeverUsed
		const auto calcIndex = [latitudinalDivisions, longitudinalDivisions](const unsigned short indexLatitude, const unsigned short indexLongitude)
			{ return indexLatitude * longitudinalDivisions + indexLongitude; };

		std::vector<unsigned short> indices;
		for (unsigned short indexLatitude = 0; indexLatitude < latitudinalDivisions - 2; indexLatitude++)
		{
			for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions - 1; indexLongitude++)
			{
				indices.push_back(calcIndex(indexLatitude, indexLongitude));
				indices.push_back(calcIndex(indexLatitude + 1, indexLongitude));
				indices.push_back(calcIndex(indexLatitude, indexLongitude + 1));
				indices.push_back(calcIndex(indexLatitude, indexLongitude + 1));
				indices.push_back(calcIndex(indexLatitude + 1, indexLongitude));
				indices.push_back(calcIndex(indexLatitude + 1, indexLongitude + 1));
			}

			// wrap band
			indices.push_back(calcIndex(indexLatitude, longitudinalDivisions - 1));
			indices.push_back(calcIndex(indexLatitude + 1, longitudinalDivisions - 1));
			indices.push_back(calcIndex(indexLatitude, 0));
			indices.push_back(calcIndex(indexLatitude, 0));
			indices.push_back(calcIndex(indexLatitude + 1, longitudinalDivisions - 1));
			indices.push_back(calcIndex(indexLatitude + 1, 0));
		}

		// cap fans
		for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions - 1; indexLongitude++)
		{
			// north
			indices.push_back(indexNorthPole);
			indices.push_back(calcIndex(0, indexLongitude));
			indices.push_back(calcIndex(0, indexLongitude + 1));

			// south
			indices.push_back(calcIndex(latitudinalDivisions - 2, indexLongitude + 1));
			indices.push_back(calcIndex(latitudinalDivisions - 2, indexLongitude));
			indices.push_back(indexSouthPole);
		}

		// wrap triangles
		// north
		indices.push_back(indexNorthPole);
		indices.push_back(calcIndex(0, longitudinalDivisions - 1));
		indices.push_back(calcIndex(0, 0));

		// south
		indices.push_back(calcIndex(latitudinalDivisions - 2, 0));
		indices.push_back(calcIndex(latitudinalDivisions - 2, longitudinalDivisions - 1));
		indices.push_back(indexSouthPole);

		return
		{
			std::move(vertices),
			std::move(indices)
		};
	}
};