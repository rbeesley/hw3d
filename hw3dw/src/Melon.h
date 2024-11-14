#pragma once

#include <random>

#include "DrawableStaticStorage.h"

class melon : public drawable_static_storage<melon>
{
public:
	melon(graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& distance_distribution,									// rdist
		std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
		std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
		std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution,	// odist
		std::uniform_int_distribution<int>& latitude_distribution,
		std::uniform_int_distribution<int>& longitude_distribution);
	void update(float dt) noexcept override;
	DirectX::XMMATRIX get_transform_xm() const noexcept override;
private:
	// Positional
	float distance_;
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