#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "AtumMath.h"

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> make()
	{
		return makeTessellated<V>(24);
	}

	template<class V>
	static IndexedTriangleList<V> makeTessellated(const int longitudinalDivisions, std::function<void(std::vector<V>&)> setAttributes = nullptr) {
		namespace dx = DirectX;
		assert(longitudinalDivisions >= 3);
		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / static_cast<float>(longitudinalDivisions);

		// base vertices
		std::vector<V> vertices;
		for (int indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++) {
			vertices.emplace_back();
			auto vertex = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitudeAngle * static_cast<float>(indexLongitude)));
			dx::XMStoreFloat3(&vertices.back().pos, vertex);
		}

		// the center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto indexCenter = static_cast<unsigned short>(vertices.size() - 1);

		// the tip
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto indexTip = static_cast<unsigned short>(vertices.size() - 1);

		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++) {
			indices.push_back(indexCenter);
			indices.push_back((indexLongitude + 1) % longitudinalDivisions);
			indices.push_back(indexLongitude);
		}

		// cone indices
		for (unsigned short indexLongitude = 0; indexLongitude < longitudinalDivisions; indexLongitude++) {
			indices.push_back(indexLongitude);
			indices.push_back((indexLongitude + 1) % longitudinalDivisions);
			indices.push_back(indexTip);
		}

		if (setAttributes) {
			setAttributes(vertices);
		}

		return IndexedTriangleList<V>{std::move(vertices), std::move(indices)};
	}
};