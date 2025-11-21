#pragma once

#include <random>

#include "DrawableStaticStorage.hpp"

class Melon : public DrawableStaticStorage<Melon>
{
public:
	Melon(Graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& distanceDistribution,								// rdist
		std::uniform_real_distribution<float>& sphericalCoordinatePositionDistribution,				// adist
		std::uniform_real_distribution<float>& rotationOfDrawableDistribution,						// ddist
		std::uniform_real_distribution<float>& sphericalCoordinateMovementOfDrawableDistribution,	// odist
		std::uniform_int_distribution<int>& latitudeDistribution,
		std::uniform_int_distribution<int>& longitudeDistribution);
	void update(float dt) noexcept override;
	DirectX::XMMATRIX getTransformXm() const noexcept override;
private:
	// Positional
	float radiusDistanceFromCenter_;
	float roll_ = 0.0f;
	float pitch_ = 0.0f;
	float yaw_ = 0.0f;
	float theta_;
	float phi_;
	float rho_;

	// Speed (delta/s)
	float droll_;
	float dpitch_;
	float dyaw_;
	float dtheta_;
	float dphi_;
	float drho_;
};