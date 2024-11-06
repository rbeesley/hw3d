#pragma once

#include <random>

#include "DrawableStaticStorage.h"

class skinned_box : public drawable_static_storage<skinned_box>
{
public:
	skinned_box(graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& distance_distribution,									// rdist
		std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
		std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
		std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution	// odist
		);

	skinned_box(const skinned_box&) = delete;
	skinned_box& operator=(const skinned_box&) = delete;
	skinned_box(const skinned_box&&) = delete;
	skinned_box& operator=(const skinned_box&&) = delete;

	void update(float dt) noexcept override;
	DirectX::XMMATRIX get_transform_xm() const noexcept override;
private:
	// Positional
	float radius_distance_from_center_;
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